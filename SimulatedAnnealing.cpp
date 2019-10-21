#include "SimulatedAnnealing.h"
#include <random>
#include <queue>
#include <cstring>

typedef std::mt19937_64 Rng;

ProblemGraph::ProblemGraph(Graph &graph) : graph(graph), nonFixedNodes(), removedGroups(), labels(nullptr) {
    labels = (Label*) malloc(sizeof(Label) * graph.getNbNodes());
    memset(labels, NONFIXED, sizeof(Label) * graph.getNbNodes());

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (labels[node] != NONFIXED)
            continue; // Already done

        if (graph.isPreColored(node)) {
            labels[node] = PRECOLORED;
            continue;
        }
        if (graph.getEdges(node).size() == 1) {
            RemovedGroup removedGroup(node);
            labels[node] = REMOVED;

            unsigned int previous = node;
            unsigned int next = graph.getEdges(node)[0];
            while (next != -1) {
                const std::vector<unsigned int> &vector = graph.getEdges(next);
                if (!graph.isPreColored(next) && vector.size() == 2) {
                    removedGroup.nodes.push_back(next);
                    labels[next] = REMOVED;

                    if (vector[0] == previous) {
                        previous = next;
                        next = vector[1];
                    } else {
                        previous = next;
                        next = vector[0];
                    }
                } else {
                    removedGroup.connection = next;
                    break;
                }
            }
            removedGroups.push_back(removedGroup);
            continue;
        }
        if (graph.getEdges(node).size() == 2) {
            if (graph.isPreColored(graph.getEdges(node)[0]) && graph.isPreColored(graph.getEdges(node)[1])) {
                enclosedNodes.push_back(node);
                labels[node] = ENCLOSED;
            }
            continue;
        }
    }

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (labels[node] == NONFIXED)
            nonFixedNodes.push_back(node);
    }
}

unsigned int ProblemGraph::getHappyVertices() const {
    unsigned int nbHappy = 0;

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        bool happy;
        switch (labels[node]) {
            case PRECOLORED:
            case NONFIXED:
                happy = true;
                for (unsigned int adj : graph.getEdges(node)) {
                    switch (labels[adj]) {
                        case PRECOLORED:
                        case NONFIXED:
                            if (graph.getColor(adj) != graph.getColor(node))
                                happy = false;
                            break;
                        case REMOVED:
                            break;
                        case ENCLOSED:
                            happy = false; //TODO: improve enclosed counting
                            break;
                    }
                    if (!happy)
                        break;
                }
                if (happy)
                    ++nbHappy;
                break;
            case REMOVED:
                ++nbHappy;
                break;
            case ENCLOSED:
                break;
        }
    }

    return nbHappy;
}

unsigned int ProblemGraph::colorRemoved() {
    for (const RemovedGroup &removedGroup: removedGroups) {
        for (unsigned int node: removedGroup.nodes) {
            graph.color(node, graph.getColor(removedGroup.connection));
        }
    }

    for (unsigned int enclosedNode: enclosedNodes) {
        // TODO
    }

    return graph.getHappyVertices();
}

void initRandomColoring(ProblemGraph &pGraph, Rng &rng) {
    std::uniform_int_distribution<unsigned int> distribution(1, pGraph.graph.getNbColors());
    for (unsigned int node: pGraph.nonFixedNodes) {
        if (!pGraph.graph.isPreColored(node))
            pGraph.graph.color(node, distribution(rng));
    }
}

std::vector<Group> generateGroups(const ProblemGraph &pGraph) {
    std::vector<Group> groups;

    int group[pGraph.nonFixedNodes.size()];
    memset(&group, 0, sizeof(int) * pGraph.nonFixedNodes.size());

    int groupCounter = 0;

    for (unsigned int nodeIdx = 0; nodeIdx < (unsigned int)pGraph.nonFixedNodes.size(); ++nodeIdx) {
        if (group[nodeIdx] != 0)
            continue;

        unsigned int node = pGraph.nonFixedNodes[nodeIdx];

        Group newGroup(pGraph.graph.getColor(node), node);
        group[nodeIdx] = ++groupCounter;
        std::queue<unsigned int> toCheck;
        toCheck.push(node);

        while (!toCheck.empty()) {
            const unsigned int check = toCheck.front();
            toCheck.pop();
            for (unsigned int adj: pGraph.graph.getEdges(check)) {
                if (pGraph.graph.getColor(adj) != newGroup.color) {
                    newGroup.adjColors.insert(pGraph.graph.getColor(adj));
                    continue;
                }

                if (pGraph.graph.isPreColored(adj))
                    continue;

                if (pGraph.labels[adj] == NONFIXED) {
                    auto adjIdx = *std::find(pGraph.nonFixedNodes.begin(), pGraph.nonFixedNodes.end(), adj);
                    group[adjIdx] = groupCounter;
                }

                newGroup.nodes.push_back(adj);
                toCheck.push(adj);
            }
        }
        if (!newGroup.adjColors.empty())
            groups.push_back(newGroup);
    }
    return groups;
}

ProblemGraph generateNeighbour(const ProblemGraph &pGraph, Rng &rng) {
    const std::vector<Group> groups = generateGroups(pGraph);

    ProblemGraph newGraph = pGraph;

    std::uniform_int_distribution<unsigned int> nodeDistr(0, groups.size() - 1);
    const Group group = groups[nodeDistr(rng)];

    std::uniform_int_distribution<unsigned int> colorDistr(0, group.adjColors.size() - 1);
    unsigned int nbColor = colorDistr(rng);
    auto it = group.adjColors.begin();
    for (; nbColor != 0; --nbColor) {
        it++;
    }
    unsigned int color = *it;

    for (unsigned int node: group.nodes) {
        newGraph.graph.color(node, color);
    }

    return newGraph;
}

double swapProbability(unsigned int oldEnergy, unsigned int newEnergy, double temperature) {
    if (newEnergy > oldEnergy)
        return 1;
    return exp(-(newEnergy - oldEnergy) / temperature);
}

double coolTemperature(double temperature) {
    return temperature * 0.995;
}

unsigned int simulatedAnnealing(Graph &graph, int seed) {
    Rng rng = Rng(seed);
    std::uniform_real_distribution<double> swapDistr(0,1);
    ProblemGraph pGraph(graph);

    FILE* f = fopen("progress.txt", "w");

    int maxIter = 1000;
    double temperature = 500;
    initRandomColoring(pGraph, rng);

    const unsigned int nodes = graph.getNbNodes();
    unsigned int energy = nodes - pGraph.getHappyVertices();

    for (int i = 0; i < maxIter; ++i) {
        ProblemGraph neighbour = generateNeighbour(pGraph, rng);
        unsigned int newEnergy = nodes - neighbour.getHappyVertices();
        if (swapDistr(rng) < swapProbability(energy, newEnergy, temperature)) {
            fprintf(f, "%u\n", newEnergy);
            pGraph = neighbour; // FIXME problem
            energy = newEnergy;
        }
        temperature = coolTemperature(temperature);
    }
    fclose(f);

    pGraph.colorRemoved();

    return nodes - energy;
}

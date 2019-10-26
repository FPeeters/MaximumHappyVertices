#include "SimulatedAnnealing.h"
#include <random>
#include <queue>
#include <cstring>

typedef std::mt19937_64 Rng;

void initRandomColoring(Graph &graph, Rng &rng) {
    std::uniform_int_distribution<unsigned int> distribution(1, graph.getNbColors());
    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (!graph.isPreColored(node))
            graph.color(node, distribution(rng));
    }
}

std::vector<Group> generateGroups(const Graph &graph) {
    std::vector<Group> groups;
    int* group = (int*) calloc(sizeof(int), graph.getNbNodes());
    int groupCounter = 0;
    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (graph.isPreColored(node)) {
            group[node] = -1;
            continue;
        }
        if (group[node] != 0)
            continue;

        Group newGroup(graph.getColor(node), node);
        group[node] = ++groupCounter;
        std::queue<unsigned int> toCheck;
        toCheck.push(node);

        while (!toCheck.empty()) {
            const unsigned int check = toCheck.front();
            toCheck.pop();
            for (unsigned int adj: graph.getEdges(check)) {
                if (graph.getColor(adj) != newGroup.color)
                    newGroup.adjColors.insert(graph.getColor(adj));

                if (graph.getColor(adj) != newGroup.color || group[adj] != 0)
                    continue;
                if (graph.isPreColored(adj)) {
                    group[adj] = -1;
                    continue;
                }

                group[adj] = groupCounter;
                newGroup.nodes.push_back(adj);
                toCheck.push(adj);
            }
        }
        if (!newGroup.adjColors.empty())
            groups.push_back(newGroup);
    }
    return groups;
}

Graph generateNeighbour(const Graph &graph, Rng &rng) {
    const std::vector<Group> groups = generateGroups(graph);

    Graph newGraph = graph;

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
        newGraph.color(node, color);
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
    initRandomColoring(graph, rng);

    int maxIter = 1000;
    double temperature = 500;

    const unsigned int nodes = graph.getNbNodes();
    unsigned int energy = nodes - graph.getHappyVertices();

    for (int i = 0; i < maxIter; ++i) {
        Graph neighbour = generateNeighbour(graph, rng);
        unsigned int newEnergy = nodes - neighbour.getHappyVertices();
        if (swapDistr(rng) < swapProbability(energy, newEnergy, temperature)) {
            printf("Swapped from %u to %u\n", energy, newEnergy);
            graph = neighbour;
            energy = newEnergy;
        }
        temperature = coolTemperature(temperature);
    }
    return nodes - energy;
}

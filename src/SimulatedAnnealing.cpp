#include "SimulatedAnnealing.h"
#include "ConstructiveAlgs.h"
#include <random>
#include <queue>
#include <fstream>
#include <ctime>
#include <iostream>

typedef std::mt19937_64 Rng;

void initRandomColoring(Graph &graph, Rng &rng) {
    std::uniform_int_distribution<unsigned int> distribution(1, graph.getNbColors());
    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (!graph.isPreColored(node))
            graph.color(node, distribution(rng));
    }
}

struct colored_group {
    unsigned int color;
    std::vector<unsigned int> nodes;
    std::set<unsigned int> adjColors;

    explicit colored_group(unsigned int color, unsigned int node) : color(color), nodes(), adjColors() {
        nodes.push_back(node);
    }

    bool removeNode(unsigned int node) {
        nodes.erase(std::find(nodes.begin(), nodes.end(), node));
        return nodes.empty();
    }

    void mergeGroup(const colored_group &other) {
        if (this->color != other.color)
            throw std::runtime_error("Merging different color groups");
        nodes.insert(nodes.end(), other.nodes.begin(), other.nodes.end());
        adjColors.insert(other.adjColors.begin(), other.adjColors.end());
    }
};

struct grouped_graph {
    Graph graph;
    std::vector<colored_group> groups;
    std::vector<unsigned int> nodeGroups;

    explicit grouped_graph(const Graph &original) : graph(original), groups(),
                                                    nodeGroups(original.getNbNodes(), original.getNbNodes()) {
        generateGroups(original);
    }

    colored_group &getGroup(unsigned int node) {
        return groups[nodeGroups[node]];
    }

    void removeGroup(unsigned int index) {
        groups.erase(groups.begin() + index);
        for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
            if (nodeGroups[node] > index)
                nodeGroups[node] -= 1;
        }
    }

private:
    void generateGroups(const Graph &original) {
        int groupCounter = -1;
        for (unsigned int node = 0; node < original.getNbNodes(); ++node) {
            if (original.isPreColored(node)) {
                nodeGroups[node] = -1;
                continue;
            }
            if (nodeGroups[node] != graph.getNbNodes())
                continue;

            colored_group newGroup(original.getColor(node), node);
            nodeGroups[node] = ++groupCounter;
            std::queue<unsigned int> toCheck;
            toCheck.push(node);

            while (!toCheck.empty()) {
                const unsigned int check = toCheck.front();
                toCheck.pop();
                for (unsigned int adj: original.getEdges(check)) {
                    if (original.getColor(adj) != newGroup.color)
                        newGroup.adjColors.insert(original.getColor(adj));

                    if (original.getColor(adj) != newGroup.color || nodeGroups[adj] != graph.getNbNodes())
                        continue;
                    if (original.isPreColored(adj)) {
                        nodeGroups[adj] = -1;
                        continue;
                    }

                    nodeGroups[adj] = groupCounter;
                    newGroup.nodes.push_back(adj);
                    toCheck.push(adj);
                }
            }
            groups.push_back(newGroup);
        }
    }
};

void splitGroup(grouped_graph &groupedGraph, unsigned int groupIdx, Rng &rng) {
    colored_group group = groupedGraph.groups[groupIdx];
    std::vector<unsigned int> candidates;

    for (auto node: group.nodes) {
        for (auto adj: groupedGraph.graph.getEdges(node)) {
            if (groupedGraph.graph.getColor(adj) != group.color) {
                candidates.push_back(node);
                break;
            }
        }
    }

    if (candidates.size() < 2)
        return;

    std::uniform_int_distribution<unsigned int> candidateDistr(0, candidates.size() - 1);
    const unsigned int leftSeedIdx = candidateDistr(rng);
    unsigned int rightSeedIdx;
    do rightSeedIdx = candidateDistr(rng);
    while (rightSeedIdx == leftSeedIdx);

    const unsigned int leftSeed = candidates[leftSeedIdx];
    const unsigned int rightSeed = candidates[rightSeedIdx];

    std::set<unsigned int> leftGroup;
    leftGroup.insert(leftSeed);
    std::set<unsigned int> rightGroup;
    rightGroup.insert(rightSeed);

    std::sort(group.nodes.begin(), group.nodes.end());

    const std::vector<unsigned int> &leftInitEdges = groupedGraph.graph.getEdges(leftSeed);
    std::vector<unsigned int> diffEdges(leftInitEdges.size());
    auto iter = std::set_intersection(leftInitEdges.begin(), leftInitEdges.end(), group.nodes.begin(),
                                      group.nodes.end(), diffEdges.begin());
    diffEdges.resize(iter - diffEdges.begin());
    std::set<unsigned int> leftEdges(diffEdges.begin(), diffEdges.end());
    leftEdges.erase(rightSeed);

    const std::vector<unsigned int> &rightInitEdges = groupedGraph.graph.getEdges(rightSeed);
    diffEdges.clear();
    diffEdges.resize(rightInitEdges.size());
    iter = std::set_intersection(rightInitEdges.begin(), rightInitEdges.end(), group.nodes.begin(), group.nodes.end(),
                                 diffEdges.begin());
    diffEdges.resize(iter - diffEdges.begin());
    std::set<unsigned int> rightEdges(diffEdges.begin(), diffEdges.end());
    rightEdges.erase(leftSeed);

    int nbNodes = group.nodes.size();

    while (nbNodes > 2) {
        if (leftEdges.empty()) {
            std::vector<unsigned int> diff(group.nodes.size());
            iter = std::set_difference(group.nodes.begin(), group.nodes.end(), leftGroup.begin(), leftGroup.end(),
                                       diff.begin());
            diff.resize(iter - diff.begin());
            rightGroup.insert(diff.begin(), diff.end());
            break;
        } else if (rightEdges.empty()) {
            std::vector<unsigned int> diff(group.nodes.size());
            iter = std::set_difference(group.nodes.begin(), group.nodes.end(), rightGroup.begin(), rightGroup.end(),
                                       diff.begin());
            diff.resize(iter - diff.begin());
            leftGroup.insert(diff.begin(), diff.end());
            break;
        }

        std::uniform_int_distribution<unsigned int> edgeDistr(0, leftEdges.size() + rightEdges.size() - 2);
        unsigned int edge = edgeDistr(rng);

        if (edge >= leftEdges.size()) {
            edge -= leftEdges.size();

            auto it = rightEdges.begin();
            for (; edge != 0; --edge) it++;
            const unsigned int adj = *it;

            rightGroup.insert(adj);

            const std::vector<unsigned int> &edges = groupedGraph.graph.getEdges(adj);
            std::vector<unsigned int> uni(edges.size());
            iter = std::set_intersection(group.nodes.begin(), group.nodes.end(), edges.begin(), edges.end(),
                                         uni.begin());
            rightEdges.insert(uni.begin(), iter);

            std::vector<unsigned int> firstDiff(rightEdges.size());
            std::vector<unsigned int> secondDiff(rightEdges.size());
            iter = std::set_difference(rightEdges.begin(), rightEdges.end(), rightGroup.begin(), rightGroup.end(),
                                       firstDiff.begin());
            firstDiff.resize(iter - firstDiff.begin());
            iter = std::set_difference(firstDiff.begin(), firstDiff.end(), leftGroup.begin(), leftGroup.end(),
                                       secondDiff.begin());
            rightEdges.clear();
            rightEdges.insert(secondDiff.begin(), iter);

            leftEdges.erase(adj);
        } else {
            auto it = leftEdges.begin();
            for (; edge != 0; --edge) it++;
            const unsigned int adj = *it;

            leftGroup.insert(adj);

            const std::vector<unsigned int> &edges = groupedGraph.graph.getEdges(adj);
            std::vector<unsigned int> intersect(edges.size());
            iter = std::set_intersection(group.nodes.begin(), group.nodes.end(), edges.begin(), edges.end(),
                                         intersect.begin());
            leftEdges.insert(intersect.begin(), iter);

            std::vector<unsigned int> firstDiff(leftEdges.size());
            std::vector<unsigned int> secondDiff(leftEdges.size());
            iter = std::set_difference(leftEdges.begin(), leftEdges.end(), leftGroup.begin(), leftGroup.end(),
                                       firstDiff.begin());
            firstDiff.resize(iter - firstDiff.begin());
            iter = std::set_difference(firstDiff.begin(), firstDiff.end(), rightGroup.begin(), rightGroup.end(),
                                       secondDiff.begin());
            leftEdges.clear();
            leftEdges.insert(secondDiff.begin(), iter);

            rightEdges.erase(adj);
        }

        --nbNodes;
    }

    std::set<unsigned int> adjColors;
    for (unsigned int node: leftGroup) {
        for (unsigned int adj: groupedGraph.graph.getEdges(node))
            adjColors.insert(groupedGraph.graph.getColor(adj));
    }

    std::uniform_int_distribution<unsigned int> colorDistr(0, adjColors.size() - 1);
    unsigned int nbColor = colorDistr(rng);
    auto it = adjColors.begin();
    for (; nbColor != 0; --nbColor) it++;
    unsigned int color = *it;

    for (unsigned int node: leftGroup) {
        groupedGraph.graph.color(node, color);
        groupedGraph.nodeGroups[node] = groupedGraph.groups.size();
    }

    groupedGraph.removeGroup(groupIdx);

    colored_group newLeftGroup(color, *leftGroup.begin());
    newLeftGroup.nodes.insert(newLeftGroup.nodes.end(), ++leftGroup.begin(), leftGroup.end());

    for (unsigned int node: leftGroup) {
        for (unsigned int adj: groupedGraph.graph.getEdges(node)) {
            if (!groupedGraph.graph.isPreColored(adj)
                && !rightGroup.contains(adj)
                && groupedGraph.nodeGroups[adj] != groupedGraph.nodeGroups[node]
                && groupedGraph.graph.getColor(adj) == color) {

                colored_group mergeGroup = groupedGraph.getGroup(adj);

                newLeftGroup.mergeGroup(mergeGroup);
                groupedGraph.removeGroup(groupedGraph.nodeGroups[adj]);
                for (unsigned int mergeNode: mergeGroup.nodes)
                    groupedGraph.nodeGroups[mergeNode] = groupedGraph.nodeGroups[node];
            } else if (groupedGraph.graph.getColor(adj) != color)
                newLeftGroup.adjColors.insert(groupedGraph.graph.getColor(adj));
        }
    }

    groupedGraph.groups.push_back(newLeftGroup);

    adjColors.clear();
    for (unsigned int node: rightGroup) {
        for (unsigned int adj: groupedGraph.graph.getEdges(node))
            adjColors.insert(groupedGraph.graph.getColor(adj));
    }

    colorDistr = std::uniform_int_distribution<unsigned int>(0, adjColors.size() - 1);
    nbColor = colorDistr(rng);
    it = adjColors.begin();
    for (; nbColor != 0; --nbColor) it++;
    color = *it;

    for (unsigned int node: rightGroup) {
        groupedGraph.graph.color(node, color);
        groupedGraph.nodeGroups[node] = groupedGraph.groups.size();
    }

    colored_group newRightGroup(color, *rightGroup.begin());
    newRightGroup.nodes.insert(newRightGroup.nodes.end(), ++rightGroup.begin(), rightGroup.end());

    for (unsigned int node: rightGroup) {
        for (unsigned int adj: groupedGraph.graph.getEdges(node)) {
            if (!groupedGraph.graph.isPreColored(adj)
                && groupedGraph.nodeGroups[adj] != groupedGraph.nodeGroups[leftSeed]
                && groupedGraph.nodeGroups[adj] != groupedGraph.nodeGroups[node]
                && groupedGraph.graph.getColor(adj) == color) {

                colored_group mergeGroup = groupedGraph.getGroup(adj);

                newRightGroup.mergeGroup(mergeGroup);
                groupedGraph.removeGroup(groupedGraph.nodeGroups[adj]);
                for (unsigned int mergeNode: mergeGroup.nodes)
                    groupedGraph.nodeGroups[mergeNode] = groupedGraph.nodeGroups[node];
            } else if (groupedGraph.graph.getColor(adj) != color)
                newRightGroup.adjColors.insert(groupedGraph.graph.getColor(adj));
        }
    }

    if (newLeftGroup.color == newRightGroup.color) {
        newLeftGroup.mergeGroup(newRightGroup);
        groupedGraph.groups[groupedGraph.groups.size() - 1] = newLeftGroup;

        for (unsigned int node: newRightGroup.nodes)
            groupedGraph.nodeGroups[node] = groupedGraph.groups.size() - 1;
    } else {
        groupedGraph.groups.push_back(newRightGroup);
    }
}

void swapDegreeBased(grouped_graph &groupedGraph, Rng &rng) {
    unsigned int nbEdges = 0;
    for (unsigned int node = 0; node < groupedGraph.graph.getNbNodes(); ++node) {
        if (!groupedGraph.graph.isPreColored(node))
            nbEdges += groupedGraph.graph.getEdges(node).size();
    }
    if (nbEdges == 0)
        return;

    std::uniform_int_distribution<unsigned int> degreeDistr(0, nbEdges - 1);
    unsigned int pickedDegree = degreeDistr(rng);

    unsigned int pickedNode = -1;
    for (unsigned int node = 0; node < groupedGraph.graph.getNbNodes(); ++node) {
        if (!groupedGraph.graph.isPreColored(node)) {
            const unsigned int degree = groupedGraph.graph.getEdges(node).size();
            if (pickedDegree >= degree)
                pickedDegree -= degree;
            else {
                pickedNode = node;
                break;
            }
        }
    }

    std::set<unsigned int> adjColors;
    for (unsigned int adj: groupedGraph.graph.getEdges(pickedNode)) {
        if (groupedGraph.graph.getColor(adj) != groupedGraph.graph.getColor(pickedNode))
            adjColors.insert(groupedGraph.graph.getColor(adj));
    }

    if (adjColors.empty()) {
        std::uniform_int_distribution<unsigned int> colorDistr(1, groupedGraph.graph.getNbColors());
        unsigned int color = colorDistr(rng);
        groupedGraph.graph.color(pickedNode, color);

        if (groupedGraph.getGroup(pickedNode).removeNode(pickedNode))
            groupedGraph.removeGroup(groupedGraph.nodeGroups[pickedNode]);

        colored_group newGroup(color, pickedNode);
        groupedGraph.nodeGroups[pickedNode] = groupedGraph.groups.size();
        groupedGraph.groups.push_back(newGroup);
    } else {
        std::uniform_int_distribution<unsigned int> colorDistr(0, adjColors.size() - 1);
        unsigned int nbColor = colorDistr(rng);
        auto it = adjColors.begin();
        for (; nbColor != 0; --nbColor) it++;
        unsigned int color = *it;
        groupedGraph.graph.color(pickedNode, color);

        if (groupedGraph.getGroup(pickedNode).removeNode(pickedNode))
            groupedGraph.removeGroup(groupedGraph.nodeGroups[pickedNode]);

        colored_group newGroup(color, pickedNode);
        newGroup.adjColors = adjColors;
        groupedGraph.nodeGroups[pickedNode] = groupedGraph.groups.size();
        std::set<unsigned int> mergeGroups;
        for (unsigned int adj: groupedGraph.graph.getEdges(pickedNode)) {
            if (!groupedGraph.graph.isPreColored(adj)
                && groupedGraph.graph.getColor(adj) == color
                && groupedGraph.nodeGroups[adj] != groupedGraph.groups.size()) {

                colored_group group = groupedGraph.getGroup(adj);
                newGroup.mergeGroup(group);
                groupedGraph.removeGroup(groupedGraph.nodeGroups[adj]);
                for (unsigned int node: group.nodes)
                    groupedGraph.nodeGroups[node] = groupedGraph.groups.size();
            }
        }

        groupedGraph.groups.push_back(newGroup);
    }
}

void mergeGroup(grouped_graph &groupedGraph, unsigned int groupIdx, Rng &rng) {
    colored_group group = groupedGraph.groups[groupIdx];

    unsigned int color;
    if (group.adjColors.empty()) {
        std::uniform_int_distribution<unsigned int> colorDistr(1, groupedGraph.graph.getNbColors());
        color = colorDistr(rng);
    } else {
        std::uniform_int_distribution<unsigned int> colorDistr(0, group.adjColors.size() - 1);
        unsigned int nbColor = colorDistr(rng);
        auto it = group.adjColors.begin();
        for (; nbColor != 0; --nbColor) it++;
        color = *it;
    }

    for (unsigned int node: group.nodes)
        groupedGraph.graph.color(node, color);

    group.color = color;
    group.adjColors.clear();

    std::vector<unsigned int> originalNodes = group.nodes;
    for (unsigned int node: originalNodes) {
        for (unsigned int adj: groupedGraph.graph.getEdges(node)) {
            if (!groupedGraph.graph.isPreColored(adj)
                && groupedGraph.nodeGroups[adj] != groupedGraph.nodeGroups[node]
                && groupedGraph.graph.getColor(adj) == color) {

                colored_group mergeGroup = groupedGraph.getGroup(adj);

                group.mergeGroup(mergeGroup);
                groupedGraph.removeGroup(groupedGraph.nodeGroups[adj]);
                for (unsigned int mergeNode: mergeGroup.nodes)
                    groupedGraph.nodeGroups[mergeNode] = groupedGraph.nodeGroups[node];
            } else if (groupedGraph.graph.getColor(adj) != color)
                group.adjColors.insert(groupedGraph.graph.getColor(adj));
        }
    }

    groupedGraph.groups[groupedGraph.nodeGroups[*group.nodes.begin()]] = group;
}

grouped_graph
generateNeighbour(const grouped_graph &groupedGraph, Rng &rng, const config &config) {
    grouped_graph newGraph(groupedGraph);

    std::uniform_real_distribution<double> splitDistr(0, 1);
    const double val = splitDistr(rng);

    if (val < config.splitGroupPerc) {
        if (newGraph.groups.size() <= 1)
            return newGraph;
        std::uniform_int_distribution<unsigned int> groupDistr(0, newGraph.groups.size() - 1);
        splitGroup(newGraph, groupDistr(rng), rng);
    } else if (val < config.splitGroupPerc + config.swapDegreePerc) {
        swapDegreeBased(newGraph, rng);
    } else {
        if (newGraph.groups.size() <= 1)
            return newGraph;
        std::uniform_int_distribution<unsigned int> groupDistr(0, newGraph.groups.size() - 1);
        mergeGroup(newGraph, groupDistr(rng), rng);
    }

    return newGraph;
}

double swapProbability(unsigned int oldEnergy, unsigned int newEnergy, double temperature) {
    if (newEnergy < oldEnergy)
        return 1;
    if (temperature < 0.1)
        return 0;
    return exp(-(newEnergy - oldEnergy) / temperature);
}

double coolTemperature(const config &config, const unsigned int iteration, const clock_t clock) {
    if (config.timeLimit != -1)
        if (clock > (1 - config.zeroIterations) * config.timeLimit * CLOCKS_PER_SEC)
            return 0;
        else
            return config.initTemp -
                   config.initTemp / (config.timeLimit * CLOCKS_PER_SEC * (1 - config.zeroIterations)) * clock;
    else if (iteration > (1 - config.zeroIterations) * config.maxIterations)
        return 0;
    else
        return config.initTemp - config.initTemp / (config.maxIterations * (1 - config.zeroIterations)) * iteration;
}

unsigned int simulatedAnnealing(Graph &graph, const config &config) {
    Rng rng = Rng(config.seed);
    std::uniform_real_distribution<double> swapDistr(0, 1);

    float startTime = clock();

    switch (config.initAlgorithm) {
        case config::random:
            initRandomColoring(graph, rng);
            break;
        case config::greedy:
            greedyMHV(graph);
            break;
        case config::growth:
            growthMHV(graph, config);
            break;
        case config::best:
            unsigned int growth = growthMHV(graph, config);
            Graph copy = graph;
            if (greedyMHV(graph) < growth)
                graph = copy;
            break;
    }

    std::cout << "Initial solution done after " << (clock() - startTime) / CLOCKS_PER_SEC << " sec" << std::endl;
    startTime = clock();

    std::ofstream f;
    if (config.outputProgress)
        f.open("progress.txt");

    double temperature = config.initTemp;

    const unsigned int nodes = graph.getNbNodes();
    unsigned int energy = nodes - graph.getHappyVertices();

    unsigned int currBestEnergy = energy;
    Graph currBestGraph = graph;

    grouped_graph groupedGraph(graph);

    std::cout << "Built groups after " << (clock() - startTime) / CLOCKS_PER_SEC << " sec" << std::endl;
    startTime = clock();

    unsigned int i = 0;
    clock_t startClock = clock();
    clock_t maxClocks = config.timeLimit != -1 ? config.timeLimit * CLOCKS_PER_SEC : LONG_MAX;
    clock_t clocks = 0;
    while ((config.maxIterations == -1 || i < config.maxIterations) &&
           (config.timeLimit == -1 || clocks < maxClocks)) {
        grouped_graph neighbour = generateNeighbour(groupedGraph, rng, config);
        unsigned int newEnergy = nodes - neighbour.graph.getHappyVertices();
        if (swapDistr(rng) < swapProbability(energy, newEnergy, temperature)) {
            groupedGraph = neighbour;
            energy = newEnergy;

            if (energy < currBestEnergy) {
                currBestEnergy = energy;
                currBestGraph = groupedGraph.graph;
            }
        }
        temperature = coolTemperature(config, i, clocks);

        if (config.outputProgress)
            f << i << "\t" << temperature << "\t" << energy << "\t" << currBestEnergy << std::endl;

        ++i;
        clocks = clock() - startClock;
    }

    std::cout << i << " iterations done after " << (clock() - startTime) / CLOCKS_PER_SEC << " sec" << std::endl;

    if (config.outputProgress)
        f.close();

    graph = currBestGraph;
    return nodes - currBestEnergy;
}

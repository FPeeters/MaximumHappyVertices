#include "SimulatedAnnealing.h"
#include <random>
#include <queue>
#include <fstream>

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

void splitGroup(Graph &graph, Group &group,Rng &rng) {
    std::vector<unsigned int> candidates;

    for (auto node: group.nodes) {
        for (auto adj: graph.getEdges(node)) {
            if (graph.getColor(adj) != group.color) {
                candidates.push_back(node);
                break;
            }
        }
    }

    if (candidates.size() < 2)
        return;

	std::sort(group.nodes.begin(), group.nodes.end());

    std::uniform_int_distribution<unsigned int> candidateDistr(0, candidates.size() - 1);
    const unsigned int leftSeedIdx = candidateDistr(rng);
    unsigned int rightSeedIdx;
    do rightSeedIdx = candidateDistr(rng);
    while (rightSeedIdx == leftSeedIdx);

    const unsigned int leftSeed = group.nodes[leftSeedIdx];
    const unsigned int rightSeed = group.nodes[rightSeedIdx];

    std::set<unsigned int> leftGroup;
    leftGroup.insert(leftSeed);
    std::set<unsigned int> rightGroup;
    rightGroup.insert(rightSeed);

    const std::vector<unsigned int> &leftInitEdges = graph.getEdges(leftSeed);
    std::vector<unsigned int> diffEdges(leftInitEdges.size());
    auto iter = std::set_intersection(leftInitEdges.begin(), leftInitEdges.end(), group.nodes.begin(), group.nodes.end(), diffEdges.begin());
    diffEdges.resize(iter - diffEdges.begin());
    std::set<unsigned int> leftEdges(diffEdges.begin(), diffEdges.end());
	leftEdges.erase(rightSeed);

    const std::vector<unsigned int>& rightInitEdges = graph.getEdges(rightSeed);
    diffEdges.clear();
    diffEdges.resize(rightInitEdges.size());
	iter = std::set_intersection(rightInitEdges.begin(), rightInitEdges.end(), group.nodes.begin(), group.nodes.end(), diffEdges.begin());
    diffEdges.resize(iter - diffEdges.begin());
	std::set<unsigned int> rightEdges(diffEdges.begin(), diffEdges.end());
	rightEdges.erase(leftSeed);

    int nbNodes = group.nodes.size();

    while (nbNodes > 2) {
        if (leftEdges.empty()) {
            rightGroup.insert(rightEdges.begin(), rightEdges.end());
            break;
        } else if (rightEdges.empty()) {
            leftGroup.insert(leftEdges.begin(), leftEdges.end());
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

            const std::vector<unsigned int> &edges = graph.getEdges(adj);
            std::vector<unsigned int> uni(edges.size());
            iter = std::set_intersection(group.nodes.begin(), group.nodes.end(), edges.begin(), edges.end(), uni.begin());
            rightEdges.insert(uni.begin(), iter);

            std::vector<unsigned int> firstDiff(rightEdges.size());
            std::vector<unsigned int> secondDiff(rightEdges.size());
            iter = std::set_difference(rightEdges.begin(), rightEdges.end(), rightGroup.begin(), rightGroup.end(), firstDiff.begin());
            firstDiff.resize(iter - firstDiff.begin());
            iter = std::set_difference(firstDiff.begin(), firstDiff.end(), leftGroup.begin(), leftGroup.end(), secondDiff.begin());
            rightEdges.clear();
            rightEdges.insert(secondDiff.begin(), iter);

            leftEdges.erase(adj);
        } else {
            auto it = leftEdges.begin();
            for (; edge != 0; --edge) it++;
            const unsigned int adj = *it;

            leftGroup.insert(adj);

            const std::vector<unsigned int> &edges = graph.getEdges(adj);
            std::vector<unsigned int> intersect(edges.size());
            iter = std::set_intersection(group.nodes.begin(), group.nodes.end(), edges.begin(), edges.end(), intersect.begin());
            leftEdges.insert(intersect.begin(), iter);

            std::vector<unsigned int> firstDiff(leftEdges.size());
            std::vector<unsigned int> secondDiff(leftEdges.size());
            iter = std::set_difference(leftEdges.begin(), leftEdges.end(), leftGroup.begin(), leftGroup.end(), firstDiff.begin());
            firstDiff.resize(iter - firstDiff.begin());
            iter = std::set_difference(firstDiff.begin(), firstDiff.end(), rightGroup.begin(), rightGroup.end(), secondDiff.begin());
            leftEdges.clear();
            leftEdges.insert(secondDiff.begin(), iter);

            rightEdges.erase(adj);
        }

        --nbNodes;
    }

    std::uniform_int_distribution<unsigned int> colorDistr(0, group.adjColors.size() - 1);
    unsigned int nbColor = colorDistr(rng);
    auto it = group.adjColors.begin();
    for (; nbColor != 0; --nbColor) it++;
    unsigned int color = *it;

    for (unsigned int leftNode: leftGroup)
        graph.color(leftNode, color);

    nbColor = colorDistr(rng);
    it = group.adjColors.begin();
    for (; nbColor != 0; --nbColor) it++;
    color = *it;

    for (unsigned int rightNode: rightGroup)
        graph.color(rightNode, color);
}

Graph generateNeighbour(const Graph &graph, Rng &rng) {
    const std::vector<Group> groups = generateGroups(graph);

    Graph newGraph = graph;

    std::uniform_int_distribution<unsigned int> groupDistr(0, groups.size() - 1);
    Group group = groups[groupDistr(rng)];

    std::uniform_real_distribution<double> splitDistr(0, 1);
    if (group.nodes.size() > 1 && splitDistr(rng) < 0.5)
        splitGroup(newGraph, group, rng);
    else {
        std::uniform_int_distribution<unsigned int> colorDistr(0, group.adjColors.size() - 1);
        unsigned int nbColor = colorDistr(rng);
        auto it = group.adjColors.begin();
        for (; nbColor != 0; --nbColor) it++;
        unsigned int color = *it;

        for (unsigned int node: group.nodes)
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

    std::ofstream f;
    f.open("../../../progress.txt");

    int maxIter = 5000;
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

        f << energy << std::endl;
    }
    return nodes - energy;
}

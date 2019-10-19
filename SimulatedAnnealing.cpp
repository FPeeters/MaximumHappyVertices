#include "SimulatedAnnealing.h"
#include <random>

typedef std::mt19937_64 Rng;

void initRandomColoring(Graph &graph, Rng &rng) {
    std::uniform_int_distribution<unsigned int> distribution(1, graph.getNbColors());
    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (!graph.isPreColored(node))
            graph.color(node, distribution(rng));
    }
}

Graph generateNeighbour(const Graph &graph, Rng &rng) {
    Graph newGraph = graph;

    std::uniform_int_distribution<unsigned int> nodeDistr(0, graph.getNbNodes() - 1);
    unsigned int node;
    do {
        node = nodeDistr(rng);
    } while (graph.isPreColored(node));

    std::uniform_int_distribution<unsigned int> colorDistr(1, graph.getNbColors());
    unsigned int color;
    do {
        color = colorDistr(rng);
    } while (graph.getColor(node) == color);

    newGraph.color(node, color);
    return newGraph;
}

std::vector<Group> generateGroups(const Graph &graph) {
    std::vector<Group> groups;
    int group[graph.getNbNodes()];
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
        for (unsigned int adj: graph.getEdges(node)) {
            if (graph.getColor(adj) != newGroup.color || group[adj] != 0)
                continue;
            group[adj] = groupCounter;
            newGroup.nodes.push_back(adj);
        }
        groups.push_back(newGroup);
    }
    return groups;
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

    generateGroups(graph);

    int maxIter = 10000;
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

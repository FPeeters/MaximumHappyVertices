#include <iostream>
#include "Graph.h"
#include "ConstructiveAlgs.h"
#include "SimulatedAnnealing.h"

int main() {
    Graph graph("graph.txt");

    unsigned int happy = simulatedAnnealing(graph, 123);

    std::cout << happy << std::endl;

    for (unsigned int i = 0; i < graph.getNbNodes(); ++i) {
        printf("n %u %u %s\n", i+1, graph.getColor(i), graph.isPreColored(i) ? "pre": "");
    }

    return EXIT_SUCCESS;
}
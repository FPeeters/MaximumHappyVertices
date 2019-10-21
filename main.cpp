#include <iostream>
#include "Graph.h"
#include "ConstructiveAlgs.h"
#include "SimulatedAnnealing.h"

int main() {
    Graph graph("graph.txt");

    unsigned int happy = simulatedAnnealing(graph, 1234);

    std::cout << happy << std::endl;

    graph.writeToDot("graph");
    return EXIT_SUCCESS;
}
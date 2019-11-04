#include <iostream>
#include "Graph.h"
#include "ConstructiveAlgs.h"
#include "SimulatedAnnealing.h"
#include "ExactSolver.h"

int main() {
    Graph graph("../../../graph2.txt");

    unsigned int happy = solveExact(graph);

    std::cout << happy << std::endl;

    graph.writeToDot("../../../graph2");
    return EXIT_SUCCESS;
}
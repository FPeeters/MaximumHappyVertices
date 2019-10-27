#include <iostream>
#include "Graph.h"
#include "ConstructiveAlgs.h"
#include "SimulatedAnnealing.h"
#include "ExactSolver.h"

int main() {
    Graph graph("../../../graph.txt");

    unsigned int happy = solveExact(graph);

    std::cout << happy << " " << graph.getHappyVertices() << std::endl;

    graph.writeToDot("graph");
    return EXIT_SUCCESS;
}
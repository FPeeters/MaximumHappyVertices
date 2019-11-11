#include <iostream>
#include "Graph.h"
#include "ConstructiveAlgs.h"
#include "SimulatedAnnealing.h"
#include "ExactSolver.h"
#include "config.h"

int main(int argc, char** argv) {
    config config(argc, argv);
    if (!config.loaded)
        return EXIT_FAILURE;

    Graph graph(config.inputFilename);

    unsigned int happy = 0;
    switch (config.algorithm) {
        case config::GREEDY:
            happy = greedyMHV(graph);
            break;
        case config::GROWTH:
            happy = growthMHV(graph);
            break;
        case config::SIMULATED_ANNEALING:
            happy = simulatedAnnealing(graph, config);
            break;
        case config::EXACT:
            happy = solveExact(graph);
            break;
    }

    if (config.outputPngFilename != nullptr)
        graph.writeToDot(config.outputPngFilename);

    std::cout << happy;
    return EXIT_SUCCESS;
}
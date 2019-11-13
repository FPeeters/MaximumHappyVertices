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
    
    try {
        Graph graph(config.inputFilename);
        
        unsigned int happy = 0;
        
        switch (config.algorithm) {
            case config::GREEDY:
                std::cout << "Executing greedy search" << std::endl;
                happy = greedyMHV(graph);
                break;
            case config::GROWTH:
                std::cout << "Executing growth search" << std::endl;
                happy = growthMHV(graph);
                break;
            case config::SIMULATED_ANNEALING:
                std::cout << "Executing simulated annealing" << std::endl;
                happy = simulatedAnnealing(graph, config);
                break;
            case config::EXACT:
                std::cout << "Executing exact solver" << std::endl;
                happy = solveExact(graph, config);
                break;
        }

        if (config.outputPngFilename != nullptr)
            graph.writeToDot(config.outputPngFilename);

        std::cout << happy;
    } catch (std::runtime_error &e) {
        std::cout << e.what();
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
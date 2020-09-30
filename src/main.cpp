#include <iostream>
#include <ctime>
#include "Graph.h"
#include "ConstructiveAlgs.h"
#include "SimulatedAnnealing.h"
#include "ExactSolver.h"
#include "reduce.h"


int main(int argc, char **argv) {
    config config(argc, argv);
    if (!config.loaded)
        return EXIT_FAILURE;

    try {
        float startTime = clock();

        Graph graph(config.inputFilename);
        std::cout << "Load took " << (clock() - startTime) / CLOCKS_PER_SEC << " sec" << std::endl;
        startTime = clock();

        reduced_graph reduced(graph, config);
        reduced.writeStats(std::cout);

        std::cout << "Reduce took " << (clock() - startTime) / CLOCKS_PER_SEC << " sec" << std::endl;
        startTime = clock();

        unsigned int happy = 0;

        if (reduced.reducedGraph.getNbNodes() == 0) {
            std::cout << "No nodes left after reducing" << std::endl;
            happy = 0;
        } else {
            switch (config.algo) {
                case config::GREEDY:
                    std::cout << "Executing greedy search" << std::endl;
                    happy = greedyMHV(reduced.reducedGraph);
                    break;
                case config::GROWTH:
                    std::cout << "Executing growth search" << std::endl;
                    happy = growthMHV(reduced.reducedGraph, config);
                    break;
                case config::TWO_REGULAR:
                    std::cout << "Executing 2-regular exact algorithm" << std::endl;
                    happy = twoRegular(reduced.reducedGraph);
                    break;
                case config::SIMULATED_ANNEALING:
                    std::cout << "Executing simulated annealing" << std::endl;
                    happy = simulatedAnnealing(reduced.reducedGraph, config);
                    break;
                case config::EXACT:
                    std::cout << "Executing exact solver" << std::endl;
                    happy = solveExact(reduced.reducedGraph, config);
                    break;
            }
        }

        std::cout << "Algorithm took " << (clock() - startTime) / CLOCKS_PER_SEC << " sec" << std::endl;

        happy += reduced.colorOriginal();

        if (config.outputPngFilename != nullptr)
            reduced.originalGraph.writeToDot(config.outputPngFilename);

        if (config.outputFilename != nullptr)
            reduced.originalGraph.writeToFile(config.outputFilename);

        std::cout << std::endl;
        if (config.minimize)
            std::cout << graph.getNbNodes() - happy;
        else
            std::cout << happy;

    } catch (std::runtime_error &e) {
        std::cout << std::endl << "The following error occured:" << std::endl << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
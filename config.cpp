#include <iostream>
#include "config.h"

using namespace std;

static void printUsage() {
    cout << "Solver for the Maximum Happy Vertices Problem" << endl
        << "USAGE:" << endl << endl
        << "INPUTFILE       Required, name if the input graph, in DIMACS format." << endl
        << "-a alg          Alogorithm choice, possible options:" << endl
        << "                greedy, growth, simAnn, exact. Default: exact" << endl
        << "-r INT          Seed for the random generator. Default: 123" << endl
        << "-opng FILENAME  Name for the output image of the graph." << endl
        << "                If not present, no image will be generated" << endl
        << endl
        << "Options for simulated annealing" << endl
        << "-i INT          Maximum amount of iterations. Default: 5000" << endl
        << "-t DOUBLE       Initial temperature. Default: 500" << endl
        << "-progress       Flag, if present, a file progress.txt will be generated." << endl;
}

config::config(int argc, char **argv) {
    if (argc < 2) {
        printUsage();
        return;
    }

    try {
        inputFilename = argv[1];

        for (int i = 2; i < argc; ++i) {
            if (strcmp("-a", argv[i]) == 0) {
                ++i;
                if (strcmp("greedy", argv[i]) == 0)
                    algorithm = GREEDY;
                else if (strcmp("growth", argv[i]) == 0)
                    algorithm = GROWTH;
                else if (strcmp("simAnn", argv[i]) == 0)
                    algorithm = SIMULATED_ANNEALING;
                else if (strcmp("exact", argv[i]) == 0)
                    algorithm = EXACT;
            } else if (strcmp("-r", argv[i]) == 0)
                seed = (int) strtol(argv[++i], nullptr, 10);
            else if (strcmp("-opng", argv[i]) == 0)
                outputPngFilename = argv[++i];
            else if (strcmp("-i", argv[i]) == 0)
                maxIterations = (int) strtol(argv[++i], nullptr, 10);
            else if (strcmp("-t", argv[i]) == 0)
                initTemp = strtod(argv[++i], nullptr);
            else if (strcmp("-progress", argv[i]) == 0)
                outputProgress = true;
        }

        loaded = true;
    } catch (...) {
        printUsage();
        return;
    }
}
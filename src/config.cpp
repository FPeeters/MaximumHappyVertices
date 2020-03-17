#include <iostream>
#include "config.h"

using namespace std;

static void printUsage() {
    cout << "Solver for the Maximum Happy Vertices Problem" << endl
         << "USAGE:" << endl << endl
         << "INPUTFILE       Required, name if the input graph, in DIMACS format." << endl
         << "-a ALG          Alogorithm choice, possible options:" << endl
         << "                greedy, growth, 2reg, simAnn, exact. Default: exact" << endl
         << "-r INT          Seed for the random generator. Default: 123" << endl
         << "-time INT       Time limit in seconds for the picked algorithm. Default: -1" << endl
         << "-opng FILENAME  Name for the output image of the graph." << endl
         << "                If not present, no image will be generated." << endl
         << "-out FILENAME   Name for the file used to print the solution to." << endl
         << "                If not present, the solution will not be outputted." << endl
         << "-minimize       The count of unhappy vertices will be printed" << endl
         << "                instead of the standard count of happy vertices." << endl
         << endl
         << "Options for the exact solver" << endl
         << "-threads INT    The amount of threads used by the exact solver." << endl
         << "                Default: The maximum amount of threads available." << endl
         << endl
         << "Options for simulated annealing" << endl
         << "-init ALG       Initial solution algotihm: possible options:" << endl
         << "                random, greedy, growth, best. Default: random" << endl
         << "-maxI INT       Maximum amount of iterations. Default: -1" << endl
         << "                Either this or the time limit have to be set." << endl
         << "-temp DOUBLE    Initial temperature. Default: 500" << endl
         << "-progress       Flag, if present, a file progress.txt will be generated." << endl
         << "-swap DOUBLE    Chance to use the swap operator in neighbour generation." << endl
         << "                Default: 0.33" << endl
         << "-split DOUBLE   Chance to use the split operator in neighbour generation." << endl
         << "                Default: 0.33" << endl;
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
                else if (strcmp("2reg", argv[i]) == 0)
                    algorithm = TWO_REGULAR;
                else if (strcmp("simAnn", argv[i]) == 0)
                    algorithm = SIMULATED_ANNEALING;
                else if (strcmp("exact", argv[i]) == 0)
                    algorithm = EXACT;
            } else if (strcmp("-r", argv[i]) == 0)
                seed = (int) strtol(argv[++i], nullptr, 10);
            else if (strcmp("-time", argv[i]) == 0)
                timeLimit = (int) strtol(argv[++i], nullptr, 10);
            else if (strcmp("-opng", argv[i]) == 0)
                outputPngFilename = argv[++i];
            else if (strcmp("-out", argv[i]) == 0)
                outputFilename = argv[++i];
            else if (strcmp("-minimize", argv[i]) == 0)
                minimize = true;
            else if (strcmp("-threads", argv[i]) == 0)
                threads = (int) strtol(argv[++i], nullptr, 10);
            else if (strcmp("-init", argv[i]) == 0) {
                ++i;
                if (strcmp("random", argv[i]) == 0)
                    initAlgorithm = random;
                else if (strcmp("greedy", argv[i]) == 0)
                    initAlgorithm = greedy;
                else if (strcmp("growth", argv[i]) == 0)
                    initAlgorithm = growth;
                else if (strcmp("best", argv[i]) == 0)
                    initAlgorithm = best;
            } else if (strcmp("-maxI", argv[i]) == 0)
                maxIterations = (int) strtol(argv[++i], nullptr, 10);
            else if (strcmp("-temp", argv[i]) == 0)
                initTemp = strtod(argv[++i], nullptr);
            else if (strcmp("-progress", argv[i]) == 0)
                outputProgress = true;
            else if (strcmp("-swap", argv[i]) == 0)
                swapDegreePerc = strtod(argv[++i], nullptr);
            else if (strcmp("-split", argv[i]) == 0)
                splitGroupPerc = strtod(argv[++i], nullptr);
            else {
                cout << "No such option: " << argv[i] << endl << endl;
                printUsage();
                return;
            }
        }

        if (swapDegreePerc + splitGroupPerc > 1) {
            cout << "The sum of both percentages must be < 1." << endl;
            return;
        }

        if (algorithm == SIMULATED_ANNEALING) {
            if (!(timeLimit != -1 && maxIterations == -1) && !(timeLimit == -1 && maxIterations != -1)) {
                cout << "For simulated annealing either a time limit or a maximum amout of iterations has to be set" << endl;
                return;
            }
        }
        loaded = true;
    } catch (...) {
        printUsage();
        return;
    }
}

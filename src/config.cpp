#include <iostream>
#include <cstring>
#include "config.h"

using namespace std;

static void printUsage() {
    cout << "Solver for the Maximum Happy Vertices Problem" << endl
         << "USAGE:" << endl << endl
         << "INPUTFILE        Required, name if the input graph, in DIMACS format." << endl
         << "-a ALG           Alogorithm choice, possible options:" << endl
         << "                 greedy, growth, 2reg, simAnn, exact. Default: exact" << endl
         << "-r INT           Seed for the random generator. Default: 123" << endl
         << "-time INT        Time limit in seconds for the picked algorithm. Default: -1" << endl
         << "-opng FILENAME   Name for the output image of the graph." << endl
         << "                 If not present, no image will be generated." << endl
         << "-out FILENAME    Name for the file used to print the solution to." << endl
         << "                 If not present, the solution will not be outputted." << endl
         << "-minimize        The count of unhappy vertices will be printed" << endl
         << "                 instead of the standard count of happy vertices." << endl
         << "-red RED         Reduction method to use, possible options:" << endl
         << "                 none, thiruvady, basic, articul. Default: articul" << endl
         << endl
         << "Options for the exact solver" << endl
         << "-threads INT     The amount of threads used by the exact solver." << endl
         << "                 Default: The maximum amount of threads available." << endl
         << "-altModel        If present, the alternative IP model will be used" << endl
         << endl
         << "Options for the growth algorithm" << endl
         << "-selectRandom    If present, the growth algorithm will select nodes randomly" << endl
         << "-alpha DOUBLE    The bias parameter for the linear distribution in the" << endl
         << "                 random selector. Range: [-2, 2]. Default: 0" << endl
         << endl
         << "Options for simulated annealing" << endl
         << "-init ALG        Initial solution algorithm: possible options:" << endl
         << "                 random, greedy, growth, best. Default: random" << endl
         << "-maxI INT        Maximum amount of iterations. Default: -1" << endl
         << "                 Either this or the time limit have to be set." << endl
         << "-temp DOUBLE     Initial temperature. Default: 500" << endl
         << "-zeroTemp DOUBLE Part of iterations that will have temperature 0. Default: 0.05" << endl
         << "-progress        Flag, if present, a file progress.txt will be generated." << endl
         << "-swap DOUBLE     Chance to use the swap operator in neighbour generation." << endl
         << "                 Default: 0.33" << endl
         << "-split DOUBLE    Chance to use the split operator in neighbour generation." << endl
         << "                 Default: 0.33" << endl;
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
                    algo = GREEDY;
                else if (strcmp("growth", argv[i]) == 0)
                    algo = GROWTH;
                else if (strcmp("2reg", argv[i]) == 0)
                    algo = TWO_REGULAR;
                else if (strcmp("simAnn", argv[i]) == 0)
                    algo = SIMULATED_ANNEALING;
                else if (strcmp("exact", argv[i]) == 0)
                    algo = EXACT;
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
            else if (strcmp("-red", argv[i]) == 0) {
                ++i;
                if (strcmp("none", argv[i]) == 0)
                    reduct = NONE;
                else if (strcmp("thiruvady", argv[i]) == 0)
                    reduct = THIRUVADY;
                else if (strcmp("basic", argv[i]) == 0)
                    reduct = BASIC;
                else if (strcmp("articul", argv[i]) == 0)
                    reduct = ARTICULATION;
            } else if (strcmp("-threads", argv[i]) == 0)
                threads = (int) strtol(argv[++i], nullptr, 10);
            else if (strcmp("-altModel", argv[i]) == 0)
                altModel = true;
            else if (strcmp("-selectRandom", argv[i]) == 0)
                randomSelection = true;
            else if (strcmp("-alpha", argv[i]) == 0)
                alpha = strtod(argv[++i], nullptr);
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
            else if (strcmp("-zeroTemp", argv[i]) == 0)
                zeroIterations = strtod(argv[++i], nullptr);
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

        if (algo == SIMULATED_ANNEALING) {
            if (!(timeLimit != -1 && maxIterations == -1) && !(timeLimit == -1 && maxIterations != -1)) {
                cout << "For simulated annealing either a time limit or a maximum amout of iterations has to be set" << endl;
                return;
            }
        }

        if (alpha < -2 || alpha > 2) {
            cout << "The allowed range for alpha is [-2, 2]" << endl;
            return;
        }

        if (zeroIterations < 0 || zeroIterations > 1) {
            cout << "The allowed range for zeroTemp is [0, 1]" << endl;
            return;
        }

        loaded = true;
    } catch (...) {
        printUsage();
        return;
    }
}

#ifndef MAXHAPPYVERTS_CONFIG_H
#define MAXHAPPYVERTS_CONFIG_H

class config {
public:
    bool loaded = false;
    char *inputFilename = nullptr;

    enum algorithm {
        GREEDY,
        GROWTH,
        TWO_REGULAR,
        SIMULATED_ANNEALING,
        EXACT
    };
    algorithm algo = EXACT;
    int seed = 123;
    bool minimize = false;

    enum reduction {
        NONE,
        THIRUVADY,
        BASIC,
        ARTICULATION
    };
    reduction reduct = ARTICULATION;

    // Exact solver options
    int timeLimit = -1;
    int threads = 0;
    bool altModel = false;

    // Growth options
    bool randomSelection = false;
    double alpha = 0;

    // Simulated annealing options
    enum initAlgorithm {
        random,
        greedy,
        growth,
        best
    };
    initAlgorithm initAlgorithm = random;
    int maxIterations = -1;
    double initTemp = 500;
    double zeroIterations = 0.05;
    bool outputProgress = false;
    double swapDegreePerc = 0.33;
    double splitGroupPerc = 0.33;

    char *outputPngFilename = nullptr;
    char *outputFilename = nullptr;

    explicit config(int argc, char **argv);
};


#endif //MAXHAPPYVERTS_CONFIG_H

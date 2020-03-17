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
    algorithm algorithm = EXACT;
    int seed = 123;
    bool minimize = false;

    // Exact solver options
    int timeLimit = -1;
    int threads = 0;

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
    bool outputProgress = false;
    double swapDegreePerc = 0.33;
    double splitGroupPerc = 0.33;

    char *outputPngFilename = nullptr;
    char *outputFilename = nullptr;

    explicit config(int argc, char **argv);
};


#endif //MAXHAPPYVERTS_CONFIG_H

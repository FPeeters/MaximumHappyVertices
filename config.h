#ifndef MAXHAPPYVERTS_CONFIG_H
#define MAXHAPPYVERTS_CONFIG_H

class config {
public:
    bool loaded = false;
    char* inputFilename = nullptr;

    enum algorithm {
        GREEDY,
        GROWTH,
        SIMULATED_ANNEALING,
        EXACT
    };
    algorithm algorithm = EXACT;
    int seed = 123;

    // Exact solver options
    int timeLimit = -1;
    int threads = 0;

    // Simulated annealing options
    int maxIterations = 5000;
    double initTemp = 500;
    bool outputProgress = false;

    char* outputPngFilename = nullptr;
    char* outputFilename = nullptr;


    explicit config(int argc, char** argv);
};


#endif //MAXHAPPYVERTS_CONFIG_H

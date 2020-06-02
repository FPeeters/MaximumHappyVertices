#ifndef MAXHAPPYVERTS_REDUCE_H
#define MAXHAPPYVERTS_REDUCE_H

#include <ostream>
#include "Graph.h"
#include "config.h"

class reduced_graph {
private:
    struct reduce_stats {
        unsigned int unconnectedComponent = 0;
        unsigned int singleColorComponent = 0;
        unsigned int unhappyConnections = 0;
        unsigned int singleLinkChains = 0;

        unsigned int freeArticulation = 0;
        unsigned int singleArticulation = 0;
        unsigned int nbIterations = 0;
    };

    std::vector<std::vector<unsigned int>> firstReferences;
    std::vector<unsigned int> secondReferences;
    reduce_stats stats;
    config::reduction mode;

    void thiruvadyReduction();

    void basicReduction();

    void articulationReduction();


public:
    Graph &originalGraph;
    Graph reducedGraph;

    explicit reduced_graph(Graph &original, const config &config);

    unsigned int colorOriginal();

    void writeStats(std::ostream &out);
};


#endif //MAXHAPPYVERTS_REDUCE_H

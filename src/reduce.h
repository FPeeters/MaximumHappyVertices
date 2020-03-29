#ifndef MAXHAPPYVERTS_REDUCE_H
#define MAXHAPPYVERTS_REDUCE_H

#include <ostream>
#include "Graph.h"
#include "config.h"

struct ReduceStats {
    unsigned int unconnectedComponent = 0;
    unsigned int singleColorComponent = 0;
    unsigned int unhappyConnections = 0;
    unsigned int singleLinkChains = 0;

    unsigned int freeArticulation = 0;
    unsigned int singleArticulation = 0;
};

class ReducedGraph {
private:
    std::vector<unsigned int> firstReferences;
    std::vector<unsigned int> secondReferences;
    ReduceStats stats;
    config::reduction mode;

    void thiruvadyReduction();

    void basicReduction();

    void articulationReduction();


public:
    Graph &originalGraph;
    Graph reducedGraph;

    explicit ReducedGraph(Graph &original, const config &config);

    unsigned int colorOriginal();

    void writeStats(std::ostream &out);
};


#endif //MAXHAPPYVERTS_REDUCE_H

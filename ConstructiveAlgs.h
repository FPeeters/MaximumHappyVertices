#ifndef MAXHAPPYVERTS_CONSTRUCTIVEALGS_H
#define MAXHAPPYVERTS_CONSTRUCTIVEALGS_H

#include "Graph.h"
#include "config.h"

enum node_status {
    None,
    Happy,
    PotentiallyHappy,
    Unhappy,
    Lp,
    Lh,
    Lu,
    Lf
};

unsigned int greedyMHV(Graph &graph);

unsigned int growthMHV(Graph &graph, const config &config);

#endif //MAXHAPPYVERTS_CONSTRUCTIVEALGS_H

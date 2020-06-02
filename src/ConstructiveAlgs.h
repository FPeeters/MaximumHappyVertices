#ifndef MAXHAPPYVERTS_CONSTRUCTIVEALGS_H
#define MAXHAPPYVERTS_CONSTRUCTIVEALGS_H

#include "Graph.h"
#include "config.h"

unsigned int greedyMHV(Graph &graph);

unsigned int growthMHV(Graph &graph, const config &config);

unsigned int twoRegular(Graph &graph);

#endif //MAXHAPPYVERTS_CONSTRUCTIVEALGS_H

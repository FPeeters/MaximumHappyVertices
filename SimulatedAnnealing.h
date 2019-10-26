#ifndef MAXHAPPYVERTS_SIMULATEDANNEALING_H
#define MAXHAPPYVERTS_SIMULATEDANNEALING_H

#include <set>
#include "Graph.h"

struct Group {
    unsigned int color;
    std::vector<unsigned int> nodes;
    std::set<unsigned int, std::greater<>> adjColors;

    explicit Group(unsigned int color, unsigned int node) : color(color), nodes(), adjColors() {
        nodes.push_back(node);
    }
};

unsigned int simulatedAnnealing(Graph &graph, int seed);

#endif //MAXHAPPYVERTS_SIMULATEDANNEALING_H

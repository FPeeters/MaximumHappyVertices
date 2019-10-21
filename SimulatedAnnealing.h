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

struct RemovedGroup {
    std::vector<unsigned int> nodes;
    unsigned int connection;

    explicit RemovedGroup(unsigned int node) : nodes(), connection() {
        nodes.push_back(node);
    }
};

enum Label {
    PRECOLORED,
    NONFIXED,
    REMOVED,
    ENCLOSED
};

class ProblemGraph {
public:
    Graph &graph;
    std::vector<unsigned int> nonFixedNodes;
    std::vector<RemovedGroup> removedGroups;
    std::vector<unsigned int> enclosedNodes;
    Label* labels;

    explicit ProblemGraph(Graph &graph);

    unsigned int getHappyVertices() const;

    unsigned int colorRemoved();
};

unsigned int simulatedAnnealing(Graph &graph, int seed);

#endif //MAXHAPPYVERTS_SIMULATEDANNEALING_H

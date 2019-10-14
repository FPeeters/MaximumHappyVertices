#include "ConstructiveAlgs.h"

void colorAll(Graph &graph, unsigned int color) {
    for(unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (!graph.isPreColored(node))
            graph.color(node, color);
    }
}

unsigned int greedyMHV(Graph &graph) {
    unsigned int bestHappy = 0;
    unsigned int bestColor = 0;
    for (unsigned int color = 1; color <= graph.getNbColors(); ++color) {
        colorAll(graph, color);
        unsigned int happy = graph.getHappyVertices();
        if (happy > bestHappy) {
            bestHappy = happy;
            bestColor = color;
        }
    }
    colorAll(graph, bestColor);
    return bestHappy;
}

unsigned int growthMHV(Graph &graph) {
    return 0;
}

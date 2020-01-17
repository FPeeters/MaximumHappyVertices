#include <random>
#include "ConstructiveAlgs.h"

typedef std::mt19937_64 Rng;

void colorAll(Graph &graph, unsigned int color) {
    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
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

node_status determineColoredStatus(const Graph &graph, const unsigned int node,
                                   const std::vector<node_status> &labels, unsigned int &nbHappy) {
    if (labels[node] == Happy || labels[node] == Unhappy)
        return labels[node];

    bool freeAdj = false;
    for (unsigned int adj: graph.getEdges(node)) {
        if (graph.getColor(adj) == 0)
            freeAdj = true;
        else if (graph.getColor(node) != graph.getColor(adj))
            return Unhappy;
    }

    if (!freeAdj) {
        ++nbHappy;
        return Happy;
    } else {
        return PotentiallyHappy;
    }
}

node_status determineFreeStatus(const Graph &graph, const unsigned int node, const std::vector<node_status> &labels) {
    bool multipleAdjColors = false;
    unsigned int adjColor = 0;

    for (unsigned int adj: graph.getEdges(node)) {
        if (labels[adj] == PotentiallyHappy)
            return Lp;

        if (graph.getColor(adj) != 0 && adjColor == 0)
            adjColor = graph.getColor(adj);
        else if (graph.getColor(adj) != 0 && adjColor != graph.getColor(adj))
            multipleAdjColors = true;
    }

    if (adjColor != 0 && !multipleAdjColors)
        return Lh;
    else if (multipleAdjColors)
        return Lu;
    else
        return Lf;
}

unsigned int getNext(const Graph &graph, const std::vector<node_status> &labels,
                     bool &pVertex, bool &lhVertex, bool &luVertex) {
    pVertex = lhVertex = luVertex = false;
    unsigned int luVertexIdx = -1, lfVertexIdx = -1;

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (labels[node] == PotentiallyHappy) {
            pVertex = true;
            return node;
        }
    }

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        switch (labels[node]) {
            case Lh:
                lhVertex = true;
                return node;
            case Lu:
                luVertex = true;
                luVertexIdx = node;
                break;
            case Lf:
                lfVertexIdx = node;
                break;
            default:
                break;
        }
    }

    if (luVertex)
        return luVertexIdx;
    else
        return lfVertexIdx;
}

void updateDistanceTwo(const Graph &graph, const unsigned int node, std::vector<node_status> &labels,
                       unsigned int &nbHappy) {
    labels[node] = Happy;
    ++nbHappy;

    for (unsigned int adj: graph.getEdges(node))
        labels[adj] = determineColoredStatus(graph, adj, labels, nbHappy);

    for (unsigned int adj: graph.getEdges(node)) {
        for (unsigned int adjAdj: graph.getEdges(adj)) {
            if (graph.getColor(adjAdj) != 0) {
                node_status oldLbl = labels[adjAdj];
                labels[adjAdj] = determineColoredStatus(graph, adjAdj, labels, nbHappy);

                if (oldLbl != labels[adjAdj]) {
                    for (unsigned int adjAdjAdj: graph.getEdges(adjAdj)) {
                        if (graph.getColor(adjAdjAdj) == 0)
                            labels[adjAdjAdj] = determineFreeStatus(graph, adjAdjAdj, labels);
                    }
                }
            } else
                labels[adjAdj] = determineFreeStatus(graph, adjAdj, labels);
        }
    }
}

unsigned int getAdjColor(const Graph &graph, const unsigned int node) {
    for (unsigned int adj: graph.getEdges(node))
        if (graph.getColor(adj) != 0)
            return graph.getColor(adj);

    return -1;
}

unsigned int getUnhappyAdj(const Graph &graph, const unsigned int node, const std::vector<node_status> &labels) {
    for (unsigned int adj: graph.getEdges(node))
        if (labels[adj] == Unhappy)
            return graph.getColor(adj);

    return -1;
}

void updateAdj(Graph &graph, unsigned int node, std::vector<node_status> &labels, unsigned int &nbHappy) {
    for (unsigned int adj: graph.getEdges(node)) {
        if (graph.getColor(adj) != 0)
            labels[adj] = determineColoredStatus(graph, adj, labels, nbHappy);
        else
            labels[adj] = determineFreeStatus(graph, adj, labels);
    }
}


unsigned int growthMHV(Graph &graph, const config &config) {
    Rng rng(config.seed);

    std::vector<node_status> labels(graph.getNbNodes(), None);

    unsigned int nbHappy = 0, numColored = 0;

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (graph.isPreColored(node)) {
            labels[node] = determineColoredStatus(graph, node, labels, nbHappy);
            ++numColored;
        }

    }

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (!graph.isPreColored(node))
            labels[node] = determineFreeStatus(graph, node, labels);
    }

    bool pVertex, lhVertex, luVertex;
    while (numColored < graph.getNbNodes()) {
        unsigned int next = getNext(graph, labels, pVertex, lhVertex, luVertex);
        unsigned int col;

        if (pVertex) {
            col = graph.getColor(next);
            for (unsigned int adj: graph.getEdges(next)) {
                if (labels[adj] == Lp && graph.getColor(adj) == 0) {
                    graph.color(adj, col);
                    ++numColored;
                }
            }
            updateDistanceTwo(graph, next, labels, nbHappy);
        } else if (lhVertex) {
            col = getAdjColor(graph, next);
            graph.color(next, col);
            ++numColored;
            for (unsigned int adj: graph.getEdges(next)) {
                if (graph.getColor(adj) == 0) {
                    graph.color(adj, col);
                    ++numColored;
                }
            }
            updateDistanceTwo(graph, next, labels, nbHappy);
        } else if (luVertex) {
            col = getUnhappyAdj(graph, next, labels);
            graph.color(next, col);
            ++numColored;
            labels[next] = Unhappy;
            updateAdj(graph, next, labels, nbHappy);
        } else {
            std::uniform_int_distribution<unsigned int> colorDistr(1, graph.getNbColors());
            col = colorDistr(rng);
            graph.color(next, col);
            ++numColored;
            if (graph.getEdges(next).empty()) {
                labels[next] = Happy;
                ++nbHappy;
            } else {
                labels[next] = Unhappy;
                updateAdj(graph, next, labels, nbHappy);
            }
        }
    }

    return nbHappy;
}

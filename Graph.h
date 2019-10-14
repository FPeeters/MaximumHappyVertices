#ifndef MAXHAPPYVERTS_GRAPH_H
#define MAXHAPPYVERTS_GRAPH_H

#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>

class Graph {

    typedef std::vector<unsigned int> edgesType;

public:
    struct Node {
        unsigned int color;
        bool preColored;
        edgesType edges;

        explicit Node(unsigned int color) : color(color), preColored(color != 0) {};
    };

private:
    unsigned int nbNodes;
    unsigned int nbColors;
    std::vector<Node> nodes;

public:
    explicit Graph(unsigned int nbColors) : nbNodes(0), nbColors(nbColors) {};

    explicit Graph(const std::string &fileName);

    inline Node getNode(unsigned int i) const {
        return nodes[i];
    }

    inline unsigned int getColor(unsigned int i) const {
        return nodes[i].color;
    }

    inline edgesType getEdges(unsigned int i) const {
        return nodes[i].edges;
    }

    inline bool isPreColored(unsigned int i) const {
        return nodes[i].preColored;
    }

    inline unsigned int getNbColors() const {
        return nbColors;
    }

    inline unsigned int getNbNodes() const {
        return nbNodes;
    }


    inline bool hasEdge(unsigned int first, unsigned int second) const {
        const edgesType &edges = nodes[first].edges;
        return std::find(edges.begin(), edges.end(), second) != edges.end();
    }


    inline void color(unsigned int i, unsigned int color) {
        Node &node = nodes[i];
        if (node.preColored)
            throw std::runtime_error("Cannot change color of precolored nodes");
        if (color > nbColors)
            throw std::runtime_error("Color out of bounds");
        node.color = color;
    }

    inline void addNode(unsigned int color = 0) {
        if (color > nbColors)
            throw std::runtime_error("Color out of bounds");
        Node node(color);
        nodes.push_back(node);
        nbNodes++;
    }

    inline void addEdge(unsigned int from, unsigned int to) {
        if (from >= nbNodes || to >= nbNodes)
            throw std::runtime_error("Node index for edge out of bounds");
        nodes[from].edges.push_back(to);
        nodes[to].edges.push_back(from);
    }

    unsigned int getHappyVertices() const;

};

#endif //MAXHAPPYVERTS_GRAPH_H

#ifndef MAXHAPPYVERTS_GRAPH_H
#define MAXHAPPYVERTS_GRAPH_H

#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>

#if __has_cpp_attribute(nodiscard)
#define NODISCARD [[nodiscard]]
#elif __has_cpp_attribute(gnu::warn_unused_result)
#define NODISCARD [[gnu::warn_unused_result]]
#else
#define NODISCARD
#endif

class Graph {

private:
    struct node {
        unsigned int color;
        bool preColored;
        std::vector<unsigned int> edges;

        explicit node(unsigned int color) : color(color), preColored(color != 0) {};
    };

    unsigned int nbNodes;
    unsigned int nbColors;
    std::vector<node> nodes;

public:
    explicit Graph(unsigned int nbColors) : nbNodes(0), nbColors(nbColors) {};

    explicit Graph(const std::string &fileName);

    NODISCARD inline unsigned int getColor(unsigned int i) const {
        return nodes[i].color;
    }

    NODISCARD inline std::vector<unsigned int> getEdges(unsigned int i) const {
        return nodes[i].edges;
    }

    NODISCARD inline bool isPreColored(unsigned int i) const {
        return nodes[i].preColored;
    }

    NODISCARD inline unsigned int getNbColors() const {
        return nbColors;
    }

    NODISCARD inline unsigned int getNbNodes() const {
        return nbNodes;
    }


    NODISCARD inline bool hasEdge(unsigned int first, unsigned int second) const {
        const std::vector<unsigned int> &edges = nodes[first].edges;
        return std::find(edges.begin(), edges.end(), second) != edges.end();
    }


    inline void color(unsigned int i, unsigned int color) {
        if (nodes[i].preColored)
            throw std::runtime_error("Cannot change color of precolored nodes");
        if (color > nbColors)
            throw std::runtime_error("Color out of bounds");
        nodes[i].color = color;
    }

    inline void addNode(unsigned int color = 0) {
        if (color > nbColors)
            throw std::runtime_error("Color out of bounds");
        nodes.emplace_back(color);
        nbNodes++;
    }

    inline void addEdge(unsigned int from, unsigned int to) {
        if (from >= nbNodes || to >= nbNodes)
            throw std::runtime_error("Node index for edge out of bounds");
        nodes[from].edges.push_back(to);
        nodes[to].edges.push_back(from);
    }

    inline void sortEdges() {
        for (auto &node : nodes)
            std::sort(node.edges.begin(), node.edges.end());
    }

    NODISCARD bool isHappy(unsigned int node) const;

    NODISCARD unsigned int getHappyVertices() const;

    void writeToDot(const std::string &filename) const;

    void writeToFile(const std::string &filename) const;
};

#endif //MAXHAPPYVERTS_GRAPH_H

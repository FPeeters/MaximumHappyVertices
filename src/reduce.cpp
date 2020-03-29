#include <queue>
#include <ostream>
#include <iostream>
#include <set>
#include "reduce.h"
#include "Graph.h"

std::vector<unsigned int> findChain(Graph &graph, unsigned int node, unsigned int &anchor) {
    std::vector<unsigned int> chain;
    chain.push_back(node);

    unsigned int previous = node;
    unsigned int next = graph.getEdges(node)[0];
    anchor = next;

    while (!graph.isPreColored(next) && graph.getEdges(next).size() <= 2) {
        chain.push_back(next);
        if (graph.getEdges(next).size() == 1) {
            anchor = -1;
            break;
        } else {
            unsigned int temp = next;
            if (graph.getEdges(next)[0] == previous)
                next = graph.getEdges(next)[1];
            else
                next = graph.getEdges(next)[0];
            previous = temp;
            anchor = next;
        }
    }
    return chain;
}

bool isUnhappy(Graph &graph, unsigned int node) {
    unsigned int ownColor = graph.getColor(node);
    for (unsigned int adj: graph.getEdges(node)) {
        if (graph.isPreColored(adj) && graph.getColor(adj) != ownColor)
            return true;
    }
    return false;
}

bool onlyConnectedToUnhappyDifferentColor(Graph &graph, unsigned int node) {
    unsigned int adjColor = -1;
    bool differentColors = false;
    for (unsigned int adj: graph.getEdges(node)) {
        if (graph.isPreColored(adj) && isUnhappy(graph, adj)) {
            if (adjColor == -1)
                adjColor = graph.getColor(adj);
            else if (adjColor != graph.getColor(adj))
                differentColors = true;
        } else
            return false;
    }
    return differentColors;
}

bool isInFreeComponent(Graph &graph, unsigned int node, std::vector<unsigned int> &component, unsigned int &reference) {
    unsigned int adjColor = -1;
    std::queue<unsigned int> queue;
    queue.push(node);
    std::vector<unsigned int> done;
    done.push_back(node);

    while (!queue.empty()) {
        unsigned int next = queue.front();
        queue.pop();
        component.push_back(next);
        for (unsigned int adj: graph.getEdges(next)) {
            if (graph.isPreColored(adj)) {
                if (adjColor == -1) {
                    adjColor = graph.getColor(adj);
                    reference = adj;
                } else if (adjColor != graph.getColor(adj))
                    return false;
            } else if (std::find(done.begin(), done.end(), adj) == done.end()) {
                done.push_back(adj);
                queue.push(adj);
            }
        }
    }

    return true;
}

// Based on pseudocode of Tarjan & Hopcroft's algorithm at https://en.wikipedia.org/wiki/Biconnected_component
void
depthFirstSearch(const Graph &graph, unsigned int node, unsigned int currentDepth, bool *visited, unsigned int *depth,
                 unsigned int *low, unsigned int *parent, bool *articulation) {
    visited[node] = true;
    depth[node] = currentDepth;
    low[node] = currentDepth;
    unsigned int childCount = 0;

    for (unsigned int adj: graph.getEdges(node)) {
        if (!visited[adj]) {
            parent[adj] = node;
            ++childCount;
            depthFirstSearch(graph, adj, currentDepth + 1, visited, depth, low, parent, articulation);
            if (low[adj] >= depth[node])
                articulation[node] = true;
            low[node] = __min(low[node], low[adj]);
        } else if (adj != parent[node])
            low[node] = __min(low[node], depth[adj]);
    }

    if (parent[node] == -1)
        articulation[node] = childCount > 1;
    if (graph.getEdges(node).size() == 2)
        articulation[node] = false;
}

std::vector<unsigned int> buildReducedGraph(Graph &original, Graph &reduced, unsigned int *replacements) {
    std::vector<unsigned int> references(original.getNbNodes());

    unsigned int nodeCounter = 0;
    for (unsigned int node = 0; node < original.getNbNodes(); ++node) {
        if (replacements[node] == -2) {
            if (original.isPreColored(node))
                reduced.addNode(original.getColor(node));
            else
                reduced.addNode();
            references[node] = nodeCounter++;
        }
    }

    for (unsigned int node = 0; node < original.getNbNodes(); ++node) {
        if (replacements[node] == -1)
            references[node] = -1;
        else if (replacements[node] != -2)
            references[node] = references[replacements[node]];
        else {
            for (unsigned int adj: original.getEdges(node)) {
                if (replacements[adj] == -2 && references[adj] < references[node])
                    reduced.addEdge(references[node], references[adj]);
            }
        }
    }

    reduced.sortEdges();

    return references;
}


void ReducedGraph::thiruvadyReduction() {
    auto *replacements = (unsigned int *) malloc(sizeof(unsigned int) * originalGraph.getNbNodes());
    std::fill_n(replacements, originalGraph.getNbNodes(), -2);

    for (unsigned int node = 0; node < originalGraph.getNbNodes(); ++node) {
        if (replacements[node] != -2 || originalGraph.isPreColored(node))
            continue;

        std::vector<unsigned int> freeComponent;
        unsigned int adjReference = -1;
        if (isInFreeComponent(originalGraph, node, freeComponent, adjReference)) {
            for (unsigned int item: freeComponent) {
                if (adjReference == -1)
                    ++stats.unconnectedComponent;
                else
                    ++stats.singleColorComponent;
                replacements[item] = adjReference;
            }
            continue;
        }

        if (onlyConnectedToUnhappyDifferentColor(originalGraph, node)) {
            ++stats.unhappyConnections;
            replacements[node] = -1;
            continue;
        }
    }

    firstReferences = buildReducedGraph(originalGraph, reducedGraph, replacements);

    free(replacements);
}

void ReducedGraph::basicReduction() {
    auto *replacements = (unsigned int *) malloc(sizeof(unsigned int) * originalGraph.getNbNodes());
    std::fill_n(replacements, originalGraph.getNbNodes(), -2);

    for (unsigned int node = 0; node < originalGraph.getNbNodes(); ++node) {
        if (replacements[node] != -2)
            continue;

        const std::vector<unsigned int> &edges = originalGraph.getEdges(node);
        if (edges.empty()) {
            ++stats.unconnectedComponent;
            replacements[node] = -1;
            continue;
        }

        if (originalGraph.isPreColored(node))
            continue;

        if (edges.size() == 1) {
            unsigned int anchor;
            std::vector<unsigned int> chain = findChain(originalGraph, node, anchor);

            for (unsigned int item: chain) {
                ++stats.singleLinkChains;
                replacements[item] = anchor;
            }
            continue;
        }

        if (onlyConnectedToUnhappyDifferentColor(originalGraph, node)) {
            ++stats.unhappyConnections;
            replacements[node] = -1;
            continue;
        }

        std::vector<unsigned int> freeComponent;
        unsigned int adjReference = -1;
        if (isInFreeComponent(originalGraph, node, freeComponent, adjReference)) {
            for (unsigned int item: freeComponent) {
                if (adjReference == -1)
                    ++stats.unconnectedComponent;
                else
                    ++stats.singleColorComponent;
                replacements[item] = adjReference;
            }
            continue;
        }
    }

    firstReferences = buildReducedGraph(originalGraph, reducedGraph, replacements);
    free(replacements);
}

struct component {
    std::vector<unsigned int> nodes;
    unsigned int reference = -1;
    unsigned int nbPrecolors = 0;
    unsigned int nbAdj = 0;
};

component findComponent(const Graph &graph, unsigned int node, bool *visited, const bool *articulation) {
    component comp{};
    std::queue<unsigned int> queue;
    queue.push(node);
    visited[node] = true;

    std::set<unsigned int> adjColors;

    while (!queue.empty()) {
        unsigned int next = queue.front();
        queue.pop();
        comp.nodes.push_back(next);
        for (unsigned int adj: graph.getEdges(next)) {
            if (graph.isPreColored(adj)) {
                adjColors.insert(graph.getColor(adj));
                comp.reference = adj;
            } else if (articulation[adj]) {
                if (comp.reference != adj) {
                    comp.reference = adj;
                    ++comp.nbAdj;
                }
            } else if (!visited[adj]) {
                queue.push(adj);
                visited[adj] = true;
            }
        }
    }
    comp.nbPrecolors = adjColors.size();

    return comp;
}

void ReducedGraph::articulationReduction() {
    auto *replacements = (unsigned int *) malloc(sizeof(unsigned int) * originalGraph.getNbNodes());
    std::fill_n(replacements, originalGraph.getNbNodes(), -2);

    auto visited = (bool *) calloc(originalGraph.getNbNodes(), sizeof(bool));
    auto depth = (unsigned int *) calloc(originalGraph.getNbNodes(), sizeof(unsigned int));
    auto low = (unsigned int *) calloc(originalGraph.getNbNodes(), sizeof(unsigned int));
    auto parent = (unsigned int *) calloc(originalGraph.getNbNodes(), sizeof(unsigned int));
    std::fill_n(parent, originalGraph.getNbNodes(), -1);
    auto articulation = (bool *) calloc(originalGraph.getNbNodes(), sizeof(bool));

    for (unsigned int node = 0; node < originalGraph.getNbNodes(); ++node) {
        if (!visited[node])
            depthFirstSearch(originalGraph, node, 0, visited, depth, low, parent, articulation);
    }

    std::fill_n(visited, originalGraph.getNbNodes(), false);
    free(depth);
    free(low);
    free(parent);

    auto* replacedReferences = (unsigned int *) calloc(originalGraph.getNbNodes(), sizeof(unsigned int));
    std::fill_n(replacedReferences, originalGraph.getNbNodes(), -1);

    for (unsigned int node = 0; node < originalGraph.getNbNodes(); ++node) {
        if (visited[node] || articulation[node] || originalGraph.isPreColored(node))
            continue;
        component comp = findComponent(originalGraph, node, visited, articulation);

        if (comp.nbAdj == 0 && comp.nbPrecolors == 0) {
            stats.freeArticulation += comp.nodes.size();
            for (unsigned int item: comp.nodes)
                replacements[item] = comp.reference;
        } else if (comp.nbAdj == 1 && comp.nbPrecolors == 0) {
            stats.singleArticulation += comp.nodes.size();
            for (unsigned int item: comp.nodes)
                replacements[item] = comp.reference;
        } else if (comp.nbAdj == 0 && comp.nbPrecolors == 1) {
            stats.singleArticulation += comp.nodes.size();
            for (unsigned int item: comp.nodes)
                replacements[item] = comp.reference;
        }
    }

    free(visited);
    free(articulation);
    free(replacedReferences);

    Graph tempGraph(originalGraph.getNbColors());
    firstReferences = buildReducedGraph(originalGraph, tempGraph, replacements);
    free(replacements);

    secondReferences.resize(tempGraph.getNbNodes());

    for (unsigned int node = 0; node < tempGraph.getNbNodes(); ++node) {
        if (tempGraph.getEdges(node).empty()) {
            if (tempGraph.isPreColored(node))
                secondReferences[node] = originalGraph.getNbNodes() + tempGraph.getColor(node);
            else
                secondReferences[node] = -1;
            ++stats.unconnectedComponent;
        }

        if (onlyConnectedToUnhappyDifferentColor(tempGraph, node)) {
            secondReferences[node] = -1;
            ++stats.unhappyConnections;
        }
    }

    unsigned int nodeCounter = 0;
    for (unsigned int node = 0; node < tempGraph.getNbNodes(); ++node) {
        if (secondReferences[node] < originalGraph.getNbNodes()) {
            if (tempGraph.isPreColored(node))
                reducedGraph.addNode(tempGraph.getColor(node));
            else
                reducedGraph.addNode();
            secondReferences[node] = nodeCounter++;
        }
    }

    for (unsigned int node = 0; node < tempGraph.getNbNodes(); ++node) {
        if (secondReferences[node] < originalGraph.getNbNodes()) {
            for (unsigned int adj: tempGraph.getEdges(node)) {
                if (secondReferences[node] < originalGraph.getNbNodes() && secondReferences[adj] < secondReferences[node])
                    reducedGraph.addEdge(secondReferences[node], secondReferences[adj]);
            }
        }
    }

    reducedGraph.sortEdges();
}


ReducedGraph::ReducedGraph(Graph &original, const config &config) : originalGraph(original),
                                                                    reducedGraph(original.getNbColors()),
                                                                    mode(config.reduct) {
    switch (mode) {
        case config::NONE:
            reducedGraph = original;
            firstReferences.clear();
            break;
        case config::THIRUVADY:
            thiruvadyReduction();
            break;
        case config::BASIC:
            basicReduction();
            break;
        case config::ARTICULATION:
            articulationReduction();
            break;
    }
}

unsigned int ReducedGraph::colorOriginal() {
    reducedGraph.writeToDot("reduced.png");
    switch (mode) {
        case config::NONE:
        case config::THIRUVADY:
            for (unsigned int node = 0; node < originalGraph.getNbNodes(); ++node) {
                if (!originalGraph.isPreColored(node)) {
                    unsigned int reference = firstReferences[node];
                    if (reference == -1)
                        originalGraph.color(node, 1);
                    else
                        originalGraph.color(node, reducedGraph.getColor(reference));
                }
            }
            return stats.singleColorComponent + stats.unconnectedComponent;
        case config::BASIC:
            for (unsigned int node = 0; node < originalGraph.getNbNodes(); ++node) {
                if (!originalGraph.isPreColored(node)) {
                    unsigned int reference = firstReferences[node];
                    if (reference == -1)
                        originalGraph.color(node, 1);
                    else
                        originalGraph.color(node, reducedGraph.getColor(reference));
                }
            }
            return stats.unconnectedComponent + stats.singleLinkChains + stats.singleColorComponent;
        case config::ARTICULATION:
            std::vector<unsigned int> tempColor(secondReferences.size());
            for (unsigned int node = 0; node < secondReferences.size(); ++node) {
                if (secondReferences[node] == -1)
                    tempColor[node] = 1;
                else if (secondReferences[node] > originalGraph.getNbNodes())
                    tempColor[node] = secondReferences[node] - originalGraph.getNbNodes();
                else
                    tempColor[node] = reducedGraph.getColor(secondReferences[node]);
            }
            for (unsigned int node = 0; node < originalGraph.getNbNodes(); ++node) {
                if (!originalGraph.isPreColored(node)) {
                    unsigned int reference = firstReferences[node];
                    if (reference == -1)
                        originalGraph.color(node, 1);
                    else
                        originalGraph.color(node, tempColor[reference]);
                }
            }
            return stats.freeArticulation + stats.singleArticulation + stats.unconnectedComponent;
    }
    return 0;
}

void ReducedGraph::writeStats(std::ostream &out) {

    switch (mode) {
        case config::NONE:
            break;
        case config::THIRUVADY:
            out << "Thiruvady reduce stats: " << std::endl
                << stats.unconnectedComponent << " Nodes removed from unconnected components" << std::endl
                << stats.singleColorComponent << " Nodes removed from single color components" << std::endl
                << stats.unhappyConnections << " Nodes removed only connected to unhappy nodes" << std::endl
                << std::endl;
            break;
        case config::BASIC:
            out << "Basic reduce stats: " << std::endl
                << stats.unconnectedComponent << " Nodes removed from unconnected components" << std::endl
                << stats.singleLinkChains << " Nodes removed from single link chains" << std::endl
                << stats.unhappyConnections << " Nodes removed only connected to unhappy nodes" << std::endl
                << stats.singleColorComponent << " Nodes removed from free components connected to a single color"
                << std::endl
                << std::endl;
            break;
        case config::ARTICULATION:
            out << "Articulation reduce stats: " << std::endl
                << stats.freeArticulation << " Nodes from free components removed in articulation phase" << std::endl
                << stats.singleArticulation << " Nodes removed from single connection compoments in articulation phase" << std::endl
                << stats.unhappyConnections << " Unhappy nodes removed in second phase" << std::endl
                << stats.unconnectedComponent << " Unconnected nodes removed in second phase" << std::endl
                << std::endl;
            break;
    }

}

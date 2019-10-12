#include <fstream>
#include <iostream>
#include <cstring>
#include "Graph.h"

using namespace std;

Graph::Graph(const std::string &fileName) : nbColors(0), nbNodes(0) {
    ifstream inStream;
    inStream.open(fileName);
    if (inStream.fail())
        throw runtime_error("Failed to open file");

    char c, line[512];
    int fileNbNodes = 0, nbEdges = 0;
    vector<pair<unsigned int, unsigned int>> edges;
    vector<pair<unsigned int, unsigned int>> preColoring;

    while (!inStream.eof()) {
        inStream.get(c);
        if (inStream.eof()) break;
        switch (c) {
            case 'p':
                inStream.get();
                inStream.getline(line, 512, ' ');
                if (strcmp(line, "edge") != 0)
                    throw runtime_error("No edge keyword found in problem line");
                inStream >> fileNbNodes >> nbEdges >> nbColors;
                break;
            case 'e':
                unsigned int from, to;
                inStream >> from >> to;
                edges.emplace_back(from - 1, to - 1);
                break;
            case 'n':
                unsigned int node, color;
                inStream >> node >> color;
                preColoring.emplace_back(node - 1, color);
                break;
            case 'c':
                inStream.putback(c);
                inStream.get(line, 512, '\n');
                break;
            default:
                throw runtime_error("Undefined line control character");
        }
        inStream.get();
    }
    inStream.close();

    sort(preColoring.begin(), preColoring.end());
    int preColorCounter = 0;
    auto preColor = preColoring[0];
    for (unsigned int i = 0; i < fileNbNodes; ++i) {
        if (preColorCounter < preColoring.size()) {
            if (preColor.first == i) {
                addNode(preColor.second);
                ++preColorCounter;
                preColor = preColoring[preColorCounter];
                continue;
            } else if (preColor.first < i)
                throw runtime_error("A node was colored multiple times");
        }
        addNode();
    }
    if (preColorCounter != preColoring.size())
        throw runtime_error("A precoloring was given for a node with an index out of bounds");

    if (nbEdges != edges.size())
        throw runtime_error("Defined nubmer of edges is not equal to the amount of 'e' lines");

    for (auto edge : edges) {
        if (hasEdge(edge.first, edge.second))
            throw runtime_error("An edge was specified multiple times");
        addEdge(edge.first, edge.second);
    }
}

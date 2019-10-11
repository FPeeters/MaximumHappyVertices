#include <fstream>
#include <iostream>
#include <cstring>
#include "Graph.h"

using namespace std;

Graph::Graph(const std::string &fileName) : nbColors(0) {
    ifstream inStream;
    inStream.open(fileName);
    if (inStream.fail())
        throw runtime_error("Failed to open file");

    char c, line[128];
    int nbEdges = 0;
    vector<pair<unsigned int, unsigned int>> edges;
    vector<pair<unsigned int, unsigned int>> preColoring;

    while (!inStream.eof()) {
        inStream.get(c);
        switch (c) {
            case 'p':
                inStream.getline(line, 128, ' ');
                if (strcmp(line, "edge") != 0)
                    throw runtime_error("No edge keyword found in problem line");
                inStream >> nbNodes >> nbEdges >> nbColors;
                edges.resize(nbEdges);
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
                break;
            default:
                throw runtime_error("Undefined line control character");
        }
    }
    inStream.close();

    if (nbEdges != edges.size())
        throw runtime_error("Defined nubmer of edges is not equal to the amount of 'e' lines");

    sort(preColoring.begin(), preColoring.end());
    int preColorCounter = 0;
    for (unsigned int i = 0; i < nbNodes; ++i) {
        if (preColorCounter != preColoring.size()) {

        }
        const auto preColor = preColoring[preColorCounter];
        if (preColor.first == i) {
            addNode(preColor.second);
            ++preColorCounter;
        } else if (preColor.first < i) {
            throw runtime_error("A node was colored multiple times");
        }
        addNode();
    }




}

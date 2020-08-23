#include <fstream>
#include <cstring>
#include <iostream>
#include "Graph.h"

using namespace std;

Graph::Graph(const std::string &fileName) : nbColors(0), nbNodes(0), nodes() {
    ifstream inStream;
    inStream.open(fileName);
    if (inStream.fail())
        throw runtime_error("Failed to open file");

    char c, line[512];
    int fileNbNodes = 0, nbEdges = 0;
    vector<pair<unsigned int, unsigned int>> edges;
    vector<pair<unsigned int, unsigned int>> preColoring;
    unsigned int temp1 = 0, temp2 = 0;

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
                inStream >> temp1 >> temp2;
                edges.emplace_back(temp1 - 1, temp2 - 1);
                break;
            case 'n':
                inStream >> temp1 >> temp2;
                preColoring.emplace_back(temp1 - 1, temp2);
                break;
            case 'c':
                inStream.putback(c);
                inStream.get(line, 512, '\n');
                break;
            case '\n':
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
                if (preColorCounter < preColoring.size())
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
        throw runtime_error("Defined number of edges is not equal to the amount of 'e' lines");

    for (auto edge : edges) {
        if (hasEdge(edge.first, edge.second))
            throw runtime_error("An edge was specified multiple times");
        addEdge(edge.first, edge.second);
    }

    sortEdges();
}

bool Graph::isHappy(const unsigned int node) {
    if (nodes[node].happyCache != X)
        return nodes[node].happyCache == H;

    bool happy = true;
    for (unsigned int adj: getEdges(node)) {
        if (getColor(node) != getColor(adj)) {
            happy = false;
            break;
        }
    }
    nodes[node].happyCache = (happy ? H : U);
    return happy;
}

unsigned int Graph::getHappyVertices() {
    unsigned int count = 0;
    for (unsigned int node = 0; node < nbNodes; ++node) {
        if (isHappy(node))
            ++count;
    }
    return count;
}


void Graph::writeToDot(const std::string &filename) {
    ofstream out;
    out.open(filename + ".dot");

    if (out.fail()) {
        cout << "Failed to open .dot file." << endl;
        return;
    }

    out << "graph {" << endl;
    out << "graph [overlap=false]" << endl;
    out << "node [style=filled,width=0.05,label=\"\",colorscheme=svg,fixedsize=true]" << endl;

    if (nbColors > 10) {
        cout << "Only graphs with up to 10 different colors are supported." << endl;
        return;
    }

    static const std::string COLORS[11]{"black", "darkgreen", "darkblue", "maroon", "red", "gold", "lawngreen",
                                        "fuchsia",
                                        "cornflowerblue", "aqua", "peachpuff"};

    for (unsigned int node = 0; node < nbNodes; ++node) {
        if (isPreColored(node))
            out << "\tn" << node << " [shape=square,color=" << COLORS[getColor(node)];
        else
            out << "\tn" << node << " [shape=circle,color=" << COLORS[getColor(node)];
//        out << ",label=" << node;
        if (!isHappy(node))
            out << ",label=U";
        out << "]" << endl;
    }

    for (unsigned int node = 0; node < nbNodes; ++node) {
        bool first = true;
        for (unsigned int adj: getEdges(node)) {
            if (adj > node) {
                if (first) {
                    out << "\t n" << node << " -- { ";
                    first = false;
                }
                out << " n" << adj << " ";
            }
        }
        if (!first)
            out << "}" << endl;
    }

    out << "}" << endl;
    out.close();

    system(("sfdp " + filename + ".dot -Tpng -o" + filename + " >nul 2>&1").c_str());
    remove((filename + ".dot").c_str());
}

void Graph::writeToFile(const std::string &filename) {
    ofstream out;
    out.open(filename);

    if (out.fail()) {
        cout << "Failed to open output file." << endl;
        return;
    }

    for (unsigned int node = 0; node < nbNodes; ++node) {
        out << node << "\t|\t";

        if (isHappy(node))
            out << "H" << "\t|\t";
        else
            out << "U" << "\t|\t";

        if (isPreColored(node))
            out << "*";
        else
            out << " ";

        out << getColor(node) << "\t|\t";
        for (auto adj: getEdges(node))
            out << adj << ", ";
        out << endl;
    }

    out.close();
}

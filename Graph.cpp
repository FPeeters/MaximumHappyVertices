#include <fstream>
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
        throw runtime_error("Defined nubmer of edges is not equal to the amount of 'e' lines");

    for (auto edge : edges) {
        if (hasEdge(edge.first, edge.second))
            throw runtime_error("An edge was specified multiple times");
        addEdge(edge.first, edge.second);
    }
}

bool Graph::isHappy(const unsigned int node) const {
    bool happy = true;
    for (unsigned int adj: getEdges(node)) {
        if (getColor(node) != getColor(adj)) {
            happy = false;
            break;
        }
    }
    return happy;
}

unsigned int Graph::getHappyVertices() const {
    unsigned int count = 0;
    for (unsigned int node = 0; node < nbNodes; ++node) {
        if (isHappy(node))
            ++count;
    }
    return count;
}

void Graph::writeToDot(const std::string &filename) const {
    ofstream out;
    out.open(filename + ".dot");
    if (out.fail()) throw runtime_error("Failed to open file");
    out << "graph {" << endl;
    out << "graph [overlap=false]" << endl;
    out << "node [style=filled,width=0.05,label=\"\",colorscheme=svg,fixedsize=true]" << endl;

    for (unsigned int node=0; node < nbNodes; ++node) {
        if (isPreColored(node))
            out << "\tn" << node << " [shape=square,color=" << COLORS[getColor(node)] << "]" << endl;
        else
            out << "\tn" << node << " [shape=circle,color=" << COLORS[getColor(node)] << "]" << endl;
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

    system(("sfdp " + filename + ".dot -Tpng -o" + filename).c_str());
    remove((filename + ".dot").c_str());
}

void Graph::writeToFile(const std::string &filename) const {
    ofstream out;
    out.open(filename);
    if (out.fail()) throw runtime_error("Failed to open file");

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

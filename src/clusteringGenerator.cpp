#include <cstdlib>
#include <iostream>
#include <random>
#include <fstream>
#include <algorithm>
#include "linear_int_distribution.h"

typedef std::mt19937_64 Rng;

struct adjacency_item {
    unsigned int index;
    std::vector<unsigned int> adjacency;
    unsigned int color = 0;
};

bool compare(const adjacency_item &first, const adjacency_item &second) {
    return first.adjacency.size() < second.adjacency.size();
}

bool compareReverse(const adjacency_item &first, const adjacency_item &second) {
    if (first.adjacency.size() == second.adjacency.size())
        return first.index < second.index;
    return first.adjacency.size() > second.adjacency.size();
}

int main(int argc, char **argv) {
    if (argc != 8) {
        std::cout << "USAGE: filename nbNodes avgDegree alpha nbColors precolor seed" << std::endl;
        return EXIT_FAILURE;
    }

    char *filename = argv[1];
    int nbNodes = (int) strtol(argv[2], nullptr, 10);
    double avgDegree = strtod(argv[3], nullptr);
    double alpha = strtod(argv[4], nullptr);
    int nbColors = (int) strtol(argv[5], nullptr, 10);
    double precolor = strtod(argv[6], nullptr);
    long seed = strtol(argv[7], nullptr, 10);

    int nbPrecolor = (int) round(nbNodes * precolor);
    if (nbPrecolor < nbColors) {
        std::cout << "At least nbColor nodes must be precolored" << std::endl;
        return EXIT_FAILURE;
    }
    if (avgDegree > nbNodes - 1) {
        std::cout << "The maximum average degree is nbNodes-1" << std::endl;
        return EXIT_FAILURE;
    }

    Rng rng(seed);

    std::vector<adjacency_item> adjacency(nbNodes);
    int i = 0;
    for (adjacency_item &item: adjacency)
        item.index = i++;

    int nbEdges = (int) round(avgDegree * nbNodes / 2.);

    int nbFilled = 0;

    for (int j = 0; j < nbEdges; ++j) {
        int curr_range = nbNodes - nbFilled;
        linear_int_distribution<int> firstDistr(alpha, 0, curr_range);
        int first = firstDistr(rng);
        linear_int_distribution<int> secondDistr(alpha, 0, curr_range - 1);
        int second = secondDistr(rng);
        if (second >= first)
            second += 1;

        int dir = alpha > 0 ? 1 : -1;

        int start = second;
        std::vector<unsigned int> &firstAdj = adjacency[first].adjacency;
        while (second == first ||
               std::find(firstAdj.begin(), firstAdj.end(), adjacency[second].index) != firstAdj.end()) {
            second += dir;
            if (second == nbNodes) {
                second = start - 1;
                dir = -1;
            } else if (second == -1) {
                second = start + 1;
                dir = 1;
            }
        }

        std::vector<unsigned int> &secondAdj = adjacency[second].adjacency;

        firstAdj.push_back(adjacency[second].index);
        secondAdj.push_back(adjacency[first].index);

        if (firstAdj.size() == nbNodes - 1)
            ++nbFilled;
        if (secondAdj.size() == nbNodes - 1)
            ++nbFilled;

        std::sort(adjacency.begin(), adjacency.end(), compare);
    }

    std::vector<int> renames(nbNodes);

    i = 0;
    for (const adjacency_item &item: adjacency)
        renames[item.index] = i++;

    for (adjacency_item &item: adjacency) {
        item.index = nbNodes - renames[item.index];
        for (unsigned int &j : item.adjacency)
            j = nbNodes - renames[j];
        std::sort(item.adjacency.begin(), item.adjacency.end());
    }

    std::shuffle(adjacency.begin(), adjacency.end(), rng);

    for (unsigned int col = 0; col < nbColors; ++col)
        adjacency[col].color = col + 1;

    std::uniform_int_distribution colorDistr(1, nbColors);
    for (unsigned int j = nbColors; j < nbPrecolor; ++j)
        adjacency[j].color = colorDistr(rng);

    std::sort(adjacency.begin(), adjacency.end(), compareReverse);

    std::ofstream out;
    out.open(filename);

    out << "c This graph was made by the clusteringGenerator of F. Peeters" << std::endl;
    out << "c Using these settings: " << nbNodes << " " << avgDegree << " " << alpha << " " << nbColors << " " << precolor << " " << seed << std::endl;
    out << "p edge " << nbNodes << " " << nbEdges << " " << nbColors << std::endl;

    for (const adjacency_item &item: adjacency) {
        for (unsigned int adj: item.adjacency) {
            if (item.index < adj)
                out << "e " << item.index << " " << adj << std::endl;
        }
    }

    for (const adjacency_item &item: adjacency) {
        if (item.color != 0)
            out << "n " << item.index << " " << item.color << std::endl;
    }
    out.close();
}
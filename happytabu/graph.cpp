#include "graph.h"

void makeGraphFromAdjacencyMatrix(GRAPH &G, vector<vector<bool> > &A, vector<int> &pCol, int numCols) {
	int i, j, l;
	//If we are here, we assume that a valid input file has been parsed. We now construc the graph G;
	G.UB = 0;
	G.n = pCol.size();
	G.m = 0;
	G.k = numCols;
	G.adj = A;
	G.precol = pCol;
	G.precolList.clear();
	G.freeList.clear();
	G.numPreass.clear();
	G.numPreass.resize(G.k, 0);
	for (i = 0; i < G.n; i++) {
		if (G.precol[i] != -1) {
			G.precolList.push_back(i);
			G.numPreass[G.precol[i]]++;
		}
		else G.freeList.push_back(i);
	}
	//Use the adjacency matrix to construct the adj list and degree array, and the number of edges
	G.deg.clear();
	G.deg.resize(G.n, 0);
	G.adjList.clear();
	G.adjList.resize(G.n, vector<int>());
	for (i = 0; i < G.n; i++) {
		for (j = 0; j < G.n; j++) {
			if (G.adj[i][j] && i != j) {
				G.adjList[i].push_back(j);
				G.deg[i]++;
				G.m++;
			}
		}
	}
	G.m = G.m / 2;
	//and calculate the min and max degree
	G.minDeg = G.deg[0];
	G.maxDeg = G.deg[0];
	for (i = 1; i < G.n; i++) {
		if (G.deg[i] > G.maxDeg) G.maxDeg = G.deg[i];
		if (G.deg[i] < G.minDeg) G.minDeg = G.deg[i];
	}
	//now figure out the pairs of vertices that are of distance two apart
	int u, v;
	vector<vector<bool> > distTwo(G.n, vector<bool>(G.n, false));
	for (i = 0; i < G.n; i++) {
		if (G.deg[i] > 0) {
			for (j = 0; j < G.adjList[i].size() - 1; j++) {
				for (l = j + 1; l < G.adjList[i].size(); l++) {
					v = G.adjList[i][j];
					u = G.adjList[i][l];
					if (!G.adj[u][v]) {
						//u and v are neighbours with i, but not with each other. So they have distance of two
						distTwo[u][v] = true;
						distTwo[v][u] = true;
					}
				}
			}
		}
	}
	//and populate the distTwoList
	G.distTwoList.clear();
	G.distTwoList.resize(G.n, vector<int>());
	for (i = 0; i < G.n; i++) {
		for (j = 0; j < G.n; j++) {
			if (distTwo[i][j]) G.distTwoList[i].push_back(j);
		}
	}
}
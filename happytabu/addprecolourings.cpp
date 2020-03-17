#include "addprecolourings.h"

extern int verbosity;

void getFreeComponent(int s, GRAPH &G, vector<int> &S) {
	//A modified version of BFS that starts atvertex s (which is free) and explores the graph induced by free vertices only. 
	//A list S is returned that gives all vertices reachable from S (including s itself)
	int u, v, i;

	//Set up the arrays
	S.clear();
	vector<int> col(G.n, WHITE);
	deque<int> Q;
	
	//Start BFS
	col[s] = GREY;
	Q.push_back(s);
	while (!Q.empty()) {
		u = Q.front();
		for (i = 0; i < G.adjList[u].size(); i++){
			v = G.adjList[u][i];
			if (G.precol[v] < 0) {
				//v is a free vertex
				if (col[v] == WHITE) {
					//Have discovered a new unvisited vertex v from u
					col[v] = GREY;
					Q.push_back(v);
				}
			}
		}
		Q.pop_front();
		col[u] = BLACK;
	}
	//Get all the vertices in the component
	for (i = 0; i < G.n; i++) if (col[i] == BLACK) S.push_back(i);
}

int getColOfNeighbours(GRAPH &G, vector<int> &S) {
	//S is a component in the graph induced by free vertices. This procedure returns -1 if the vertices in S have no precoloured neighbours in G,
	//NIL if S has more than one colour in adjacent preassignments, and the colour itself (uCol) otherwise.
	int v, u, j, i, uCol = -1;
	for (i = 0; i < S.size(); i++) {
		v = S[i];
		for (j = 0; j < G.adjList[v].size(); j++) {
			u = G.adjList[v][j];
			if (G.precol[u] >= 0) {
				if (uCol < 0) {
					//u is a precoloured vertex that neighbours v
					uCol = G.precol[u];
				}
				else if (uCol >= 0 && uCol != G.precol[u]) {
					//S neighbours precoloured vertices of more than one colour, so quit
					return NIL;
				}
			}
		}
	}
	return uCol;
}

bool moreThanOneAdjacentColour(GRAPH &G, int v) {
	int i, u, uCol = -1;
	for (i = 0; i < G.adjList[v].size(); i++) {
		u = G.adjList[v][i];
		if (G.precol[u] >= 0) {
			if (uCol < 0) {
				//u is a precoloured vertex that neighbours v
				uCol = G.precol[u];
			}
			else if (uCol >= 0 && uCol != G.precol[u]) {
				//have identified that v neighbours more than one colour
				return true;
			}
		}
	}
	//If we are here, v does not have more than one adjacent colour.
	return false;
}

int getStatus(GRAPH &G, int v) {
	//Takes an individual vertex in G and returns an integer indicating whether is is a U-vertex (1), L_u vertex (2), or otherwise (3)
	int u, i;
	if (G.precol[v] >= 0) {
		//v is precoloured. Check if it has at least one neighbour of a different colour (making it a U-vertex)
		for (i = 0; i < G.adjList[v].size(); i++) {
			u = G.adjList[v][i];
			if (G.precol[u] >= 0 && G.precol[u] != G.precol[v]) return 1;
		}
		return 3;
	}
	else {
		//v is not precoloured. Check to see if it has neighbours of two or more different colours (making it an L_u-vertex)
		if (moreThanOneAdjacentColour(G, v)) return 2;
		else return 3;
	}
}

bool noPotentiallyHappyNeighbours(GRAPH &G, int v, vector<int> &status) {
	int i, u;
	//Returns true only if all of v's neighbours are U or L_u vertices (i.e. guaranteed to be unhappy)
	for (i = 0; i < G.adjList[v].size(); i++) {
		u = G.adjList[v][i];
		if (status[u] == 3) return false;
	}
	return true;
}

void addPrecolourings(GRAPH &G) {
	int v, u, col, i;
	vector<int> S;
	//Try to identify free vertices that can be precoloured. Do this by looking at each free vertex, IDing free vertices that can be reached from it, and putting
	//these into a set S. Then look at the precoloured neighbours of S in G
	for (v = 0; v < G.n; v++) {
		if (G.precol[v] < 0) {
			getFreeComponent(v, G, S);
			col = getColOfNeighbours(G, S);
			if (col == -1) {
				//Have identified a connected set of free vertices that are not adjacent to any precoloured vertices. They can all be precoloured to an arbitrary colour
				col = rand() % G.k;
				for (i = 0; i < S.size(); i++) {
					u = S[i];
					G.precol[u] = col;
					G.numPreass[col]++;
				}
				if (verbosity >= 1) {
					cout << "The set of free vertices { ";
					for (i = 0; i < S.size(); i++) cout << S[i] << " ";
					cout << "} has been precoloured to the same arbitrary colour (" << col << ")." << endl;
				}
			}
			else if (col >= 0) {
				//Have identified a connected set of free vertices that are adjacent to precoloured vertices of just one colour. They can all be precoloured with this colour
				for (i = 0; i < S.size(); i++) {
					u = S[i];
					G.precol[u] = col;
					G.numPreass[col]++;
				}
				if (verbosity >= 1) {
					cout << "The set of free vertices = { ";
					for (i = 0; i < S.size(); i++) cout << S[i] << " ";
					cout << "} has been precoloured to colour " << col << "." << endl;
				}
			}
		}
	}
	//Now identify (free) L_u vertices that can be precoloured. (I.e. ones who are free, destined to be unhappy and whose neighbours are unhappy. 
	//These can be assigned to any arbitrarty colour. The staus labels are as follows: U-vertex (1), L_u vertex (2), or (3) otherwise
	vector<int> status(G.n);
	for (v = 0; v < G.n; v++) status[v] = getStatus(G, v);
	for (v = 0; v < G.n; v++) {
		if (status[v] == 2) {
			if (noPotentiallyHappyNeighbours(G, v, status)) {
				//We can precolour v to an arbitrary colour
				col = rand() % G.k;
				G.precol[v] = col;
				G.numPreass[col]++;
				if (verbosity >= 1) {
					cout << "Vertex-" << v << " (and its neighbours) are guaranteed to be unhappy. It has been precoloured to an arbitrary colour (" << col << ")." << endl;
				}
			}
		}
	}
	//Finally we need to update the PreCol and Free lists
	G.precolList.clear();
	G.freeList.clear();
	for (i = 0; i < G.n; i++) {
		if (G.precol[i] >= 0) G.precolList.push_back(i);
		else G.freeList.push_back(i);
	}
}

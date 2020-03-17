#include "initsol.h"

//---Functions for GreedyMHV-------------------------------------------------------------------
void evaluate(GRAPH &G, SOL &S)
{
	int v, u, i;
	S.numHappy = G.n;
	//Do a full evaluation of a solution S and update the relevant data structures
	for (v = 0; v < G.n; v++) {
		//Assume v is happy until determined otherwise
		S.isHappy[v] = true;
		for (i = 0; i < G.adjList[v].size(); i++) {
			u = G.adjList[v][i];
			if (S.col[v] != S.col[u]) {
				//v is adjacent to a different colour vertex so it is unhappy. 
				S.isHappy[v] = false;
				S.numHappy--;
				break;
			}
		}
	}
}

void emptyColourClass(GRAPH &G, SOL &S, int c) {
	//Removes all free vertices from colour class c
	int i, v;
	int numToDelete = S.items[c].size() - G.numPreass[c];
	for (i = 0; i < numToDelete; i++) {
		v = S.items[c].back();
		S.col[v] = -1;
		S.items[c].pop_back();
	}
}

void fillColourClass(GRAPH &G, SOL &S, int c) {
	//Puts all free vertices into colour class c. Assumes S.items only contains precoloured at this point
	int v;
	for (v = 0; v < G.n; v++) {
		if (G.precol[v] == -1) {
			S.items[c].push_back(v);
			S.col[v] = c;
		}
	}
}

void GreedyMHV(GRAPH &G, SOL &S) {
	int j, bestJ, maxHappy = INT_MIN;
	for (j = 0; j < G.k; j++) {
		//Assign all free vertices to colour class j
		fillColourClass(G, S, j);
		evaluate(G, S);
		//Check if this is the best so far
		if (S.numHappy > maxHappy) {
			maxHappy = S.numHappy;
			bestJ = j;
		}
		emptyColourClass(G, S, j);
	}
	//Produce the solution by putting all free vertices into the best colour class
	fillColourClass(G, S, bestJ);
	evaluate(G, S);
}

//---Functions for GrowthMHV--------------------------------------------------------------------
int determineColouredVertexStatus(GRAPH &G, int v, SOL &S, vector<int> &status, int &numHappy) {
	//Determines the status of a coloured vertex v by examining it neighbours u. Updates the numHappy variable where apt.
	if (S.col[v] < 0) exitAndLog("Error: have called the determineColouredVertexStatus function with an uncoloured vertex. Exiting...\n");
	//If a coloured vertex is already happy or destined to be unhappy, its status cannot change, so we end
	if (status[v] == 0 || status[v] == 2) return status[v];
	//The status of this vertex is P and therfore has the potential to change, so calculate its new status

	int oldstatus = status[v];

	int i, u;
	bool freeNeighbour = false;
	for (i = 0; i < G.adjList[v].size(); i++) {
		u = G.adjList[v][i];
		if (S.col[u] == -1) {
			freeNeighbour = true;
		}
		else if (S.col[v] != S.col[u]) {
			//v cannot be happy, so end immediately
			return 2;
		}
	}
	//If we are here, no neighbours of v are coloured differently
	if (!freeNeighbour) {
		//v has become happy
		numHappy++;
		S.isHappy[v] = true;
		return 0;
	}
	else {
		//v has uncoloured neighbours and has the potential to be happy
		return 1;
	}
}

int determineFreeVertexStatus(GRAPH &G, int v, SOL &S, vector<int> &status) {
	//Determines the status of an uncoloured (free) vertex v examining its neighbours u
	if (S.col[v] >= 0) exitAndLog("Error: have called the determineFreeVertexStatus function with a coloured vertex. Exiting...\n");
	int i, u, anAdjColour = -1;
	bool multipleAdjColoursToV = false;
	for (i = 0; i < G.adjList[v].size(); i++) {
		u = G.adjList[v][i];
		if (status[u] == 1) {
			//v is an L_p vertex (adjacent to a potentially happy, coloured vertex u). End immediately
			return 3;
		}
		if (S.col[u] != -1 && anAdjColour == -1) {
			//at least one colour adjacent to v
			anAdjColour = S.col[u];
		}
		else if (S.col[u] != -1 && S.col[u] != anAdjColour) {
			//have observed at least two diff colours adjacent to v, so v cannot be happy
			multipleAdjColoursToV = true;
		}
	}
	//If we are here, v is not adjacent to a P-vertex -- it's status must be 4, 5, or 6.
	if (anAdjColour != -1 && !multipleAdjColoursToV) {
		//v is adjacent to vertices of only one colour, it has the potential to be happy
		return 4;
	}
	else if (multipleAdjColoursToV) {
		//v is adjacent to vertices of more than one colour, it is destined to be unhappy
		return 5;
	}
	else {
		//all of v's neighbours are uncoloured.
		return 6;
	}
}

int getNextVertexToColour(GRAPH &G, vector<int> &status, bool &PVertexExists, bool &LHVertexExists, bool &LUVertexExists) {
	int i;
	PVertexExists = LHVertexExists = LUVertexExists = false;
	int LUVertexPos = -1, LFVertexPos = -1;
	//Look for a coloured, potentially happy vertex v (a P-vertex)
	for (i = 0; i < G.n; i++) {
		if (status[i] == 1) {
			PVertexExists = true;
			return i;
		}
	}
	//If we are here, we need to look for an uncoloured potentially happy, and then uncoloured unhappy, then uncoloured free vertex
	for (i = 0; i < G.n; i++) {
		if (status[i] == 4) {
			LHVertexExists = true;
			return i;
		}
		else if (status[i] == 5) {
			LUVertexExists = true;
			LUVertexPos = i;
		}
		else if (status[i] == 6) {
			LFVertexPos = i;
		}
	}
	//If we are here, we have not found a P-vertex or a LH-vertex, but we have found a LU-vertex or, failing that, a LF-vertex
	if (LUVertexExists) {
		return LUVertexPos;
	}
	else if (LFVertexPos < status.size()) {
		return LFVertexPos;
	}
	else {
		exitAndLog("Error: Should not be here in the getNextVertexToColour function\n");
		return 0;
	}
}

void updateNeighboursStatus(GRAPH &G, int v, SOL &S, vector<int> &status, int &numHappy) {
	//A vertex v has just been coloured. Update the status of all neigbours
	int u, j;
	for (j = 0; j < G.adjList[v].size(); j++) {
		u = G.adjList[v][j];
		if (S.col[u] != -1) {
			status[u] = determineColouredVertexStatus(G, u, S, status, numHappy);
		}
		else {
			status[u] = determineFreeVertexStatus(G, u, S, status);
		}
	}
}

void updateDistanceTwo(GRAPH &G, int v, vector<int> &status, SOL &S, int &numHappy) {
	//A vertex v and its neighbours have just been coloured with the same colour and v is happy. 
	int u, j, l, w, oldState;
	status[v] = 0;
	numHappy++;
	S.isHappy[v] = true;
	//Update the statuses of all vertices adjacent to v
	for (j = 0; j < G.adjList[v].size(); j++) {
		u = G.adjList[v][j];
		status[u] = determineColouredVertexStatus(G, u, S, status, numHappy);
	}
	//update status of each vertex u that has a distance of two from v
	for (j = 0; j < G.distTwoList[v].size(); j++) {
		u = G.distTwoList[v][j];
		if (S.col[u] != -1) {
			oldState = status[u];
			status[u] = determineColouredVertexStatus(G, u, S, status, numHappy);
			//This is the additional bit for dealing with the special cases
			if (oldState != status[u]) {
				for (l = 0; l < G.adjList[u].size(); l++) {
					w = G.adjList[u][l];
					if (S.col[w] == -1) {
						status[w] = determineFreeVertexStatus(G, w, S, status);
					}
				}
			}
		}
		else {
			status[u] = determineFreeVertexStatus(G, u, S, status);
		}
	}
}

int getAdjacentColour(GRAPH &G, int v, SOL &S) {
	//Return a colour adjacent to vertex v
	int j, u;
	for (j = 0; j < G.adjList[v].size(); j++) {
		u = G.adjList[v][j];
		if (S.col[u] != -1) {
			return S.col[u];
		}
	}
	//If we are here there is a problem
	exitAndLog("Error: should not be here in function getAdjacentColour\n");
	return 0;
}

int getUnhappyColouredNeighbour(GRAPH &G, int v, vector<int> &status) {
	//Return a vertex adjacent to v that is unhappy
	int j, u;
	for (j = 0; j < G.adjList[v].size(); j++) {
		u = G.adjList[v][j];
		if (status[u] == 2) {
			return u;
		}
	}
	//If we are here there is a problem
	exitAndLog("Error: should not be here in function getUnhappyColouredNeighbour\n");
	return 0;
}

void GrowthMHV(GRAPH &G, SOL &S) {
	//Constructive algorithm for Happy Colouring Problem. Note that the precolourings have already been defined 
	//Labels in the status array are as follows:
	// 0: (H-Vertex) Coloured and happy
	// 1: (P-vertex) Coloured and has potential to be happy
	// 2: (U-vertex) Coloured and destined to be unhappy
	//---------
	// 3: (L_p-vertex) Not yet coloured, and adjacent to a P-vertex 
	// 4: (L_h-vertex) Not yet coloured, not adjacent to a P-vertex, but has potential to be happy 
	// 5: (L_u-vertex) Not yet coloured, not adjacent to a P-vertex, but destined to be unhappy 
	// 6: (L_f-vertex) Not yet coloured, and not adjacent to any colured vertex
	int v, u, i, j;
	int numColoured = G.precolList.size();
	int numHappy = 0;

	//Determine initial status of all coloured vertices, and then all uncoloured vertices
	vector<int> status(G.n, -1);
	for (i = 0; i < G.n; i++) {
		if (S.col[i] != -1) {
			status[i] = determineColouredVertexStatus(G, i, S, status, numHappy);
		}
	}
	for (i = 0; i < G.n; i++) {
		if (S.col[i] == -1) status[i] = determineFreeVertexStatus(G, i, S, status);
	}

	//Main algorithm
	bool PVertex, LHVertex, LUVertex;
	while (numColoured < G.n) {
		//Chooose next vertex v to colour 
		v = getNextVertexToColour(G, status, PVertex, LHVertex, LUVertex);
		if (PVertex) {
			//v is a P-vertex with colour i. Colour all of its uncoloured neighbours u, with colour i
			i = S.col[v];
			for (j = 0; j < G.adjList[v].size(); j++) {
				u = G.adjList[v][j];
				if (status[u] == 3 && S.col[u] == -1) {
					S.col[u] = i;
					S.items[i].push_back(u);
					numColoured++;
				}
			}
			//update status of all vertices within distance 2 of v
			updateDistanceTwo(G, v, status, S, numHappy);
		}
		else if (LHVertex) {
			//v is a LH-vertex adjacent to a colour i. Colour v and all its its uncoloured neighbours u with i.
			i = getAdjacentColour(G, v, S);
			S.col[v] = i;
			S.items[i].push_back(v);
			numColoured++;
			for (j = 0; j < G.adjList[v].size(); j++) {
				u = G.adjList[v][j];
				if (S.col[u] == -1) {
					S.col[u] = i;
					S.items[i].push_back(u);
					numColoured++;
				}
			}
			//update status of all vertices within distance 2 of v
			updateDistanceTwo(G, v, status, S, numHappy);
		}
		else if (LUVertex) {
			//v is an LU-vertex. When we colour it, it will be unhappy. Also, it must have an unhappy neighbour currently
			u = getUnhappyColouredNeighbour(G, v, status);
			i = S.col[u];
			S.col[v] = i;
			S.items[i].push_back(v);
			numColoured++;
			status[v] = 2;
			//update the status of v's neighbours
			updateNeighboursStatus(G, v, S, status, numHappy);
		}
		else {
			//v must be an LF-vertex (i.e. v is in a component in which none of the vertices are coloured). First choose a random colour for v
			i = rand() % G.k;
			S.col[v] = i;
			S.items[i].push_back(v);
			numColoured++;
			if (G.adjList[v].empty()) {
				//v is an isolated vertex so it is happy
				status[v] = 0;
				S.isHappy[v] = true;
				numHappy++;
			}
			else {
				//v has neighbours so we set it as potentially happy and update its neighbours
				status[v] = 1;
				updateNeighboursStatus(G, v, S, status, numHappy);
			}
		}
	}
	S.numHappy = numHappy;
}

//---Function for producing an initial solution------------------------------------------------
void makeInitSol(GRAPH &G, SOL &S) {
	
	int i, v;
	
	//Clear the solution and set up the data structures
	S.items.clear();
	S.items.resize(G.k, vector<int>());
	S.col.clear();
	S.col.resize(G.n, -1);
	S.isHappy.clear();
	S.isHappy.resize(G.n, false);
	S.numHappy = 0;

	//Assign all of the precolurings defined in G
	for (i = 0; i < G.precolList.size(); i++) {
		v = G.precolList[i];
		S.items[G.precol[v]].push_back(v);
		S.col[v] = G.precol[v];
	}
	
	//Make a copy of S called SPrime
	SOL SPrime = S;

	//Produce solutions using GreedyMHV and GrowthMHV and return the best of these
	GreedyMHV(G, S);
	GrowthMHV(G, SPrime);
	if (SPrime.numHappy > S.numHappy) {
		swap(S,SPrime);
	}
}


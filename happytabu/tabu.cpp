#include "IO.h"

extern int verbosity;

int getTabuTenure(GRAPH &G, SOL &S, double tabuPameter) {
	return int(tabuPameter * (G.UB - S.numHappy)) + ((rand() % 9) + 1);
}

bool allNeighboursHaveColourJ(GRAPH &G, SOL &S, int v, int j) {
	//Returns true only if all the neighbours of v have colour j
	int i;
	for (i = 0; i < G.adjList[v].size(); i++) {
		if (S.col[G.adjList[v][i]] != j) {
			return false;
		}
	}
	return true;
}

bool allNeighboursOfUExceptVHaveColourJ(GRAPH &G, SOL &S, int u, int v, int j) {
	//Returns true only if all the neighbours of u (except v) have colour j
	int i;
	for (i = 0; i < G.adjList[u].size(); i++) {
		if (G.adjList[u][i] != v && S.col[G.adjList[u][i]] != j) {
			return false;
		}
	}
	return true;
}

int calculateCEntry(GRAPH &G, SOL &S, int v, int j) {
	int newHappy = 0, l , u;
	if (S.col[v] == j || G.deg[v] == 0) {
		//v is already in col, or has no neighbours
		return 0;
	}
	//if we are here, v has neighbours. So calculate the effect of changing v's colour
	if (S.isHappy[v]) {
		//v is happy, so moving it to a different colour j will make it unhappy
		newHappy--;
	}
	else if (allNeighboursHaveColourJ(G, S, v, j)) {
		//v is unhappy. If all it neighbours are colour j, then colouring v with j will make it happy
		newHappy++;
	}
	//Also calculate the effect on the neighbours u of v
	for (l = 0; l < G.adjList[v].size(); l++) {
		u = G.adjList[v][l];
		if (S.isHappy[u]) {
			//u is happy. So if its neighbour v is reassigned, it will become unhappy
			newHappy--;
		}
		else if (S.col[u] == j && allNeighboursOfUExceptVHaveColourJ(G, S, u, v, j)) {
			//u is assigned to colour j but unhappy, but only because v is not coloured with j, 
			newHappy++;
		}
	}
	return newHappy;
}

void populateCMatrix(GRAPH &G, vector<vector<int> > &C, SOL &S) {
	int i, v, j;
	//C[v][j] holds the resultant number of happy vertices if v were to be moved to colour j
	for (i = 0; i < G.freeList.size(); i++) {
		v = G.freeList[i];
		for (j = 0; j < G.k; j++) {
			C[v][j] = calculateCEntry(G, S, v, j);
		}
	}
}

void moveVertex(GRAPH &G, SOL &S, int v, int j, int newNumHappy, vector<vector<int> > &C, vector<vector<int> > &tabuList, 
	int tabuIterations, vector<int> &posInCol, double tabuPameter) {
	//Procedure for moving the vertex v in colour "col" to the new colour j
	int col = S.col[v], pos = posInCol[v];
	if (col == j) exitAndLog("Error, col = j is not possible in reassignVertex function\n");
	int i, u, l;
	//Update the solution. First remove v from col
	S.items[col][posInCol[v]] = S.items[col].back();
	posInCol[S.items[col].back()] = posInCol[v];
	S.items[col].pop_back();
	//And add it to colour j
	S.items[j].push_back(v);
	posInCol[v] = S.items[j].size() - 1;
	S.col[v] = j;
	//Now need to redetermine the status of v 
	if (allNeighboursHaveColourJ(G, S, v, j)) S.isHappy[v] = true;
	else S.isHappy[v] = false;
	//And redetermine the statuses of v's neighbours u
	for (i = 0; i < G.adjList[v].size(); i++) {
		u = G.adjList[v][i];
		if (S.isHappy[u]) S.isHappy[u] = false;
		else {
			if (S.col[u] == j && allNeighboursHaveColourJ(G, S, u, j)) S.isHappy[u] = true;
			else S.isHappy[u] = false;
		}
	}
	//Now we update the update the score of the solution.
	S.numHappy = newNumHappy;
	//Finally, we need to update the C matrix for the next iteration of the algorithm. This involves updating the rows of all free vertices within distance two of v. First update v's row
	for (l = 0; l < G.k; l++) C[v][l] = calculateCEntry(G, S, v, l);
	//Now update the rows of the neighbours u of v
	for (i = 0; i < G.adjList[v].size(); i++) {
		u = G.adjList[v][i];
		if (G.precol[u] == -1) {
			for (l = 0; l < G.k; l++) C[u][l] = calculateCEntry(G, S, u, l);
		}
	}
	//and the rows of vertices u that are distance two from v
	for (i = 0; i < G.distTwoList[v].size(); i++) {
		u = G.distTwoList[v][i];
		if (G.precol[u] == -1) {
			for (l = 0; l < G.k; l++) C[u][l] = calculateCEntry(G, S, u, l);
		}
	}

	//and update the tabu matrix (v has left col, and cannot go back there for a while)
	tabuList[v][col] = tabuIterations + getTabuTenure(G, S, tabuPameter);
}

INFO tabuSearch(GRAPH &G, SOL &S, clock_t runStart, clock_t TL, double tabuPameter) {
	int i, v, j, bestv, bestj, bestMoveScore, numBest, newScore, tabuTenure = 0, tabuIterations = 0;
	INFO R;
	R.timeOfBest = (clock() - runStart) / double(CLOCKS_PER_SEC);
	R.itOfBest = 0;
	
	//keep a record of every vertex's position in its colour class
	vector<int> posInCol(G.n, -1);
	for (i = 0; i < G.k; i++) for (j = 0; j < S.items[i].size(); j++) posInCol[S.items[i][j]] = j;

	SOL SBest = S;
	if(verbosity >= 1) cout << "\n0)\tStart Score = " << S.numHappy << "/" << G.UB << "/" << G.n << endl;
	if (SBest.numHappy == G.UB) return R;

	//Set up tabu matrix and C matrix. Rows in C are only relevant for free vertices
	vector<vector<int> > tabuList(G.n, vector<int>(G.k, 0));
	vector<vector<int> > C(G.n, vector<int>(G.k, 0));
	populateCMatrix(G, C, S);
	
	while (clock() < TL && SBest.numHappy < G.UB) {
		
		bestMoveScore = 0;
		newScore = 0;
		numBest = 0;
		tabuIterations++;

		for (i = 0; i < G.freeList.size(); i++) {
			v = G.freeList[i];
			if (G.deg[v] > 0) {
				//evaluate only unhappy vertex moves (free&unhappy * k)
				if (!S.isHappy[v]) {
					for (j = 0; j < G.k; j++) {
						if (j != S.col[v]) {
							newScore = S.numHappy + C[v][j];
							if (newScore >= bestMoveScore) {
								if (newScore > bestMoveScore) numBest = 0;
								// Only consider the move if it is non-tabu or leads to a new very best solution seen globally.
								if (tabuList[v][j] < tabuIterations || (newScore > SBest.numHappy)) {
									if (rand() % (numBest + 1) == 0) {
										//Save the move
										bestv = v;
										bestj = j;
										bestMoveScore = newScore;
									}
									numBest++;
								}
							}
						}
					}
				}		
			}
		}
		//If no non-tabu moves have been found, take any random move (any free vertex to any different colour)
		if (bestMoveScore == 0) {
			if (verbosity >= 2) cout << "* " << endl;
			bestv = G.freeList[rand() % G.freeList.size()];
			do {
				bestj = rand() % G.k;
			} while (bestj == S.col[bestv]);
			bestMoveScore = S.numHappy + C[bestv][bestj];
		}

		//We have found an improving move, so do it and update the tabu tenure
		moveVertex(G, S, bestv, bestj, bestMoveScore, C, tabuList, tabuIterations, posInCol, tabuPameter);
		if (verbosity >= 2) cout << setw(8) << tabuIterations << ") Moved  vertex " << setw(5) << bestv << " to col " << setw(4) << bestj << ".\tScore = " << bestMoveScore << "/" << G.UB << "/" << G.n << endl;
		
		// check: have we a new globally best solution?
		if (S.numHappy > SBest.numHappy) {
			SBest = S;
			R.itOfBest = tabuIterations;
			R.timeOfBest = (clock() - runStart) / double(CLOCKS_PER_SEC);
			if (verbosity >= 1) cout << tabuIterations << ")\tBest so far = " << SBest.numHappy << ". UB = " << G.UB << endl;
			if (SBest.numHappy == G.UB) break;
		}
	
	}
	swap(S, SBest);
	R.numSecs = (clock() - runStart) / double(CLOCKS_PER_SEC);
	R.numIts = tabuIterations;
	if (verbosity >= 1) {
		cout << tabuIterations << ") End Score = " << S.numHappy << "/" << G.UB << "/" << G.n;
		if (S.numHappy == G.UB) cout << " Optimal (upper bound) determined\n;";
		else cout << "\n";
	}
	return R;
}
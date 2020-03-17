#include "bounds.h"

void invert(vector<int> &A) {
	if (A.empty()) return;
	int i = 0, j = A.size() - 1;
	while (i < j) {
		swap(A[i], A[j]);
		i++;
		j--;
	}
}

void removePath(vector <int> &path, GRAPH &H) {
	//Go through each internal vertex w on the path and isolate w by removing all incident edges
	int i, j, w;
	for (i = 1; i < path.size() - 1; i++) {
		w = path[i];
		for (j = 0; j < H.n; j++) {
			H.adj[w][j] = false;
			H.adj[j][w] = false;
		}
	}
}

void getShortestUnhappyPath(int s, GRAPH &H, vector<int> &path) {
	//A modified version of BFS that starts at s (which is precoloured to sCol). If we encounter a vertex precoloured to sCol,
	//we do not consider it. We explore until we find a precoloured vertex that is not coloured with 
	//sCol. We then exit immediately. This gives us the shortest unhappy starting at s. The adj list is used here, 
	//but we also need to check adjacency with the matrix because the adj matrix is being modified by the calling function. 
	//"path" holds the output: a path from the source to the identified vertex. If no path is found, "path" is empty
	int u, v, i, sCol = H.precol[s];
	if (sCol < 0) exitAndLog("Error: getShortestUnhappyPath must be called using a precoloured vertex\n");
	bool targetFnd = false;

	//Set up the colours array, the predecessors array, and the queue
	path.clear();
	vector<int> col(H.n, WHITE);
	vector<int> pred(H.n, NIL);
	deque<int> Q;

	//Start BFS
	col[s] = GREY;
	pred[s] = NIL;
	Q.push_back(s);
	while (!Q.empty() && !targetFnd) {
		u = Q.front();
		for (i = 0; i < H.adjList[u].size(); i++) {
			v = H.adjList[u][i];
			if (H.adj[u][v] && H.precol[v] != sCol) {
				if (col[v] == WHITE) {
					//Have discovered a new unvisited vertex v from u
					col[v] = GREY;
					pred[v] = u;
					Q.push_back(v);
					if (H.precol[v] != -1) {
						//This vertex is precoloured, but not with sCol, so end.
						targetFnd = true;
						goto makePath;
					}
				}
			}
		}
		Q.pop_front();
		col[u] = BLACK;
	}

makePath:
	if (targetFnd) {
		//Construct the path using the predecessor array
		int current = v;
		while (current != s) {
			path.push_back(current);
			current = pred[current];
		}
		path.push_back(current);
		invert(path);
	}
}

int getUpperBound(GRAPH &G) {
	if (G.n == 1) return 1;
	int i, u, v, minLen, minV, unhappyCnt = 0;
	vector<int> path;
	vector<bool> counted(G.n, false);

	//Make a copy of G called H and work with this
	GRAPH H = G;

	//First, go through all pairs of adjacent diff precoloured vertices in H and remove their links.
	for (u = 0; u < H.n - 1; u++) {
		if (H.precol[u] != -1) {
			for (v = u + 1; v < H.n; v++) {
				if (H.precol[v] != -1) {
					if (H.precol[u] == H.precol[v] && H.adj[u][v]) {
						//Optional: u and v are adjacent and precoloured to the same colour, so we can delete the edge for now
						H.adj[u][v] = false;
						H.adj[v][u] = false;
					}
					else if (H.precol[u] != H.precol[v] && H.adj[u][v]) {
						//u and v are adjacent and precoloured to different colours. We can delete the edge
						if (!counted[u] && !counted[v]) {
							unhappyCnt += 2;
							counted[u] = true;
							counted[v] = true;
						}
						else if (!counted[u]) {
							unhappyCnt++;
							counted[u] = true;
						}
						else if (!counted[v]) {
							unhappyCnt++;
							counted[v] = true;
						}
						//Else we do nothing, as both u and v have already been counted as being unhappy
						H.adj[u][v] = false;
						H.adj[v][u] = false;
					}
				}
			}
		}
	}

	//The adjaceny matrix of H is now a copy of G, except that no differently precoloured vertices are adjacent to each other.
	//Hence any paths between two precoloured vertices must have at least one intermediate uncoloured vertex (and therefore >= 2 edges)
	while (unhappyCnt < G.n) {
		minLen = INFTY;
		minV = -1;
		//Go through all possible paths between two precoloured nodes and find the shortest one
		for (i = 0; i < H.precolList.size(); i++) {
			v = H.precolList[i];
			//Get shortest path from v to another precoloured vertex u. Paths must contain at least three vertices due to the steps above
			getShortestUnhappyPath(v, H, path);
			if (!path.empty()) {
				if (path.size() < minLen) {
					minLen = path.size();
					minV = v;
				}
			}
		}
		if (minLen == INFTY) {
			//No path exists. So we can finish.
			break;
		}
		else {
			//Have IDd a minimal-length path with different colour terminals. Delete the path and update the count
			v = minV;
			getShortestUnhappyPath(v, H, path);
			u = path.back();
			if (!counted[v] && !counted[u]) {
				//Add 2 beacuse neither terminal has been counted yet
				unhappyCnt += 2;
			}
			else {
				//Add one because at least one terminal has been counted
				unhappyCnt++;
			}
			counted[v] = true;
			counted[u] = true;
			//Delete all intermiediate vertices (and incident edges) on this path
			removePath(path, H);
		}
	}
	return G.n - unhappyCnt;
}

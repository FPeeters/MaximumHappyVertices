#include "IO.h"
#include <cstring>
#include <iostream>
#include <string>

extern int verbosity;

//These three arrays are cycled through to generate new colours for each set of vertices in the solution. After 56 colours, they are repeated
int REDVAL[56] = { 255,0,0,255,255,0,0,128,0,0,128,128,0,128,192,0,0,192,192,0,192,64,0,0,64,64,0,64,32,0,0,32,32,0,32,96,0,0,96,96,0,96,160,0,0,160,160,0,160,224,0,0,224,224,0,224 };
int GREENVAL[56] = { 0,255,0,255,0,255,0,0,128,0,128,0,128,128,0,192,0,192,0,192,192,0,64,0,64,0,64,64,0,32,0,32,0,32,32,0,96,0,96,0,96,96,0,160,0,160,0,160,160,0,224,0,224,0,224,224 };
int BLUEVAL[56] = { 0,0,255,0,255,255,0,0,0,128,0,128,128,128,0,0,192,0,192,192,192,0,0,64,0,64,64,64,0,0,32,0,32,32,32,0,0,96,0,96,96,96,0,0,160,0,160,160,160,0,0,224,0,224,224,224 };

//-------------------------------------------------------------------------------------
void prettyPrintSol(GRAPH &G, SOL &S, ostream &outStream)
{
	int i, j, k = S.items.size(), cnt = 0;
	outStream << "\n\n";
	for (i = 0; i < k; i++) {
		outStream << "Col" << setw(4) << i << " = { ";
		for (j = 0; j < S.items[i].size(); j++) {
			if (G.precol[S.items[i][j]] != -1) {
				outStream << "[";
				if (!S.isHappy[S.items[i][j]])
					outStream << "#" << S.items[i][j];
				else 
					outStream << S.items[i][j];
				outStream << "] ";
			}
			else {
				if (!S.isHappy[S.items[i][j]])
					outStream << "#" << S.items[i][j] << " ";
				else
					outStream << S.items[i][j] << " ";
			}
		}
		outStream << "} \n";
		cnt += S.items[i].size();
	}
	outStream << "\n|V| = " << cnt << ", k = " << G.k << ", numHappy = " << S.numHappy << endl;
}

void printRunDetails(GRAPH &G, SOL &S, double duration, ostream &outStream)
{
	double gap = 0.0;
	if (G.UB != 0) gap = (G.UB - S.numHappy) / double(G.UB);
	outStream << "\nNum vertices                 = " << G.n << endl;
	outStream << "k                            = " << G.k << endl;
	outStream << "Num precoloured              = " << G.precolList.size() << endl;
	outStream << "Min degree                   = " << G.minDeg << endl;
	outStream << "Max degree                   = " << G.maxDeg << endl;
	outStream << "Total time                   = " << duration << endl;
	outStream << "Happy vertices               = " << S.numHappy << endl;
	outStream << "Unhappy vertices             = " << G.n - S.numHappy << endl;
	outStream << "Lower Bound                  = " << S.numHappy << endl;
	outStream << "Upper bound                  = " << G.UB << endl;
	outStream << "Gap                          = " << gap << endl;
}

void printSlnToFile(GRAPH &G, string &filename, SOL &S)
{
	//Prints the solution in an easy machine readable format to sln_*. Like the input instance, colours and
	//vertices are indexed from one to work with the checker program
	ofstream outStream;
	int i;
	string outfilename = "sln_" + filename;
	outStream.open(outfilename);
	if (outStream.fail()) {
		exitAndLog("Error opening output sln file. Exiting...\n");
	}
	outStream << S.numHappy << endl;
	for (i = 0; i < G.n; i++) {
		outStream << S.col[i] + 1 << endl;
	}
	outStream.close();
}

void printRunDetailsToFileAndConsole(GRAPH &G, string &filename, SOL &S, double duration) {
	ofstream outStream;
	string outfilename = "out_" + filename;
	outStream.open(outfilename);
	if (outStream.fail()) {
	  cout << endl << outfilename << endl;
		exitAndLog("Error opening output out file. Exiting...\n");
	}
	printRunDetails(G, S, duration, outStream);
	printRunDetails(G, S, duration, cout);
	outStream.close();
}

void printVisualisationFile(GRAPH &G, string &filename, SOL &S)
{
	//Prints the solution in a format for my excel graph visualiser to vis_*. 
	ofstream outStream;
	int i, j, col;
	string outfilename = "vis_" + filename;
	outStream.open(outfilename);
	if (outStream.fail()) exitAndLog("Error opening output sln file. Exiting...\n");

	//Write out all of the edges
	outStream << G.n << endl << G.m << endl << "Edgestart\tEnd\tRed\tGreen\tBlue\tThickness\tArrow" << endl;
	for (i = 0; i < G.n - 1; i++) {
		for (j = i + 1; j < G.n; j++) {
			if (G.adj[i][j]) {
				outStream << i + 1 << "\t" << j + 1 << "\t0\t0\t0\t0.25\tn" << endl;
			}
		}
	}

	outStream << "VertexLabel\tRed\tGreen\tBlue\tThickness\tShowLabel" << endl;
	for (i = 0; i < G.n; i++) {
		col = S.col[i];
		if (col >= 0) {
			outStream << "/" << "\t" << REDVAL[col % 56] << "\t" << GREENVAL[col % 56] << "\t" << BLUEVAL[col % 56] << "\t";
		}
		else {
			outStream << "/" << "\t-1\t-1\t-1\t";
		}
		if (G.precol[i] < 0) outStream << "0.25\t";
		else  outStream << "2\t";
		if (S.isHappy[i]) outStream << "n\n";
		else outStream << "y\n";
	}
		
	outStream.close();
}

void readInputFile(GRAPH &G, string &filename)
{
	//Reads a DIMACS format file and creates the corresponding degree array and adjacency matrix
	ifstream inStream;
	inStream.open(filename);
	if (inStream.fail()) {
		exitAndLog("Error opening input file. Exiting...\n");
	}
	char c, str[1000];
	int line = 0, i, numEdges = 0, numNodes = 0, numCols = 0, node, colour, edgeCnt = 0, sSetSize = 0;
	vector<vector<bool> > adjacent;
	vector<int> precol;
	vector<bool> colUsed;
	try {
		while (!inStream.eof()) {
			line++;
			inStream.get(c);
			if (inStream.eof()) break;
			switch (c) {
			case 'p':
				inStream.get(c);
				inStream.getline(str, 999, ' ');
				if (strcmp(str, "edge") && strcmp(str, "edges")) {
					exitAndLog("Error reading 'p' line in input file: no 'edge' keyword found. Exiting...\n");
				}
				inStream >> numNodes >> numEdges >> numCols;
				//Set up the 2d adjacency matrix and the precol vector
				adjacent.resize(numNodes, vector<bool>(numNodes, false));
				precol.resize(numNodes, -1);
				colUsed.resize(numCols, false);
				break;
			case 'e':
				int node1, node2;
				inStream >> node1 >> node2;
				if (node1 < 1 || node1 > numNodes || node2 < 1 || node2 > numNodes) {
					exitAndLog("Error. Node number out of range in input file. Exiting...\n");
				}
				node1--;
				node2--;
				if (!adjacent[node1][node2]) {
					edgeCnt++;
				}
				else {
					exitAndLog("Error. Edge defined more thn once in input file. Exiting...\n");
				}
				adjacent[node1][node2] = true;
				adjacent[node2][node1] = true;
				break;
			case 'n':
				inStream >> node >> colour;
				if (node < 1 || node > numNodes || colour < 1 || colour > numCols) {
					exitAndLog("Error. Node or colour out of range in line beginning with 'n' in input file. Exiting...\n");
				}
				node--;
				colour--;
				if (precol[node] != -1) {
					exitAndLog("Error. Precolouring defined more than once in input file. Exiting...\n");
				}
				precol[node] = colour;
				colUsed[colour] = true;
				break;
			case 'c':
				inStream.putback('c');
				inStream.get(str, 999, '\n');
				break;
			default:
				exitAndLog("Error: Unknown line code in input file. Exiting...\n");
			}
			inStream.get(); // Kill the newline;
		}
	}
	catch (...) {
		exitAndLog("Undefined error in input file. It is probably corrupted. Exiting...\n");
	}
	inStream.close();

	if (edgeCnt != numEdges) {
		exitAndLog("Error: Number of edges stated at the top of the input file is not equal to the number found in the file. Exiting...\n");
	}

	//Check that all colours are used in the preassignments
	for (i = 0; i < numCols; i++) {
		if (colUsed[i] == false) {
			exitAndLog("Error: A colour is not being used in the precolourings given in input file. Exiting...\n");
		}
	}
	makeGraphFromAdjacencyMatrix(G, adjacent, precol, numCols);
}

void checkSlnValidity(GRAPH &G, SOL &S, string &str) {
	//Check it's colours and precolours
	int i, j;
	for (i = 0; i < G.n; i++) {
		if (S.col[i] < 0 || S.col[i] >= G.k) str += "Error. Vertex-" + to_string(i) + " contains and invalid colour (" + to_string(S.col[i]) + ").\n";
		if (G.precol[i] != -1) {
			//Vertex i is precoloured in the graph
			if (S.col[i] != G.precol[i]) str += "Error. Vertex-" + to_string(i) + " is assigned to colour-" + to_string(S.col[i]) + " in the solution but should actually be preassigned to colour-" + to_string(G.precol[i]) + ".\n";
		}
	}
	//Check that S.items and S.col match
	for (i = 0; i < S.items.size(); i++) {
		for (j = 0; j < S.items[i].size(); j++) {
			if (S.col[S.items[i][j]] != i) str += "Error: S.items and S.cols do not match.\n";
		}
	}
	//Now check claimed score and consistency of isHappy array
	int v, u, claimedScore = S.numHappy;
	S.numHappy = G.n;
	vector<bool> claimedIsHappy = S.isHappy;
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
	if (claimedScore != S.numHappy) str += "Error. The claimed score of " + to_string(claimedScore) + " happy vertices is not equal to the actual score of " + to_string(S.numHappy) + ".\n";
	for (i = 0; i < G.n; i++) {
		if (claimedIsHappy[i] != S.isHappy[i]) str += "Error. The claimed happyness of vertex " + to_string(i) + " is not equal to the actual happyness of " + to_string(S.isHappy[i]) + ".\n";
	}
}

void checkSln(GRAPH &G, SOL &S) {
	//Does a series of checks on a solution to check if it is vliid. I.e. the data structures/scores etc. all match up.
	bool OK = true;
	vector<int> U;
	string str = "";
	//Check the solution size
	if (G.n != S.col.size()) str += "Error The graph and the solution do not contain the same number of vertices. Exiting.\n";
	//Check that the claimed score is valid
	if (S.numHappy < 0 || S.numHappy > G.n) str += "Error. The claimed score of " + to_string(S.numHappy) + " is invalid. (There are " + to_string(G.n) + " vertices in the graph).\n";
	checkSlnValidity(G, S, str);
	if (str != "") {
		cout << str << "Exiting...\n";
		exit(1);
	}
}

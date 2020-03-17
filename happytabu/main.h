#ifndef MAIN_H
#define MAIN_H

//Thiruvady, D., R. Lewis and K. Morgan (2020) 'Tackling the Maximum Happy Vertices Problem in Large Networks'. 4OR, doi: 10.1007/s10288-020-00431-4.

#include <fstream>
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>
#include <climits>
#include <string.h>

#pragma warning(disable:4267)
#pragma warning(disable:4996)

using namespace std;

struct GRAPH
{
	int n;								//num nodes
	int m;								//num edges
	int k;								//num colours
	vector<vector<bool> > adj;			//adjacency matrix
	vector<vector<int> > adjList;		//adjacency list
	vector<int> deg;					//degree of each node
	vector<int> precol;					//colour that v is precoloured to (-1 if v is not precoloured)
	vector<int> precolList;				//list of all precoloured vertices
	vector<int> freeList;				//list of all free vertices, in order
	vector<int> numPreass;				//number of vertices preassigned to each colour
	int minDeg;							//maximum degree
	int maxDeg;							//minimum degree
	int UB;								//Upper bound of the graph / problem instance (determined using the Lewis-Thiruvady-Morgan method)
	vector<vector<int> > distTwoList;	//for each vertex, a list of vertices whose distances are exactly two (edges) from v
};

struct SOL
{
	vector<vector<int> > items;			//contains the vertices asssigned to each colour. Preassignmnets are in the leftmost positions
	vector<int> col;					//the colour of each vertex
	vector<bool> isHappy;				//true if vertex is happy, else false
	int numHappy;						//number of happy vertices
};

struct INFO
{
	double timeOfBest = 0.0;			//Tells us the number of seconds after the start of the algorithm the best solution was found
	int itOfBest = 0;					//Tells us the number of iterations that it took to find the best solution in the run
	int numIts = 0;						//Tells us the total number of iterations tabuSearch was run for
	double numSecs = 0.0;				//Tells us the total time tabuSearch was run for
};

void exitAndLog(string msg);

// CONSTANTS USED FOR DFS ALG
const int WHITE = 0;
const int GREY = 1;
const int BLACK = 2;
const int NIL = INT_MIN;
const int INFTY = INT_MAX;

#include "initsol.h"
#include "IO.h"
#include "bounds.h"
#include "tabu.h"
#include "graph.h"
#include "addprecolourings.h"

#endif //MAIN_H


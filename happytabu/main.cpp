#include "main.h"

//Thiruvady, D., R. Lewis and K. Morgan (2020) 'Tackling the Maximum Happy Vertices Problem in Large Networks'. 4OR, doi: 10.1007/s10288-020-00431-4.

//Global variable for controlling level of output
int verbosity;

void exitAndLog(string msg) {
	//Outputs the msg to the screen and log file and then exits
	ofstream resultsLog("log.txt", ios::app);
	resultsLog << msg;
	resultsLog.close();
	cout << msg;
	exit(1);
}

void usage() {
	cout << "Algorithm for the Happy Colouring Problem\n\n"
		<< "USAGE:\n"
		<< "-----------------\n"
		<< "<InputFile>     (Required. File must be in DIMACS format)\n"
		<< "-r <int>        (Random seed. DEFAULT = 1)\n"
		<< "-t <int>        (Time limit (secs). DEFAULT = 5)\n"
		<< "-T <double>     (tabuParameter. DEFAULT = 2.0)\n"
		<< "-v              (Verbosity. Repeat this for more output)\n"
		<< "-----------------\n";
	exit(0);
}

int main(int argc, char **argv) {

	if (argc <= 1) {
		usage();
	}

	//PARAMETERS----------------------------------------------------------------------------------
	//VARIABLES (DEFAULT VALS)
	int i, seed = 1, timeLimit = 5;
	verbosity = 0;
	double duration, tabuParameter = 2.0;
	string filename;
	GRAPH G;
	SOL S;

	//Parse the input parameters (command line) and the input file
	try {
		for (i = 1; i < argc; i++) {
			if (strcmp("-r", argv[i]) == 0) {
				seed = atoi(argv[++i]);
			}
			else if (strcmp("-t", argv[i]) == 0) {
				timeLimit = atoi(argv[++i]);
			}
			else if (strcmp("-v", argv[i]) == 0) {
				verbosity++;
			}
			else if (strcmp("-T", argv[i]) == 0) {
				tabuParameter = atof(argv[++i]);
			}
			else {
				cout << "Tabu Algorithm for the MHV Problem using <" << argv[i] << ">\n\n";
				filename = argv[i];
				readInputFile(G, filename);
			}
		}
	}
	catch (...) {
		cout << "Error with input arguments. Ending...\n";
		usage();
	}

	//Set seed and start the clock
	srand(seed);
	clock_t runStart = clock();
	clock_t runEnd = runStart + (timeLimit * CLOCKS_PER_SEC);

	//Use various deductions to precolour additional vertices. Then calculate an upper bound on the number of happy vertices
	int startPrecols = G.precolList.size();
	addPrecolourings(G);
	G.UB = getUpperBound(G);

	//Now make an initial solution and do the tabu search optimisation
	makeInitSol(G, S);
	int startScore = S.numHappy;
	INFO R = tabuSearch(G, S, runStart, runEnd, tabuParameter);
	
	//Write some stuff about the run to the screen and to the output file and end
	duration = (clock() - runStart) / double(CLOCKS_PER_SEC);
	checkSln(G, S);
	if (verbosity >= 1) {
        prettyPrintSol(G, S, cout);
        printRunDetailsToFileAndConsole(G, filename, S, duration);
        printSlnToFile(G, filename, S);
	}
	
	cout << endl;
	string msg;
	msg = to_string(G.n) + "\t"
		+ to_string(G.m) + "\t"
		+ to_string(G.k) + "\t"
		+ to_string(tabuParameter) + "\t"
		+ to_string(startPrecols) + "\t"
		+ to_string(G.precolList.size()) + "\t"
		+ to_string(G.UB) + "\t"
		+ to_string(startScore) + "\t"
		+ to_string(S.numHappy) + "\t"
		+ to_string(timeLimit) + "\t"
		+ to_string(R.timeOfBest) + "\t"
		+ to_string(R.itOfBest) + "\t"
		+ to_string(R.numSecs) + "\t"
		+ to_string(R.numIts) + "\t"
		+ to_string(seed) + "\n";
	exitAndLog(msg);

	return 0;
}

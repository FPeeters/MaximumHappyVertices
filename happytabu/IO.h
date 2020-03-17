#ifndef IO_H
#define IO_H

#include "main.h"

void prettyPrintSol(GRAPH &G, SOL &S, ostream &outStream);
void readInputFile(GRAPH &G, string &filename);
void printSlnToFile(GRAPH &G, string &filename, SOL &S);
void printRunDetailsToFileAndConsole(GRAPH &G, string &filename, SOL &S, double duration);
void printVisualisationFile(GRAPH &G, string &filename, SOL &S);
void checkSln(GRAPH &G, SOL &S);

#endif //IO_H

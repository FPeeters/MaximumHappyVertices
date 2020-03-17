----------------------------------------------------------------------------------------------
Tabu Search Algorithm for the Maximum Happy Vertices Problem: User Guide
------------------------------------------------------------------------

This document contains descriptions on how to compile the tabu search algorithm for the MHV problem as described by Thiruvady, Lewis and Morgan in the following publication.

Thiruvady, D., R. Lewis and K. Morgan (2020) 'Tackling the Maximum Happy Vertices Problem in Large Networks'. 4OR, doi: 10.1007/s10288-020-00431-4.

This program has been written in C++ (V11). 



----------------------------------------------------------------------------------------------
Compilation in Microsoft Visual Studio
--------------------------------------

To compile and execute using Microsoft Visual Studio, the following steps can be taken:

1.	Open Visual Studio and click File, then New, and then Project from Existing Code.
2.	In the dialog box, select Visual C++ and click Next.
3.	Select the subdirectory containing these files and click Next.
4.	Finally, select Console Application Project for the project type, and then click Finish.

The source code can then be viewed and executed from the Visual Studio application. Release mode should be used during compilation to make the program execute at maximum speed.



----------------------------------------------------------------------------------------------
Compilation with g++
--------------------

To compile the source code in Linux, please use the included makefile.



----------------------------------------------------------------------------------------------
Using the Program
-----------------

Once generated, the executable file should be run from the command line. If the program is called with no arguments, the following usage information will be printed to the screen:

***
Algorithm for the Happy Colouring Problem
Usage:
<InputFile>     (Required. File must be in DIMACS format)
-r <int>        (Random seed. DEFAULT = 1)
-t <int>        (Time limit (secs). DEFAULT = 5)
-T <double>     (tabuParameter. DEFAULT = 2.0)
-v              (Verbosity. Repeat this for more output)
***

This provides the user with information needed to produce valid commands. Here are some example commands for running the program.

>> happy graph.txt

This runs the tabu search algorithm on the problem instance "graph.txt" using the default parameters stated above and producing minimal output. The file "graph.txt" is included with this resource. 

>> happytabu.exe graph.txt -t 20 -r 123 -v

This runs the tabu search algorithm on "graph.txt" for 20 seconds of CPU time using the seed 123. The -v argument asks for additional output to the screen. 

On termination of the program, the solution is written to the screen along with various run details. The same information is also written to the file out_*, where * is the name of the input file. In addition, the solution produced by the algorithm is written to the file sln_*. The first line of this file contains the number of happy vertices. The remaining lines then contain the colour of each vertex in order, using colour labels 1 to k. Finally, after each execution, details of the run are also appended to the file  "log.txt". The information is written to a single line, with the following values separated by tabs:

Number of vertices
Number of edges
Number of colours
Tabu parameter used
NUmber of precoloured vertices at the start
Number of precoloured vertices after execution of Add-Precol routine
Generated Upper Bound
Number of Happy vertices in initial solution
Number of Happy vertices at termination
Time limit
Time at which the best observed solution was found
Iteration at which the best observed solution was found
Actual length of the run
Number of iterations carried out during the run
Random seed used. 



----------------------------------------------------------------------------------------------
Input Format
------------

Problem instances for this algorithm should be in the DIMACS format. We recommend using the problem instance generator available at http://rhydlewis.eu/resources/happyGen.zip.

For ease of reading, the vertices within the file are labeled in descending order of degree. An input file contains the following information. 

•	Lines beginning with a c contain comments and can be ignored;
•	The single line beginning with p edge contains, respectfully, the number of vertices, the number of edges, and the number of vertices preassigned to a colour;
•	Lines beginning with e state the graph adjacencies (i.e., the endpoints of each edge);
•	Lines beginning with n state the precolourings by giving, respectively, the vertex number and its colour.

For reference, an example graph, graph.txt, is included with this resource.



----------------------------------------------------------------------------------------------
Copyright notice
----------------

Redistribution and use in source and binary forms, with or without modification, of the code associated with this document are permitted provided that citations are made to the publication mentioned at the start of this document. Neither the name of the University nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. This software is provided by the contributors “as is”' and any express or implied warranties, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose are disclaimed. In no event shall the contributors be liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, procurement of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on any theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out of the use of this software, even if advised of the possibility of such damage. This software is supplied without any support services. 

Please direct any queries/comments to Rhyd Lewis: web: www.rhydLewis.eu, email: LewisR9@cf.ac.uk

R. Lewis (Last updated Monday, 21 May 2019)

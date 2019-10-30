#include <ilconcert/iloenv.h>
#include <ilconcert/ilomodel.h>
#include <ilcplex/ilocplexi.h>
#include "ExactSolver.h"

unsigned int solveExact(Graph &graph) {
    IloEnv env;
    IloModel model(env);

    IloNumVarArray xArr(env);
    IloBoolVarArray yArr(env);

    IloExpr objExpr(env);

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        const IloNumVar x(env, 1, graph.getNbColors(), ILOINT);
        xArr.add(x);

        if (graph.isPreColored(node))
            model.add(x == graph.getColor(node));

        const IloBoolVar y(env);
        yArr.add(y);
        objExpr += y;
    }

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node)
        for (auto adj: graph.getEdges(node))
            model.add(yArr[node] >= IloAbs(xArr[node] - xArr[adj]) / graph.getNbColors());


    model.add(IloMinimize(env, objExpr));

    IloCplex cplex(model);
    // cplex.exportModel("model.lp");
    cplex.setParam(IloCplex::EpRHS, 1e-9);
    cplex.solve();

    std::cout << std::endl << "Status of the found solution: " << cplex.getStatus() << std::endl;

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node)
        if (!graph.isPreColored(node))
            graph.color(node, cplex.getValue(xArr[node]));

    env.end();
    return graph.getHappyVertices();
}

#include <ilconcert/iloenv.h>
#include <ilconcert/ilomodel.h>
#include <ilcplex/ilocplexi.h>
#include "ExactSolver.h"

void
checkConstraints(const Graph &graph, const IloIntVarArray &xArr, const IloBoolVarArray &yArr, const IloCplex &cplex) {
    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        IloNum nodeColor = cplex.getValue(xArr[node]);
        auto col = (unsigned int) round(cplex.getValue(xArr[node]));
        auto realCol = (unsigned int) cplex.getValue(xArr[node]);
        if (col != realCol)
            std::cout << "Conversion error for node " << node << std::endl;

        for (auto adj: graph.getEdges(node)) {
            IloNum adjColor = cplex.getValue(xArr[adj]);
            if (abs(nodeColor - adjColor) > 1e-9 && abs(cplex.getValue(yArr[node]) - 1) > 1e-9) {

                std::cout << "Violated edge contraint " << node << " -> " << adj;
                std::cout << " | " << nodeColor << " " << adjColor << " " << cplex.getValue(yArr[node]) << std::endl;
            }
        }
    }

    std::cout << "Objective value: " << graph.getNbNodes() - cplex.getObjValue() << std::endl;
}

unsigned int solveExact(Graph &graph, const config &config) {
    IloEnv env;
    IloModel model(env);

    IloIntVarArray xArr(env);
    IloBoolVarArray yArr(env);

    IloExpr objExpr(env);

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        const IloIntVar x(env, 1, graph.getNbColors(), ("Color" + std::to_string(node)).c_str());
        xArr.add(x);

        if (graph.isPreColored(node))
            model.add(x == graph.getColor(node));

        const IloBoolVar y(env, ("Unhappy" + std::to_string(node)).c_str());
        yArr.add(y);
        objExpr += y;
    }

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node)
        for (auto adj: graph.getEdges(node))
            model.add(yArr[node] >= IloAbs(xArr[node] - xArr[adj]) / graph.getNbColors());

    model.add(IloMinimize(env, objExpr));

    IloCplex cplex(model);
    if (config.timeLimit != -1)
        cplex.setParam(IloCplex::Param::TimeLimit, config.timeLimit);
    cplex.setParam(IloCplex::Param::Threads, config.threads);
    cplex.solve();

    std::cout << std::endl << "Status of the found solution: " << cplex.getStatus() << std::endl << std::endl;

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node)
        if (!graph.isPreColored(node))
            try {
                graph.color(node, (unsigned int) round(cplex.getValue(xArr[node])));
            } catch (IloCplex::NotExtractedException &e) {
                graph.color(node, 1);
            }

    unsigned int happy = graph.getNbNodes() - (unsigned int) round(cplex.getObjValue());
    env.end();
    return happy;
}


unsigned int solveExactAlt(Graph &graph) {

    return graph.getHappyVertices();
}

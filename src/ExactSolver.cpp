#include "ExactSolver.h"
#include <iostream>

#ifdef CPLEX_FOUND

#include <ilconcert/iloenv.h>
#include <ilconcert/ilomodel.h>
#include <ilcplex/ilocplexi.h>

unsigned int solveExactDefault(Graph &graph, const config &config) {
    std::cout << "Using default model" << std::endl;

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

    std::cout << std::endl << "Status of the found solution: " << cplex.getStatus() << std::endl;

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (!graph.isPreColored(node))
            try {
                graph.color(node, (unsigned int) round(cplex.getValue(xArr[node])));
            } catch (IloCplex::NotExtractedException &e) {
                graph.color(node, 1);
            }
    }

    unsigned int happy = graph.getNbNodes() - (unsigned int) round(cplex.getObjValue());
    std::cout << cplex.getMIPRelativeGap() << std::endl;
    env.end();
    return happy;
}

unsigned int solveExactAlt(Graph &graph, const config &config) {
    std::cout << "Using alternative model" << std::endl;

    IloEnv env;
    IloModel model(env);

    IloBoolVarArray xArr(env);
    IloBoolVarArray yArr(env);

    IloExpr objExpr(env);

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        IloExpr colorSum(env);
        for (unsigned int c = 0; c < graph.getNbColors(); ++c) {
            IloBoolVar x(env);
            xArr.add(x);

            colorSum += x;
        }

        model.add(colorSum == 1);

        if (graph.isPreColored(node))
            model.add(xArr[node * graph.getNbColors() + graph.getColor(node) - 1] == 1);

        IloBoolVar y(env);
        yArr.add(y);

        objExpr += y;
    }

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node)
        for (auto adj: graph.getEdges(node))
            for (unsigned int c = 0; c < graph.getNbColors(); ++c)
                model.add(yArr[node] >=
                          IloAbs(xArr[adj * graph.getNbColors() + c] - xArr[node * graph.getNbColors() + c]));

    model.add(IloMinimize(env, objExpr));

    IloCplex cplex(model);
    if (config.timeLimit != -1)
        cplex.setParam(IloCplex::Param::TimeLimit, config.timeLimit);
    cplex.setParam(IloCplex::Param::Threads, config.threads);

    cplex.solve();

    std::cout << std::endl << "Status of the found solution: " << cplex.getStatus() << std::endl << std::endl;

    for (unsigned int node = 0; node < graph.getNbNodes(); ++node) {
        if (!graph.isPreColored(node))
            try {
                for (unsigned int c = 0; c < graph.getNbColors(); ++c) {
                    if (cplex.getValue(xArr[node * graph.getNbColors() + c]) > 0) {
                        graph.color(node, c + 1);
                        break;
                    }
                }
            } catch (IloCplex::NotExtractedException &e) {
                graph.color(node, 1);
            }
    }

    unsigned int happy = graph.getNbNodes() - (unsigned int) round(cplex.getObjValue());
    std::cout << cplex.getMIPRelativeGap() << std::endl;
    env.end();
    return happy;
}

#endif

unsigned int solveExact(Graph &graph, const config &config) {
#ifdef CPLEX_FOUND
    if (config.altModel)
        return solveExactAlt(graph, config);
    else
        return solveExactDefault(graph, config);
#else
    throw std::runtime_error("Exact solver was not loaded during compiling");
#endif // CPLEX_FOUND
}

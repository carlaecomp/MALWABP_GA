/*
 * Sequencing.h
 *
 *  Created on: 27/05/2015
 *      Author: pamela
 */

#ifndef SEQUENCING_H_
#define SEQUENCING_H_

#include "../Constants.h"
#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

using namespace std;

typedef IloArray < IloNumVarArray > NumVarMatrix;

class Sequencing {
	int M, K, I, totalDemand; // I >= totalDemand (totalDemand = 500 - user parameter)
	vector<int> xh;
	double lb, ub; // fo = \sum y_ki
	double cycleTime;
	vector<int> dayDemand;         // M - demanda real
	vector< vector<double> > t_mk;
	vector< vector<double> > t_mk_uw; // t_mk para o utility worker
	vector< vector<double> > delta;
	vector< vector<double> > wo_mk;
	vector < vector < vector<double> > > malwabpTime; // tempo N x M X W
	vector< vector<int> > assignedTasks;
	vector<int> assignedWorker;

	IloEnv env;
    NumVarMatrix s;
    NumVarMatrix x;
    NumVarMatrix y;
    IloModel model;
    IloCplex cplex;

	void calculateLbUb();
	void calculateDelta();
	void calculateWOmk();
	void calculateTmk();
	void calculateTmkUw();

	void setXmi();

	void clearModel();
	void createVariables();
	void constructModel();
	IloExpr getObjectiveFunction();

	void printData();

public:
	Sequencing(int K, int M, int I, int totalDemand, double cycleTime, vector<int> dayDemand, vector<int> xh,
			vector < vector < vector<double> > >& malwabpTime, vector< vector<int> > assignedTasks, vector<int> assignedWorker);

	double solve();
};

#endif /* SEQUENCING_H_ */

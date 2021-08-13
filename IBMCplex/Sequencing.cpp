/*
 * Sequencing.cpp
 *
 *  Created on: 27/05/2015
 *      Author: Pamela Cortez
 */

#include "Sequencing.h"

Sequencing::Sequencing(int K, int M, int I, int totalDemand, double cycleTime,
		vector<int> dayDemand, vector<int> xh,
		vector < vector < vector<double> > >& malwabpTime,
		vector< vector<int> > assignedTasks, vector<int> assignedWorker)
{
	this->K = K;
	this->M = M;
	this->I = I;
	this->totalDemand = totalDemand;
	this->cycleTime = cycleTime;
	this->dayDemand = dayDemand;
	this->xh = xh;
	this->malwabpTime = malwabpTime;
	this->assignedTasks = assignedTasks;
	this->assignedWorker = assignedWorker;
}

void Sequencing::calculateLbUb() {
	int k, m;
	lb = ub = 0;

	for (k = 0; k < K; k++) {
		for (m = 0; m < M; m++) {
			if (t_mk[m][k] > cycleTime) { // && t_mk[m][k] < MAX/2 && t_mk_uw[m][k] > 0
				ub += ((t_mk[m][k] - cycleTime) / delta[m][k]) * dayDemand[m];

				if (t_mk[m][k] > (1+LK)*cycleTime)
					lb += ((t_mk[m][k] - (1+LK)*cycleTime) / delta[m][k]) * dayDemand[m];
			}
		}
	}
}

void Sequencing::calculateTmk() {
	int m, k;

	t_mk.clear();
	t_mk.resize(M);
	for (m = 0; m < M; m++) {
		t_mk[m].resize(K);

		for (k = 0; k < K; k++) {
			t_mk[m][k] = 0;

			for (vector<int>::iterator it = assignedTasks[k].begin(); it != assignedTasks[k].end(); ++it) {
				//// cout << "K: " << k << "M:" << m << "it" << *it << "assignedWorker[k]" << assignedWorker[k] << endl;
				t_mk[m][k] += malwabpTime[m][*it][assignedWorker[k]];
			}
		}
	}
}

void Sequencing::calculateTmkUw() {
	int k, m, w, min;

	t_mk_uw.clear();
	t_mk_uw.resize(M);
	for (m = 0; m < M; m++) {
		t_mk_uw[m].resize(K);

		for (k = 0; k < K; k++) {
			t_mk_uw[m][k] = 0;

			for (vector<int>::iterator it = assignedTasks[k].begin(); it != assignedTasks[k].end(); ++it) {
				min = MAX;
				for (w = 0; w < K; w++) {
					if (malwabpTime[m][*it][w] < min)
						min = malwabpTime[m][*it][w];
				}
				t_mk_uw[m][k] += min;
			}
		}
	}
}

void Sequencing::calculateDelta() {
	delta.clear();
	delta.resize(M);
	for (int m = 0; m < M; m++) {
		delta[m].resize(K);
		for (int k = 0; k < K; k++)
			delta[m][k] = t_mk[m][k]/t_mk_uw[m][k]; // ideia da "média"
	}
}

void Sequencing::calculateWOmk() {
	wo_mk.resize(M);
		for (int m = 0; m < M; m++)
			wo_mk[m].resize(K);

	for (int m = 0; m < M; m++) {
		for (int k = 0; k < K; k++) {
			if (t_mk[m][k] > cycleTime)
				wo_mk[m][k] = t_mk[m][k] < (1+LK)*cycleTime ? (t_mk[m][k] - cycleTime) : LK*cycleTime;
			else if (t_mk[m][k] < cycleTime)
				wo_mk[m][k] = (cycleTime - t_mk[m][k]) < LK*cycleTime ? (t_mk[m][k] - cycleTime) : -LK*cycleTime;
			else
				wo_mk[m][k] = 0;
		}
	}
}

IloExpr Sequencing::getObjectiveFunction() {
	IloExpr objective(env);
	for (int i = 0; i < I; i++) {
		for (int k = 0; k < K; k++) {
			objective += y[k][i];
		}
	}
	return objective;
}

void Sequencing::clearModel() {
	cplex.end();
	model.end();
	s.end(); x.end(); y.end();
	env.end();
}

void Sequencing::createVariables() {
	int i, m, k;
	const double l = (1+LK)*cycleTime;

	for(k = 0; k < K; k++) {
		y[k] = IloNumVarArray(env, I, 0, IloInfinity, ILOFLOAT);
		s[k] = IloNumVarArray(env, I+1, 0, l-cycleTime, ILOFLOAT);
		for (i = 0; i < I; i++) {
			s[k][i] = IloNumVar(env, 0, l-cycleTime, ILOFLOAT);
			y[k][i] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
		}
		s[k][I] = IloNumVar(env, 0, l-cycleTime, ILOFLOAT);
	}

	for(m = 0; m < M; m++) {
		x[m] = IloNumVarArray(env, I, 0, 1, ILOINT);
		for (i = 0; i < I; i++) {
			x[m][i] = IloNumVar(env, 0, 1, ILOINT);
		}
	}
}

void Sequencing::constructModel() {
	int i, m, k;
	const double l = (1+LK)*cycleTime;
	double max;
	vector<double> tmaxk(K);

	env = IloEnv();
	s = NumVarMatrix(env, K);
	x = NumVarMatrix(env, M);
	y = NumVarMatrix(env, K);

	model = IloModel(env);
	cplex = IloCplex(model);

	createVariables();

	for(k = 0; k < K; k++) {
		model.add(s[k][0] == 0);
		model.add(s[k][I] == 0);
	}

	for(m = 0; m < M; m++) {
		model.add(IloSum(x[m]) == dayDemand[m]);
	}

	for (k = 0; k < K; k++) {
		max = t_mk[0][k];
		for (m = 1; m < M; m++)
			if (max < t_mk[m][k])
				max = t_mk[m][k];
		tmaxk[k] = max;
	}

	IloExpr exprd(env);
	for (i = 0; i < I; i++) {
		exprd.clear();
		for (m = 0; m < M; m++)
			exprd += x[m][i];
		model.add(exprd == 1);

		for (k = 0; k < K; k++) {
			for (m = 0; m < M; m++) {
				if (t_mk_uw[m][k] > 0) {
					model.add(s[k][i] + t_mk[m][k]*x[m][i] - cycleTime - delta[m][k]*y[k][i] <= s[k][i+1]);
					model.add(s[k][i] + t_mk[m][k]*x[m][i] - delta[m][k]*y[k][i] <= l);
				}
			}
		}
	}
	exprd.end();

	IloExpr objective = getObjectiveFunction();
	model.add(IloMinimize(env, objective));
	objective.end();
}

double Sequencing::solve() {
	// cout << "Entrou no solve" << endl;
	double min;

	calculateTmk();
	calculateTmkUw();
	calculateDelta();
	calculateWOmk();
	constructModel();
	setXmi();
	cplex.setParam(IloCplex::Threads, 1);
	cplex.setOut(env.getNullStream());
	cplex.solve();

	if (cplex.getStatus() == IloAlgorithm::Infeasible || cplex.getStatus() == IloAlgorithm::Unknown
			|| cplex.getStatus() == IloAlgorithm::Error) {
		cplex.exportModel("sequencing.lp");
		return -1;
	}

	min = cplex.getObjValue();

	clearModel();
	return min;
}

void Sequencing::setXmi() {
	for (int m = 0; m < M; m++) {
		for (int i = 0; i < I; i++) {
			x[m][i].setBounds(0,0);
		}
	}
	for (int i = 0; i < I; i++) {

		if (xh[i] >= 0 && xh[i] < M) {
			x[xh[i]][i].setBounds(1,1);
		}
	}
}

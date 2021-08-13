/*
 * Individual.h
 *
 *  Created on: 21/06/2015
 *      Author: Carla Santana
 */

#ifndef GA_INDIVIDUAL_H_
#define GA_INDIVIDUAL_H_

#include <stddef.h>  // defines NULL
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>

using namespace std;

class Individual {

	vector<int> models;
	vector<int> workers;
	vector<int> tasks;
	vector<int> station;
	vector<double> timeStation;
	double cycleTime;
	double wo;
	bool infeasible=false;
	double fitness;
	double sumStationCycle=0;

public:
	Individual(vector<int> models, vector<int> workers, vector<int> tasks, vector<int> station, vector<double> timeStation, double wo);
	virtual ~Individual();
	vector<int> getModels();
	vector<int> getWorkers();
	vector<int> getTasks();
	vector<int> getStation();
	double getCycleTime();
	double getFitness();
	void printFitness();
	void printFitnesscerr();
	double getWo();
	vector<double> getTimeStation(){
		return timeStation;
	}
	void isInfeasible(int qtdInfac, int K){
		fitness += qtdInfac*(K*K);
		this->infeasible=true;
	}
	bool getInfeasible(){
		return this->infeasible;
	}
	void printAll();
	vector< vector< int > > getAssignedTasks();
	double getSumStationCycle(){
		return sumStationCycle;
	}

};

#endif /* GA_INDIVIDUAL_H_ */

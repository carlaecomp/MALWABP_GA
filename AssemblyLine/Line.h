/*
 * Line.h
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#ifndef LINE_H_
#define LINE_H_

#include "Station.h"
#include "Worker.h"
#include "Task.h"
#include "Model.h"
#include "../InputOutputFile/ReadInstance.h"
#include <stddef.h>  // defines NULL
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

class Line {
	static Line* instance; // singleton
	int N; // qtd tarefas
	int W; // qtd de trabalhadores
	int K; // qtd de estações ou maquinas
	int M; // qtd de modelos
	int totalDemand; // user parameter
	int planningHorizon;
	double cycleTime;
	vector<Station> stations;
	vector<Task*> unassignedTasks;
	vector<Worker> unassignedWorkers;
	vector<Model*> models;
	char* nameInstance;
	double alpha;
	int totalDemandAvg;
	vector<int> avgDayDemand;
	vector < vector < vector<double> > > malwabpTime;


	//antes:
	//Line();
	Line(int totalDemand, int planningHorizon, double alpha);
	Line(Line const&){};             // copy constructor is private
	//Line& operator=(Line const&){};  // assignment operator is private

	void immediateSuccessors();

public:
	const static double MAX = 2000000.0-100;
	const static double ZERO = 0.000001;
	const static double LK = 0.1;

	static Line* getInstance(int totalDemand, int planningHorizon, double alpha);
	virtual ~Line();

	/**
	 * Create the assembly line just once
	 *
	 * @param filename filename of the instance
	 * @return true only for the first call, false otherwise
	 *
	 */

	bool create(char* subdir, char* filename);

	Station getStation(int id);
	Model* getModel(int id);
	int getLastStationId();
	int getFirstStationId();
	vector<Task*> getTasks();
	void prepareTasksForGA();

	vector<Task*>::iterator getUnassignedTasksBeginIterator();
	vector<Task*>::iterator getUnassignedTasksEndIterator();


	void unassignAllTasks();

	void balance();
	void sequencing();

	void print();

	int getTotalDemandAvg() const {
		return totalDemandAvg;
	}

	int getTotalDemand() const {
		return totalDemand;
	}

	int getK() const {
		return K;
	}

	int getM() const {
		return M;
	}

	int getN() const {
		return N;
	}

	int getW() const {
		return W;
	}
	char * getNameInstance(){
		return nameInstance;
	}
	vector<int> getAvgDayDemand(){
		return avgDayDemand;
	}
	vector < vector < vector<double> > > getMalwabpTime(){
		return malwabpTime;
	}
};

#endif /* LINE_H_ */

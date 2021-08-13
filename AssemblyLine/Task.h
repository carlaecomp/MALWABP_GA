/*
 * Task.h
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#ifndef TASK_H_
#define TASK_H_

#include <stddef.h>  // defines size_t
#include <vector>
#include <iostream>

using namespace std;

class Task {
	int id;

	/**
	 * cannot declare as Station. Even as a pointer, we get the following compilation error:
	 * invalid use of incomplete type ‘class Station’
	 * immediatePredecessors[i]->getStation()->getId()
	 */
	int station;

	int earliestWorkstation;
	int latestWorkstation;

	vector< vector<double> > executionTime; // (worker, model)
	vector<int> avgExecutionTime;        // worker (average in models)

	vector<Task*> immediatePredecessors;
	vector<Task*> immediateSuccessors;

	void resize(int W, int M);

public:
	Task(int id, int W, int M);
	virtual ~Task();
	const static double MAX = 2000000.0-100;
	void setExecutionTime(int w, int m, double value);
	double getExecutionTime(int w, int m);
	void addImmediatePredecessor(Task* immediatePredecessor);
	void addImmediateSuccessor(Task* immediateSuccessor);
	void updateEarliestLatestWorkstation(int earliest, int latest);
	vector<Task*>::const_iterator getImmediatePredecessorsBeginIterator();
	vector<Task*>::const_iterator getImmediatePredecessorsEndIterator();
	vector<Task*>::const_iterator getImmediateSuccessorsBeginIterator();
	vector<Task*>::const_iterator getImmediateSuccessorsEndIterator();
	double getAvgExecutionTime(int qtdW, int qtdM);

	int getStation() const {
		return station;
	}
	void setLatestWorkstation(int station) {
		this->latestWorkstation = station;
	}
	void setEarliestWorkstation(int station) {
		this->earliestWorkstation = station;
	}
	void setStation(int station) {
		this->station = station;
	}

	const vector<int>& getAvgExecutionTime() const {
		return avgExecutionTime;
	}

	void setAvgExecutionTime(const vector<int>& avgExecutionTime) {
		this->avgExecutionTime = avgExecutionTime;
	}

	int getId() const {
		return id;
	}

	void setId(int id) {
		this->id = id;
	}

	const int getEarliestWorkstation() const {
		return earliestWorkstation;
	}

	const int getLatestWorkstation() const {
		return latestWorkstation;
	}
};

#endif /* TASK_H_ */

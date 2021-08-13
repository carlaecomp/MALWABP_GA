/*
 * Station.h
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#ifndef STATION_H_
#define STATION_H_

#include "Task.h"
#include "Worker.h"
#include <vector>
#include <set>

using namespace std;

class Station {
	int id;
	bool balanced;
	Worker assignedWorker;
	set<Task*> assignedTasks;
	vector<double> t_mk;    // per model
	vector<double> t_mk_uw; // per model, utility worker
	vector<double> wo_mk;   // work overload per model

public:
	const static double MAX = 2000000.0;

	Station(int id);
	virtual ~Station();

	void addTask(Task* t);
	void removeTask(Task* t);
	void removeAllTasks();

	set<Task*>::iterator getAssignedTasksBeginIterator();
	set<Task*>::iterator getAssignedTasksEndIterator();

	void calculateTmk(const int M, const int W);
	void calculateWOmk(const int M, double cycleTime, const double LK);

	double getTmk(int m);
	double getTmkUw(int m);
	double getDelta(int m);
	double getWOmk(int m);

	int getId() const {
		return id;
	}

	const Worker& getAssignedWorker() const {
		return assignedWorker;
	}

	void setAssignedWorker(const Worker& assignedWorker) {
		this->assignedWorker = assignedWorker;
		balanced = true;
	}
};

#endif /* STATION_H_ */

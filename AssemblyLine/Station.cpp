/*
 * Station.cpp
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#include "Station.h"

Station::Station(int id)
: assignedWorker(Worker(-1)) {
	this->id = id;
	balanced = false;
}

Station::~Station() {
	/* // cerr << assignedTasks.size() << " ";
	for (set<Task*>::iterator it = assignedTasks.begin(); it != assignedTasks.end(); ++it) {
		// cerr << (*it)->getId() << " ";
		delete *it;
	}*/
}

set<Task*>::iterator Station::getAssignedTasksBeginIterator() {
	return assignedTasks.begin();
}

set<Task*>::iterator Station::getAssignedTasksEndIterator() {
	return assignedTasks.end();
}

void Station::addTask(Task* t) {
	assignedTasks.insert(t);
}

void Station::removeTask(Task* t) {
	assignedTasks.erase(t);
}

void Station::removeAllTasks() {
	assignedTasks.clear();
}

void Station::calculateTmk(int M, int W) {
	int m, w, min;

	t_mk.clear();
	t_mk.resize(M);
	t_mk_uw.clear();
	t_mk_uw.resize(M);

	for (m = 0; m < M; m++) {
		t_mk[m] = t_mk_uw[m] = 0;

		for (set<Task*>::iterator it = getAssignedTasksBeginIterator();
				it != getAssignedTasksEndIterator(); ++it) {
			// t_mk
			t_mk[m] += (*it)->getExecutionTime(getAssignedWorker().getId(), m);

			// t_uw(m, k) - utility worker execution time on model m and station k
			min = MAX;
			for (w = 0; w < W; w++) {
				if ((*it)->getExecutionTime(w, m) < min)
					min = (*it)->getExecutionTime(w, m);
			}
			t_mk_uw[m] += min;
		}
		// // cerr << t_mk[m] << "/" << t_mk_uw[m] << " ";
	}
	// // cerr << endl;
}


void Station::calculateWOmk(int M, double cycleTime, const double LK) {
	wo_mk.clear();
	wo_mk.resize(M);

	for (int m = 0; m < M; m++) {
		if (getTmk(m) > cycleTime)
			wo_mk[m] = getTmk(m) < (1+LK)*cycleTime ? (getTmk(m) - cycleTime) : LK*cycleTime;
		else if (getTmk(m) < cycleTime)
			wo_mk[m] = (cycleTime - getTmk(m)) < LK*cycleTime ? (getTmk(m) - cycleTime) : -LK*cycleTime;
		else
			wo_mk[m] = 0;
	}
}

double Station::getTmk(int m) {
	if (m < t_mk.size())
		return t_mk[m];

	// cerr << "Station::getTmk invalid param " << m << "-" << t_mk.size() << endl;
	return -1;
}

double Station::getTmkUw(int m) {
	if (m < t_mk_uw.size())
		return t_mk_uw[m];

	// cerr << "Station::getTmkUw invalid param " << m << "-" << t_mk_uw.size() << endl;
	return -1;
}

double Station::getDelta(int m) {
	if (m < t_mk_uw.size())
		return getTmk(m)/getTmkUw(m);

	// cerr << "Station::getDelta invalid param " << m << "-" << t_mk_uw.size() << endl;
	return -1;
}

double Station::getWOmk(int m) {
	if (m < wo_mk.size())
		return wo_mk[m];

	// cerr << "Station::getWOmk invalid param " << m << "-" << wo_mk.size() << endl;
	return -1;
}



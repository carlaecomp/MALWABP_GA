/*
 * Task.cpp
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#include "Task.h"

Task::Task(int id, int W, int M) {
	this->id = id;
	station = -1;
	resize(W, M);
}

Task::~Task() {
	// TODO Auto-generated destructor stub
}

void Task::resize(int W, int M) {
	if (W > 0 && M > 0){
		executionTime.resize(W);
		for (int w = 0; w < W; w++)
			executionTime[w].resize(M);
	}
}

void Task::setExecutionTime(int w, int m, double value) {
	// // cerr << executionTime.size() << " " << w << " " << executionTime[w].size() << " " << m << endl;
	executionTime[w][m] = value;
}

double Task::getExecutionTime(int w, int m) {
	return executionTime[w][m];
}

double Task::getAvgExecutionTime(int qtdW, int qtdM) {

	double avgTemp;
	double avgTemp2=0;
	int qtdWorker;
	int qtdCM =0;

	//vector<int> avgModels;

	for (int m = 0; m < qtdM; m++) {
		avgTemp = 0;
		qtdWorker=0;
		bool cont =false;
		for (int w = 0; w < qtdW; w++) {
			if (getExecutionTime(w,m) < MAX-10 && getExecutionTime(w,m)!=0) {
				avgTemp += getExecutionTime(w,m);
				qtdWorker++;
				cont = true;
				//cout << "recebeu true " << endl;
			}
		}
		if(cont){
			//cout << "aumentou " << endl;
			qtdCM++;
			avgTemp2+=avgTemp/qtdWorker;
		}
		//avgModels[m] = avgTemp/qtdWorker;

	}
	return (avgTemp2/qtdCM);
}

void Task::addImmediatePredecessor(Task* immediatePredecessor) {
	immediatePredecessors.push_back(immediatePredecessor);
	// // cerr << immediatePredecessor->getId()+1 << "-" << immediatePredecessors[immediatePredecessors.size()-1]->getId()+1 << " ";
}

void Task::addImmediateSuccessor(Task* immediateSuccessor) {
	immediateSuccessors.push_back(immediateSuccessor);
}

void Task::updateEarliestLatestWorkstation(int earliest, int latest) {
	int station;
	size_t i;

	earliestWorkstation = earliest;
	for (i = 0; i < immediatePredecessors.size(); i++) {
		station = immediatePredecessors[i]->getStation();
		if (station > earliestWorkstation)
			earliestWorkstation = station;
	}

	latestWorkstation = latest;
	for (i = 0; i < immediateSuccessors.size(); i++) {
		station = immediateSuccessors[i]->getStation();
		if (station < latestWorkstation)
			latestWorkstation = station;
	}
}

vector<Task*>::const_iterator Task::getImmediatePredecessorsBeginIterator() {
	return immediatePredecessors.begin();
}

vector<Task*>::const_iterator Task::getImmediateSuccessorsBeginIterator() {
	return immediateSuccessors.begin();
}

vector<Task*>::const_iterator Task::getImmediatePredecessorsEndIterator() {
	return immediatePredecessors.end();
}

vector<Task*>::const_iterator Task::getImmediateSuccessorsEndIterator() {
	return immediateSuccessors.end();
}

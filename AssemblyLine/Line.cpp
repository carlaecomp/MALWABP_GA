/*
 * Line.cpp
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#include "Line.h"

Line *Line::instance = NULL;

Line::Line(int totalDemand, int planningHorizon, double alpha) {
	N = W = K = M = 0;
	this->totalDemand = totalDemand;
	this->planningHorizon = planningHorizon;
	this->alpha = alpha;
	cycleTime = 0;
}

Line* Line::getInstance(int totalDemand, int planningHorizon, double alpha)
{
   if (!instance)
      instance = new Line(totalDemand, planningHorizon, alpha);

   return instance;
}

Line::~Line() {
	unassignAllTasks();

	for (vector<Task*>::iterator it = getUnassignedTasksBeginIterator(); it != getUnassignedTasksEndIterator(); ++it) {
		delete *it;
		(*it) = NULL;
	}

	for (vector<Model*>::iterator it = models.begin(); it != models.end(); ++it) {
		delete *it;
		(*it) = NULL;
	}
}

bool Line::create(char* subdir, char* filename) {
	if (N > 0) {
		cerr << "Line has already been created\n";
		return false;
	}

	int i, j, k, w, m;
	vector<vector<int> > graph;

	ReadInstance instance(filename, subdir, alpha);

	if (instance.readMalwabpInstance()) {
		N = instance.getN();
		K = W = instance.getW();
		M = instance.getM();

		graph = instance.getGraph();
		nameInstance =  instance.getMalwabpFileName();
		// create tasks
		for (i = 0; i < N; i++) {
			Task* t = new Task(i, W, M);
			for (w = 0; w < W; w++) {
				for (m = 0; m < M; m++)	{
					t->setExecutionTime(w, m, instance.getMalwabpTime(m, i, w));
				}
			}
			// // cerr << i+1 << " ---- ";
			for (j = 0; j < graph[i].size(); j++) {
				// // cerr << graph[i][j]+1 << "-" << unassignedTasks[graph[i][j]]->getId()+1 << "-";
				// this is only possible because the instance presents ordered tasks and predecessors have a lower id
				t->addImmediatePredecessor(unassignedTasks[graph[i][j]]);
			}
			// // cerr << endl;
			unassignedTasks.push_back(t);
		}
		immediateSuccessors();

		// create stations and workers
		for (k = 0; k < K; k++) {
			Worker worker(k);
			unassignedWorkers.push_back(worker);
			Station station(k);
			stations.push_back(station);
		}

		// create models
		for (m = 0; m < M; m++) {
			Model* model = new Model(m, instance.getDemand(m), totalDemand, instance.getAvgDayDemand(m));
			models.push_back(model);
		}
		avgDayDemand = instance.getAvgDayDemand();
		totalDemandAvg = instance.getTotalDemandAvg();
		malwabpTime = instance.getMalwabpTime();
		return true;
	}
	return false;
}


void Line::prepareTasksForGA() {


	for (int n = 0; n < this->N; n++) { // Percorrer as tarefas

		vector<int> avg(K);
		double somaModelo=0;
		unassignedTasks[n]->setLatestWorkstation(-1);
		unassignedTasks[n]->setEarliestWorkstation(K);

		for (int k = 0; k < this->K; k++) { // Percorrer os trabalhadores

			for (int m = 0; m < this->M; m++) { // Percorrer os modelos

				if(unassignedTasks[n]->getExecutionTime(k, m) > MAX - 10){
					//// cerr << "ENTROU NO IF"<< "trab " << k << "tarefa" << n << endl;
					avg[k] = MAX;
					break;
				}else{
					somaModelo = somaModelo + unassignedTasks[n]->getExecutionTime(k, m);
				}
			}
			if (avg[k] == 0) {
				avg[k] = somaModelo/M;
			}

			//// cerr << "o avg" << avg[k] << "trab " << k << "tarefa" << n << endl;

		}

		unassignedTasks[n]->setAvgExecutionTime(avg);
	}


}

void Line::immediateSuccessors() {
	vector<Task*>::const_iterator it;
	for (size_t i = 0; i < unassignedTasks.size(); i++) {
		// // cerr << unassignedTasks[i]->getId()+1 << " ";
		for (it = unassignedTasks[i]->getImmediatePredecessorsBeginIterator();
				it != unassignedTasks[i]->getImmediatePredecessorsEndIterator(); ++it) {
			// // cerr << (*it)->getId()+1 << " ";
			(*it)->addImmediateSuccessor(unassignedTasks[i]);
		}
		// // cerr << endl;
		unassignedTasks[i]->updateEarliestLatestWorkstation(0, K-1);
	}
}

Model* Line::getModel(int m) {
	if (m < models.size())
		return models[m];

	// cerr << "Line::getModel invalid param " << m << "-" << models.size() << endl;
	return NULL;
}

Station Line::getStation(int k) {
	if (k < stations.size())
		return stations[k];

	// cerr << "Line::getStation invalid param " << k << endl;
	return Station(k);
}

int Line::getLastStationId() {
	if (K)
		return stations[K-1].getId();
	return -1;
}

int Line::getFirstStationId() {
	if (K)
		return stations[0].getId();
	return -1;
}

vector<Task*>::iterator Line::getUnassignedTasksBeginIterator() {
	return unassignedTasks.begin();
}

vector<Task*>::iterator Line::getUnassignedTasksEndIterator() {
	return unassignedTasks.end();
}


vector<Task*> Line::getTasks(){
	return unassignedTasks;
}

void Line::unassignAllTasks() {
	for (int k = 0; k < K; k++) {
		for (set<Task*>::iterator it = stations[k].getAssignedTasksBeginIterator();
				it != stations[k].getAssignedTasksEndIterator(); ++it) {
			unassignedTasks.push_back(*it);
		}
		stations[k].removeAllTasks();
	}
}

void Line::print() {
	int i, k, w, m;
	// cerr << "Line::print() Parameters: M " << M << " N " << N << " W " << W << " demand " << totalDemand << endl << endl;

	// Stations
	for (k = 0; k < K; k++) {
		w = stations[k].getAssignedWorker().getId();
		// cerr << "Station " << k+1 << " worker " << w+1 << " task ";
		for (set<Task*>::iterator it = stations[k].getAssignedTasksBeginIterator();
				it != stations[k].getAssignedTasksEndIterator(); ++it) {
			// cerr << (*it)->getId()+1 << " ";
		}
		// cerr << endl;
	}

	// Tasks
	for (i = 0; i < unassignedTasks.size(); i++) {
		// cerr << "-----------------------\nTask " << i+1 << "-" << unassignedTasks[i]->getId()+1 << " time (worker x model)\n";
		for (w = 0; w < W; w++) {
			for (m = 0; m < M; m++) {
				// cerr << unassignedTasks[i]->getExecutionTime(w, m) << " ";
			}
			// cerr << endl;
		}
		// cerr << "-----------------------\n";
	}
	// cerr << "Predecessors\n";
	for (k = 0; k < K; k++) {
		for (set<Task*>::iterator itTask = stations[k].getAssignedTasksBeginIterator();
				itTask != stations[k].getAssignedTasksEndIterator(); ++itTask) {
			// cerr << (*itTask)->getId()+1;
			for (vector<Task*>::const_iterator it = (*itTask)->getImmediatePredecessorsBeginIterator();
					it != (*itTask)->getImmediatePredecessorsEndIterator(); ++it) {
				// cerr << " " << (*it)->getId()+1;
			}
			// cerr << endl;
		}
	}
	// cerr << "Successors\n";
	for (k = 0; k < K; k++) {
		for (set<Task*>::iterator itTask = stations[k].getAssignedTasksBeginIterator();
					itTask != stations[k].getAssignedTasksEndIterator(); ++itTask) {
			// cerr << (*itTask)->getId()+1;
			for (vector<Task*>::const_iterator it = (*itTask)->getImmediateSuccessorsBeginIterator();
					it != (*itTask)->getImmediateSuccessorsEndIterator(); ++it) {
				// cerr << " " << (*it)->getId()+1;
			}
			// cerr << endl;
		}

	}
}
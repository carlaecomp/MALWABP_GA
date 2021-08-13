/*
 * Individual.cpp
 *
 *  Created on: 21/06/2015
 *      Author: Carla Santana
 */

#include "Individual.h"


Individual::Individual(vector<int> models, vector<int> workers, vector<int> tasks, vector<int> station, vector<double> timeStation, double wo) {

	this->models = models;
	this->workers = workers;
	this->tasks = tasks;
	this->station = station;
	this->timeStation = timeStation;
	this->cycleTime = timeStation[workers.size()];
	this->wo = wo;
	// Poderação : 460 roszieg 760 heskia
	this->fitness = wo + 50*cycleTime;
	for (int i = 0; i < workers.size(); i++) {
		sumStationCycle += timeStation[i];
	}

}

Individual::~Individual() {
	// TODO Auto-generated destructor stub
}

double Individual::getFitness(){
	return fitness;
}

vector<int> Individual::getModels(){
	return models;
}

vector<int> Individual::getWorkers(){
	return workers;
}
vector<int> Individual::getTasks(){
	return tasks;
}

vector<int> Individual::getStation(){
	return station;
}

double Individual::getCycleTime(){
	return cycleTime;
}

double Individual::getWo(){
	return wo;
}

void Individual::printFitness(){

	cout << "Fitness: " << (fitness) << ": Cycle Time: " << cycleTime << ": WO: " << wo << endl;
}
void Individual::printFitnesscerr(){

	cerr << "Fitness: " << (fitness) << ": Cycle Time: " << cycleTime << ": WO: " << wo << endl;
}
void Individual::printAll(){
	cerr << "Tarefas :" << endl;
	for (int i = 0; i < tasks.size(); i++) {
		cerr << tasks[i] << ":";
	}
	cerr << "" << endl;

	cerr << "Estações :" << endl;
	for (int i = 0; i < station.size(); i++) {
		cerr << station[i] << ":";
	}
	cerr << "" << endl;

	cerr << "Trabalhadores :" << endl;
	for (int i = 0; i < workers.size(); i++) {
		cerr << workers[i] << ":";
	}
	cerr << "" << endl;

	cerr << "Modelos :" << endl;
	for (int i = 0; i < models.size(); i++) {
		cerr << models[i] << ":";
	}

	cerr << "Tempo cada estação :" << endl;
	for (int i = 0; i < timeStation.size()-1; i++) {
		cerr << timeStation[i] << ":";
	}
	cerr << "" << endl;
}

vector< vector < int > > Individual::getAssignedTasks(){
	int t_inicio=0;
	int t_limite = tasks.size();
	vector< vector < int > >  assignedTasks;
	assignedTasks.resize(workers.size());
	//int aux;
	for (int k = 0; k < workers.size(); k++) {
		//cout << " K :" << k << endl;
		if (t_limite != tasks.size()) {
			t_inicio = t_limite;
			t_limite = tasks.size();
		}
		int aux=0;
		// cout << " inicio :" << t_inicio << " fim: " << t_limite << endl;
		for (int n = t_inicio; n < t_limite; n++) {

			if (station[n] != k) {
				t_limite = n;
				// cout << "Definiu limite " << n << endl;
				break;
			}else{
				// cout << "Tamanhos: " << assignedTasks.size() << " " << workers.size()<<endl;
				assignedTasks[k].push_back(tasks[n]);
				// cout << assignedTasks[k][aux] <<" ok";
				// cout << assignedTasks[k][aux] <<" : " << n << " ; ";
				aux++;
			}
		}

	}

	return assignedTasks;

}

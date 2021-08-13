/*
 * GA.h
 *
 *  Created on: 21/06/2015
 *      Author: Carla Santana
 */

#ifndef GENETICALGORITHM_H_
#define GENETICALGORITHM_H_


#include "../AssemblyLine/Line.h"
#include "../AssemblyLine/Task.h"
#include <stddef.h>  // defines NULL
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <sstream>
#include <map>


using namespace std;
#include "Individual.h"


class GeneticAlgorithm {
	
	const static double MAX = 2000000.0-100;
	vector<int> nulo;
	int infacCrossover =0;
	int infacMutation =0;
	int qtdInfacGenes=0;
	vector<int> timeStationTemp;
	Individual* nullo;
	double fitnessTheBest =0;
	static GeneticAlgorithm* instance; // singleton
	Line* assemblyLine;
	vector<Task*> allTask;
	vector<Individual*> population;
	vector<Individual*> theBest;
	Individual* factivel;
	vector< vector<int> > tableTasks;
	int sizePop;
	double cycleTime;
	int qtdInfacNewPop =0;
	GeneticAlgorithm(Line* assemblyLine);

public:
	static GeneticAlgorithm* getInstance(Line* assemblyLine);
	virtual ~GeneticAlgorithm();
	void generateTableTasks();
	bool containsSucessors(int p, int f);
	void printTableTasks();
	void generateInicialPop(int qtdI);
	Individual* generateIndividual();
	vector<int> generateIndividualTasks();
	vector<int> generateIndividualModel();
	vector<int> generateIndividualStation(vector<int> taskI);
	vector<int> generateIndividualWorker(vector<int> taskI, vector<int> stationI);
	vector<int> tasksFree(vector< vector<int> >, vector<int> taskI);
	void printVector(vector<int> v);
	void crossover(int porc);
	bool generateF(int p1, int p2);
	vector<int> generateFModels(vector<int> p1, vector<int> p2);
	vector<int> generateFTasks(vector<int> p1,vector<int> p2);
	vector<double> generateTimeStation(vector<int> tasks, vector<int> workers, vector <int> station );
	double getApproximateWo(int i, vector<int> xh, vector<Station> stationsTemp, int I);
	double generateWo(vector<int> tasks, vector<int> workers, vector <int> station, vector<int> models , double t_cycle);
	void selection();
	void printPopulation();

	int stationOfTask(int task, int ind);
	int getLatestWorkstation(int task, int ind);
	int getEarliestWorkstation(int task, int ind);
	void mutation(int porc);

	int lastPositionStation(int ind, int k);
	int earlyPositionStation(int ind, int k);
	bool moveTask(int ind, int kOld, int kNew, int pTask);
	void printTheBest();
	vector<Individual*> getTheBest();
	vector<int> updateStationMutation(int knew, int kold, int ind, vector<int> stations);
	bool mutationTask();
	void mutationModels();
	vector<int> generateFWorkerInfeasible();
	void printDemandFrac();
	void generateNewIndividuals();
	int getInfacCrossover(){
		return infacCrossover;
	}
	int getInfacMutation(){
		return infacMutation ;
	}
	int getQtdInfacNewPop(){
		return qtdInfacNewPop;
	}
	Individual* getFactivel(){
		return factivel;
	}
};


#endif /* GA_GENETICALGORITHM_H_ */

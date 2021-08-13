/*
 * Main.cpp
 *
 *  Created on: 21/05/2015
 *      Author: Carla Santana
 */

#include "AssemblyLine/Line.h"
#include "GA/GeneticAlgorithm.h"
#include "IBMCplex/Sequencing.h"
#include <stddef.h>
#include <cstdlib>
#include <csignal>
#include <exception>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    srand(time(NULL));

   Line* assemblyLine =  Line::getInstance(atoi(argv[1]), atoi(argv[2]), atof(argv[3]));
   for (int it = 5; it < argc; it++) {
	   try {
		   if (!assemblyLine->create(argv[4], argv[it])) {
			cerr << "Main Error: could not create assembly line\n";
			exit(EXIT_FAILURE);
		   }
	   } catch(exception& e) {
		   cerr << "Main Error: " << e.what() << endl;
	   }
   	}
   	assemblyLine->print();

    GeneticAlgorithm* ga = GeneticAlgorithm::getInstance(assemblyLine);

    //cout << "***Familia : " << assemblyLine->getNameInstance() << endl;

    time_t start, end; //declara vars
    time(&start);

    cout << "30% mutação f = wo + 50*c Familia_" << assemblyLine->getNameInstance() << " ";

    ga->generateTableTasks();
    ga->generateInicialPop(150);
    ga->printPopulation();
    double somaInfacCroos =0;

	for (int i = 1; i <= 300; i++) {
		ga->mutation(30);
		ga->crossover(25);
		ga->selection();
		ga->printPopulation();
	}

	int K = assemblyLine->getK();
	int M = assemblyLine->getM();
	vector<int> dayDemand = assemblyLine->getAvgDayDemand();
	vector < vector < vector<double> > > malwabpTime = assemblyLine->getMalwabpTime();
	int i = ga->getTheBest().size()-1;
	double solveSeq;


	if (!ga->getTheBest()[i]->getInfeasible()) {
		Sequencing s(K, M, assemblyLine->getTotalDemandAvg(), assemblyLine->getTotalDemand(),
			ga->getTheBest()[i]->getCycleTime(), dayDemand, ga->getTheBest()[i]->getModels(),
			malwabpTime, ga->getTheBest()[i]->getAssignedTasks(), ga->getTheBest()[i]->getWorkers());

		solveSeq = s.solve();
		cout << ga->getTheBest()[i]->getCycleTime()<< " " << solveSeq << " ";
	}else if(ga->getFactivel()->getWo()!=0){
		Sequencing s(K, M, assemblyLine->getTotalDemandAvg(), assemblyLine->getTotalDemand(),
					ga->getFactivel()->getCycleTime(), dayDemand, ga->getFactivel()->getModels(),
					malwabpTime, ga->getFactivel()->getAssignedTasks(), ga->getFactivel()->getWorkers());

		solveSeq = s.solve();
		cout << ga->getFactivel()->getCycleTime()<< " " << solveSeq << " ";

	}else{
		cout<<"Não houve factiveis"<<endl;
	}
    time(&end);
    double executionTime = difftime(end, start);
    cout << executionTime << endl;
    delete ga;
    return EXIT_SUCCESS;
}

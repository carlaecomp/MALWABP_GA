/*
 * ReadInstance.h
 *
 *  Created on: 19/05/2015
 *      Author: pamela
 */

#ifndef READINSTANCE_H_
#define READINSTANCE_H_

//#include "../Constants.h"
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class ReadInstance {
	char filename[128];            // IN2
	char subDirectoryFilename[16]; // IN
	char alpha[8];
    int M, N, W, totalDemand;
    vector < vector < vector<double> > > malwabpTime;
    vector< vector<int> > graph;   // Joint precedence graph
    vector<double> demand; // demand from file \in [0,1]
    vector<int> avgDayDemand;
    int totalDemandAvg;
    vector< vector<int> > matrixDayDemand;
    const static double ZERO = 0.000001;
    const static double MAX = 2000000.0;



public:
	ReadInstance(char* filename, char* subdir, double alpha);
	char* getMalwabpFileName();
	char* getDemandFileName();
	bool readMalwabpInstance();
	double getMalwabpTime(int m, int i, int w);
	double getDemand(int m);
	double getAvgDayDemand(int m);
	void calculateAvgDayDemand();

	const vector<vector<int> >& getGraph() const {
		return graph;
	}

	int getTotalDemandAvg() const {
		return totalDemandAvg;
	}

	int getI() const {
		return totalDemand;
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

	const vector<vector<int> >& getMatrixDayDemand() const {
		return matrixDayDemand;
	}
	vector<int> getAvgDayDemand(){
		return avgDayDemand;
	}
	vector < vector < vector<double> > > getMalwabpTime(){
		return malwabpTime;
	}
};

#endif /* READINSTANCE_H_ */

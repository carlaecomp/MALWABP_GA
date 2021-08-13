/*
 * ReadInstance.cpp
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#include "ReadInstance.h"

ReadInstance::ReadInstance(char* file, char* subdir, double alpha) {
	M = N = W = totalDemand = 0;
	strcpy(filename, file);
	strcpy(subDirectoryFilename, subdir);

	if (abs(alpha - 0.1) < ZERO)
		strcpy(this->alpha, "01");
	else if (abs(alpha - 0.5) < ZERO)
		strcpy(this->alpha, "05");
	else
		strcpy(this->alpha, "1");
}

char* ReadInstance::getMalwabpFileName() {
	char* str = new char[128];
	int size = strlen(subDirectoryFilename);

	strcpy(str, filename);
	char *aux = strrchr(str,'/');
	memmove(aux+size, aux, strlen(aux));
	strncpy(aux, subDirectoryFilename, size); //sub-directory
	str[strlen(str)+size]='\0';

	return str;
}

char* ReadInstance::getDemandFileName() {
	char* str = new char[128];
	int size = strlen(subDirectoryFilename);

	strcpy(str, filename);
	char *aux = strrchr(str,'/');
	memmove(aux+size, aux, 4);
	strncpy(aux, subDirectoryFilename, size);
	strncpy(aux+size+4, alpha, strlen(alpha));
	*(aux+size+4+strlen(alpha))='\0';

	return str;
}

bool ReadInstance::readMalwabpInstance() {
    int i, j, m, w, p, su; //predecessor e successor
    double num;
    string str;
    char* malwabpFilename = getMalwabpFileName();
    char* demandFilename = getDemandFileName();

    vector<int> aux, pre;
    vector< vector<int> > sucessors;

    vector<double> workersTime;
    vector < vector<double> > tasksTime;

    cerr << "ReadInstance::readMalwabp " << malwabpFilename << " original " << filename << endl;

    ifstream file(malwabpFilename, ifstream::in);
    if (!file.is_open()) {
    	cerr << "Error: could not open file " << malwabpFilename << endl;
    	delete[] malwabpFilename;
    	return false;
    }
    delete[] malwabpFilename;

	file >> M >> N >> W;// >> totalDemand;

	getline(file, str); //\n
	getline(file, str); //\n

	// read the MALWABP workersTime
	workersTime.resize(W);
	for (m = 0; m < M; m++) {
		tasksTime.clear();
		for (i = 0; i < N; i++) {
			for (w = 0; w < W; w++) {
				file >> num;
				workersTime[w] = num < MAX ? num : MAX;
			}
			getline(file, str); //\n
			tasksTime.push_back(workersTime);
		}
		malwabpTime.push_back(tasksTime);
		getline(file, str); //\n
	}

	// read demand for each model
	demand.resize(M);
	for (m = 0; m < M; m++) {
		file >> num;
		demand[m] = num;
	}

	file.close();
	file.open(filename);
	if (!file.is_open()) {
		return false;
	}

	// read only the precedence graph in this file
	file >> num; // N
	for (i = 0; i <= num; i++)
		getline(file, str); // ALWABP tasks time

	file >> p >> su;
	i = p;
	while (p > 0 && su > 0) {
		pre.push_back(p);
		// como qtde de sucessores varia, precisa controlar se esta no mesmo antecessor
		// ou i != p (acabou sucessores do predecessor p)
		while (i == p) {
			aux.push_back(su);
			file >> p >> su;
		}
		i = p;
		sucessors.push_back(aux);
		aux.clear();
	}

	file.close();

	graph.resize(N);
	for (i = 0; i < pre.size(); i++)
		for (j = 0; j < sucessors[i].size(); j++)
			graph[sucessors[i][j]-1].push_back(pre[i]-1); //predecessores diretos (Dj) - tasks comecam em 0

    getDemandFileName();

	file.close();
	file.open(demandFilename);
	 if (!file.is_open()) {
		cerr << "Error: could not open file " << demandFilename << endl;
		delete[] demandFilename;
		return false;
	}
	delete[] demandFilename;

	matrixDayDemand.resize(20);
	for (i = 0; i < 20; i++) {
		matrixDayDemand[i].resize(M);
		for (m = 0; m < M; m++) {
			file >> matrixDayDemand[i][m];
		}
	}
	file.close();
	calculateAvgDayDemand();

	return true;
}
double ReadInstance::getAvgDayDemand(int m){
	return avgDayDemand[m];
}
void ReadInstance::calculateAvgDayDemand(){
	vector<double> aux;
	aux.resize(M);
	avgDayDemand.resize(M);
	for (int m = 0; m < M; m++) {
		for (int i = 0; i < 20; i++) {
			aux[m] += matrixDayDemand[i][m];
		}
	}
	totalDemandAvg = 0;
	for (int m = 0; m < M; m++) {
		avgDayDemand[m] = (double) aux[m]/20;
		totalDemandAvg += avgDayDemand[m];
	}

}
double ReadInstance::getMalwabpTime(int m, int i, int w) {
	if (m >=0 && i >=0 && w >=0 &&
			m < malwabpTime.size() &&
			i < malwabpTime[0].size() &&
			w < malwabpTime[0][0].size())  
			return malwabpTime[m][i][w];

	return -1;
}

double ReadInstance::getDemand(int m) {
	if (m < demand.size())
		return demand[m];

	return -1;
}

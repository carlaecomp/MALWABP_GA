/*
 * Model.cpp
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#include "Model.h"

Model::Model(int id, double fractionDemand, int totalDemand, int demand) {
	this->id = id;
	this->fractionDemand = fractionDemand;
	this->demand = demand;
	this->dayDemand = -1;
}

Model::~Model() {
	// TODO Auto-generated destructor stub
}

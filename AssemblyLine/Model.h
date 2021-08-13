/*
 * Model.h
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <vector>

using namespace std;

class Model {
	int id;
	double fractionDemand; // from file \in [0, 1]
	int demand;         // fractionDemand * I
	int dayDemand;

public:
	Model(int id, double fractionDemand, int I, int demand);
	virtual ~Model();

	int getDemand() const {
		return demand;
	}

	double getFractionDemand() const {
		return fractionDemand;
	}

	int getId() const {
		return id;
	}

	int getDayDemand() const {
		return dayDemand;
	}

	void setDayDemand(int dayDemand) {
		this->dayDemand = dayDemand;
	}
};

#endif /* MODEL_H_ */

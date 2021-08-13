/*
 * Worker.h
 *
 *  Created on: 19/05/2015
 *      Author: Pamela Cortez
 */

#ifndef WORKER_H_
#define WORKER_H_

class Worker {
	int id;

public:
	Worker();
	Worker(int id);
	virtual ~Worker();

	int getId() const {
		return id;
	}
};

#endif /* WORKER_H_ */

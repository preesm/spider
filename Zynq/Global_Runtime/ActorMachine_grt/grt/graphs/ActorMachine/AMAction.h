/*
 * AMAction.h
 *
 *  Created on: Jul 3, 2013
 *      Author: jheulot
 */

#ifndef AMACTION_H_
#define AMACTION_H_

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "../../SchedulerDimensions.h"


class AMAction {
private:
	int functionID;
	int nbFifoIn;
	int nbFifoOut;
	int nbArgs;
	int fifoIn[MAX_SRDAG_INPUT_EDGES];
	int fifoOut[MAX_SRDAG_OUTPUT_EDGES];
	int args[MAX_VARIABLE_NUMBER];
	char name[AM_ACTION_NAME];

public:
	AMAction(){
		functionID=-1;
		nbFifoIn=0;
		nbFifoOut=0;
		nbArgs=0;
	}

	int getFunctionId(){return functionID;}
	void setFunctionId(int functionId){functionID = functionId;}


	int getNbArgs(){return nbArgs;}
	int getArg(int id){return args[id];}
	void addArg(int value){
		if(nbArgs>=MAX_VARIABLE_NUMBER){
			printf("nbArgs>=MAX_VARIABLE_NUMBER\n");
			abort();
		}
		args[nbArgs++] = value;
	}

	int getNbFifoIn(){return nbFifoIn;}
	int getFifoIn(int id){return fifoIn[id];}
	void addFifoIn(int value){
		if(nbFifoIn>=MAX_SRDAG_INPUT_EDGES){
			printf("nbFifoIn>=MAX_SRDAG_INPUT_EDGES\n");
			abort();
		}
		fifoIn[nbFifoIn++] = value;
	}

	int getNbFifoOut(){
		if(nbFifoOut>=MAX_SRDAG_OUTPUT_EDGES){
			printf("nbFifoOut>=MAX_SRDAG_OUTPUT_EDGES\n");
			abort();
		}return nbFifoOut;
	}
	int getFifoOut(int id){return fifoOut[id];}
	void addFifoOut(int value){fifoOut[nbFifoOut++] = value;}

	void setName(const char* _name){strncpy(name, _name, AM_ACTION_NAME);}
	char* getName(){return name;}

};

#endif /* AMACTION_H_ */

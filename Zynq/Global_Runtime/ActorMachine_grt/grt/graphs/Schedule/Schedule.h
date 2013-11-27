/*
 * Schedule.h
 *
 *  Created on: Jul 1, 2013
 *      Author: jheulot
 */

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "../SRDAG/SRDAGVertex.h"
#include "../SRDAG/SRDAGGraph.h"
#include <grt_definitions.h>
#include <string.h>

typedef struct communication{
	int start;
	int end;
}communication;

class Schedule {
private:
	SRDAGVertex* schedule[MAX_SLAVES][MAX_SRDAG_VERTICES];
	communication coms[MAX_SLAVES][MAX_SRDAG_VERTICES*2];
	int nbVertex[MAX_SLAVES];
	int nbComs[MAX_SLAVES];
	unsigned int readyTime[MAX_SLAVES];
	unsigned int vertexStartTime[MAX_SRDAG_VERTICES];
	unsigned int vertexEndTime[MAX_SRDAG_VERTICES];

public:
	Schedule(){
		memset(schedule, 0, MAX_SLAVES*MAX_SRDAG_VERTICES*sizeof(SRDAGVertex*));
		memset(nbVertex, 0, MAX_SLAVES*sizeof(int));
		memset(readyTime, 0, MAX_SLAVES*sizeof(int));
		memset(vertexStartTime, 0, MAX_SLAVES*sizeof(int));
		memset(vertexEndTime, 0, MAX_SLAVES*sizeof(int));
		memset(coms, 0, MAX_SLAVES*MAX_SRDAG_VERTICES*sizeof(communication));
		memset(nbComs, 0, MAX_SLAVES*sizeof(int));
	}

	virtual ~Schedule(){};

	SRDAGVertex* getVertex(int slave, int scheduleIndex){
		return schedule[slave][scheduleIndex];
	}

	void addVertex(int slave,SRDAGVertex* vertex, int startTime, int endTime){
		schedule[slave][nbVertex[slave]++] = vertex;
		readyTime[slave] = endTime;
		vertexStartTime[vertex->getBase()->getVertexIndex(vertex)] = startTime;
		vertexEndTime[vertex->getBase()->getVertexIndex(vertex)] = endTime;
	}

	void addCom(int slave, int start, int end){
		coms[slave][nbComs[slave]].start = start;
		coms[slave][nbComs[slave]].end = end;
		nbComs[slave]++;
	}

	bool isPresent(int slave, SRDAGVertex* vertex){
		for(int i=0; i<nbVertex[slave]; i++){
			if(vertex->getCsDagReference() == schedule[slave][i]->getCsDagReference() &&
					vertex->getReferenceIndex() == schedule[slave][i]->getReferenceIndex())
				return true;
		}
		return false;
	}

	int getNbVertex(int slave){
		return nbVertex[slave];
	}

	int getNbComs(int slave){
		return nbComs[slave];
	}

	unsigned int getReadyTime(int slave){
		return readyTime[slave];
	}

	unsigned int getVertexEndTime(int vertexID){
		return vertexEndTime[vertexID];
	}

	unsigned int getVertexStartTime(int vertexID){
		return vertexStartTime[vertexID];
	}

	unsigned int getComEndTime(int slave, int comId){
		return coms[slave][comId].end;
	}

	unsigned int getComStartTime(int slave, int comId){
		return coms[slave][comId].start;
	}
};

#endif /* SCHEDULE_H_ */

/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "../../graphs/SRDAG/SRDAGVertex.h"
#include "../../graphs/SRDAG/SRDAGGraph.h"
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
		vertexStartTime[vertex->getId()] = startTime;
		vertexEndTime[vertex->getId()] = endTime;
	}

	void addCom(int slave, int start, int end){
		coms[slave][nbComs[slave]].start = start;
		coms[slave][nbComs[slave]].end = end;
		nbComs[slave]++;
	}

	bool isPresent(int slave, SRDAGVertex* vertex){
//		for(int i=0; i<nbVertex[slave]; i++){
//			if(vertex->getCsDagReference() == schedule[slave][i]->getCsDagReference() &&
//					vertex->getReferenceIndex() == schedule[slave][i]->getReferenceIndex())
//				return true;
//		}
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

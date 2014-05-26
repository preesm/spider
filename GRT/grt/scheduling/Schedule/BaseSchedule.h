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

#ifndef BASESCHEDULE_H_
#define BASESCHEDULE_H_

#include "../../graphs/SRDAG/SRDAGVertex.h"
#include <scheduling/architecture/Architecture.h>
#include <platform_time.h>

typedef struct SchedType{
	void* 	vertex;
	UINT32	startTime;
	UINT32	endTime;
}SchedType;

#define MAX(a,b) ((a>b)?a:b)

class BaseSchedule {
	UINT32 nbActiveSlaves;
	UINT32 nbVertices[MAX_SLAVES];
	UINT32 slaveReadyTimes[MAX_SLAVES];
	SchedType schedules[MAX_SLAVES][MAX_SRDAG_VERTICES];
public:

	void reset(){
		memset(nbVertices, 0, sizeof(nbVertices));
		memset(slaveReadyTimes, 0, sizeof(slaveReadyTimes));
		memset(schedules, 0, sizeof(schedules));
	}

	void newStep(){
		UINT32 time = platform_time_getValue();
		for(int i=0; i<nbActiveSlaves; i++){
			slaveReadyTimes[i] = MAX(slaveReadyTimes[i],time);
		}
	}

    void setReadyTime(UINT32 slaveId, UINT32 time)
    {
        slaveReadyTimes[slaveId] = time;
    }

    UINT32 getReadyTime(UINT32 slaveId) const
    {
        return slaveReadyTimes[slaveId];
    }

    void* getVertex(UINT32 slaveId, UINT32 vertexIx)
    {
    	return schedules[slaveId][vertexIx].vertex;
    }

	int addSchedule(UINT32 slave_id, void* vertex, UINT32 startTime, UINT32 endTime){
		schedules[slave_id][nbVertices[slave_id]].vertex = vertex;
		schedules[slave_id][nbVertices[slave_id]].startTime = startTime;
		schedules[slave_id][nbVertices[slave_id]].endTime = endTime;

		slaveReadyTimes[slave_id] = endTime;

		int index = nbVertices[slave_id];
		(nbVertices[slave_id])++;

		return index;
    }

	SchedType* getSchedule(UINT32 slave_id, UINT32 vertexId){
		return &schedules[slave_id][vertexId];
	}

	bool isPresent(UINT32 slaveId, const UINT32 vertexIx, const void* vertex){
		return (schedules[slaveId][vertexIx].vertex == vertex);
//		for(UINT32 i=0; i<nbVertices[slaveId]; i++){
//			if(vertex->getId() == schedules[slaveId][i].vertex->getId())// &&	vertex->getReferenceIndex() == schedules[slaveId][i]->getIndex())
//				return true;
//		}
//		return false;
	}

    bool findSlaveId(UINT32 vertexIx, const void *vertex, UINT32 *slaveId){
    	for (UINT32 i = 0; i < nbActiveSlaves; i++) {
   			if(schedules[i][vertexIx].vertex == vertex){
				*slaveId = i;
    			return true;
   			}
    	}
    	return false;
    }

    UINT32 getVertexEndTime(UINT32 vertexIx, const void* vertex){
    	UINT32 slaveId;
    	if(findSlaveId(vertexIx, vertex, &slaveId))
    		return schedules[slaveId][vertexIx].endTime;
    	else
    		return -1;
    }


    UINT32 getVertexStartTime(UINT32 vertexIx, const void* vertex){
    	UINT32 slaveId;
    	if(findSlaveId(vertexIx, vertex, &slaveId))
    		return schedules[slaveId][vertexIx].startTime;
    	else
    		return -1;
    }


    /*
     * Auto generated setters and getters.
     */
    UINT32 getNbVertices(UINT32 slaveId) const
    {
        return nbVertices[slaveId];
    }

	void setNbActiveSlaves(UINT32 nbActiveSlaves){
		this->nbActiveSlaves = nbActiveSlaves;
	}
};

#endif /* BASESCHEDULE_H_ */

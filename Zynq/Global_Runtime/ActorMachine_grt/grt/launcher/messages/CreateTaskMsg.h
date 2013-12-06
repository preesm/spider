
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#ifndef TASKMSG_H_
#define TASKMSG_H_

#include "LRTMsg.h"
#include <types.h>
#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/PiSDF/PiSDFEdge.h"
#include "../../graphs/PiSDF/PiSDFConfigVertex.h"
#include "../ActorMachine/AMGraph.h"
#include <scheduling/Schedule/Schedule.h>
#include <grt_definitions.h>
#include "../launcher.h"
#include "../SingleActor/LRTActor.h"

extern AMGraph AMGraphTbl[MAX_NB_AM];
extern UINT32 nbAM;
extern LRTActor LRTActorTbl[MAX_SRDAG_VERTICES];
extern UINT32 nbLRTActors;

class CreateTaskMsg: public LRTMsg {
private:
	UINT32 taskID;
	UINT32 functID;
	UINT32 isAM;
//	INT32 stopAfterComplet;
//	INT32 nbFifoIn;
//	INT32 nbFifoOut;
//	INT32 FifosInID[MAX_NB_FIFO];
//	INT32 FifosOutID[MAX_NB_FIFO];

	union{
		AMGraph* 	am;
		LRTActor* 	lrtActor;
	}actor;

	/* Actor Machine */
//	INT32 initStateAM;
//	AMGraph* am;
public:
	CreateTaskMsg(){
		taskID = 0;
		functID = 0;
		isAM = 0;
	}
	CreateTaskMsg(SRDAGGraph* graph, SRDAGVertex* vertex, AMGraph* am);
	CreateTaskMsg(SRDAGGraph* graph, Schedule* schedule, int slave, AMGraph* am);
	CreateTaskMsg(SRDAGGraph* graph, BaseSchedule* schedule, int slave, launcher*);
	CreateTaskMsg(SRDAGGraph *graph, SRDAGVertex* srvertex, launcher*);
	CreateTaskMsg(PiSDFConfigVertex* vertex, AMGraph* am, INT32 stopAfterComplet = 0);

	AMGraph* getAM(){return actor.am;}
	LRTActor* getLRTActor(){return actor.lrtActor;}

	void send(int LRTID);
	int prepare(int* data, int offset);
	void prepare(int slave, launcher* launch);
	void toDot(const char* path);
//	AMGraph* getAm();
    UINT32 getIsAM() const
    {
        return isAM;
    }

    void setIsAM(UINT32 isAM)
    {
        this->isAM = isAM;
    }
};

#endif /* TASKMSG_H_ */

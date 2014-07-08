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

#include "execution.h"
#include "monitor.h"
#include <tools/Queue.h>
#include <grt_definitions.h>
#include <platform_data_queue.h>
#include <memoryAlloc.h>

#include <mpSched/mpSched.h>
#include <mpSched/baseActors.h>

static Queue<SRDAGVertex*, MAX_MASTER_ACTORS> executionQueue;
static UINT8* inputFIFOs[MAX_NB_FIFO];
static UINT8* outputFIFOs[MAX_NB_FIFO];
static UINT32 args[MAX_NB_ARGS];
static FUNCTION_TYPE functions_tbl[NB_LOCAL_FUNCTIONS];

struct param{UINT32 vxId; UINT32 value;};
static Queue<param, 50> params;

UINT32 curVertexId;

void initExecution(){
	functions_tbl[0] = config;
	functions_tbl[1] = mFilter;
	functions_tbl[2] = src;
	functions_tbl[3] = snk;
	functions_tbl[4] = setM;
	functions_tbl[5] = initSwitch;
	functions_tbl[7] = FIR;

	functions_tbl[10] = RB;
	functions_tbl[11] = broadcast;
	functions_tbl[12] = switchFct;
	functions_tbl[13] = Xplode;
	functions_tbl[14] = InitVx;
	functions_tbl[15] = EndVx;

	Monitor_init();
}

void pushExecution(SRDAGVertex* vertex){
	executionQueue.push(vertex);
}

void pushParam(UINT32 vertexID, UINT32 value){
	struct param p;
	p.vxId = vertexID;
	p.value = value;
	params.push(p);
}

BOOL popParam(UINT32* vertexID, UINT32* value){
	if(params.isEmpty())
		return FALSE;

	struct param p = params.pop();
	*vertexID = p.vxId;
	*value = p.value;
	return TRUE;
}

void execute(){
	while(!executionQueue.isEmpty()){
		SRDAGVertex* vertex = executionQueue.pop();
		int nbParams = 0;

		for(int i=0;i<vertex->getNbInputEdge(); i++){
			SRDAGEdge * edge = vertex->getInputEdge(i);
			platform_readFifo(
					edge->getFifoId(),
					edge->getFifoAddress(),
					edge->getTokenRate(),
					0);
			inputFIFOs[i] = (UINT8*)(SHARED_MEM_BASE + edge->getFifoAddress());
		}
		for(int i=0;i<vertex->getNbOutputEdge(); i++){
			outputFIFOs[i] = (UINT8*)(SHARED_MEM_BASE + vertex->getOutputEdge(i)->getFifoAddress());
		}

		switch(vertex->getType()){
		case Normal:
		case ConfigureActor:
			for(int i=0; i<vertex->getReference()->getNbParameters(); i++){
				args[nbParams++] = vertex->getParamValue(i);
			}
			break;
		case RoundBuffer:
			args[nbParams++] = vertex->getInputEdge(0)->getTokenRate();
			args[nbParams++] = vertex->getOutputEdge(0)->getTokenRate();
			break;
		case Explode:
		case Implode:
			args[nbParams++] = vertex->getNbInputEdge();
			args[nbParams++] = vertex->getNbOutputEdge();

			for(UINT32 i=0; i<vertex->getNbInputEdge(); i++){
				args[nbParams++] = vertex->getInputEdge(i)->getTokenRate();
			}
			for(UINT32 i=0; i<vertex->getNbOutputEdge(); i++){
				args[nbParams++] = vertex->getOutputEdge(i)->getTokenRate();
			}
			break;
		case Init:
			args[nbParams++] = vertex->getOutputEdge(0)->getTokenRate();
			break;
		case End:
			args[nbParams++] = vertex->getInputEdge(0)->getTokenRate();
			break;
		case Broadcast:
			nbParams = 0;
			break;
		default:
			printf("CreateTaskMsg: unknown vertex type\n");
			abort();
		}

		curVertexId = vertex->getId();

		Monitor_startTask(vertex->getId());
		functions_tbl[vertex->getFunctIx()](inputFIFOs, outputFIFOs, args);
		Monitor_endTask();


		for(int i=0;i<vertex->getNbOutputEdge(); i++){
			SRDAGEdge * edge = vertex->getOutputEdge(i);
			platform_writeFifo(
					edge->getFifoId(),
					edge->getFifoAddress(),
					edge->getTokenRate(),
					0);
		}
//		OSFreeWorkingMemory();

	}
}

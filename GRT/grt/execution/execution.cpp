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

#include "baseActors.h"

#include <graphs/SRDAG/SRDAGEdge.h>

static Queue<SRDAGVertex*, MAX_MASTER_ACTORS> executionQueue;
static UINT8* inputFIFOs[MAX_NB_FIFO];
static UINT8* outputFIFOs[MAX_NB_FIFO];
static UINT32 args[MAX_NB_ARGS];
static FUNCTION_TYPE functions_tbl[NB_LOCAL_FUNCTIONS];

struct param{UINT32 vxId; UINT32 nbParam; UINT32 value[MAX_NB_PiSDF_PARAMS];};
static Queue<param, 50> params;

UINT32 curVertexId;

void setFctPtr(int n, FUNCTION_TYPE f){
	functions_tbl[n] = f;
}

void initExecution(){
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

void pushParam(int vertexID, int nbParam, UINT32 values[MAX_NB_PiSDF_PARAMS]){
	struct param p;
	p.vxId = vertexID;
	p.nbParam = nbParam;
	for(int i=0; i<nbParam; i++){
		p.value[i] = values[i];
	}
//	p.value = value;
	params.push(p);
}

BOOL popParam(int* vertexID, int* nbParam, UINT32 values[MAX_NB_PiSDF_PARAMS]){
	if(params.isEmpty())
		return FALSE;

	struct param p = params.pop();
	*vertexID = p.vxId;
	*nbParam = p.nbParam;
	for(int i=0; i<p.nbParam; i++){
		values[i] = p.value[i];
	}
	return TRUE;
}

void execute(){
	while(!executionQueue.isEmpty()){
		SRDAGVertex* vertex = executionQueue.pop();

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
			memcpy(args, vertex->getParamArray(), vertex->getParamNb()*sizeof(int));
			break;
		case Explode:
			args[0] = vertex->getNbInputEdge();
			args[1] = vertex->getNbOutputEdge();
			args[2] = vertex->getInputEdge(0)->getTokenRate();
			for(int i=0; i<vertex->getNbOutputEdge(); i++){
				args[3+i] = vertex->getOutputEdge(i)->getTokenRate();
			}
			break;
		case Implode:
			args[0] = vertex->getNbInputEdge();
			args[1] = vertex->getNbOutputEdge();
			args[2] = vertex->getOutputEdge(0)->getTokenRate();
			for(int i=0; i<vertex->getNbInputEdge(); i++){
				args[3+i] = vertex->getInputEdge(i)->getTokenRate();
			}
			break;
			break;
		case RoundBuffer:
			args[0] = vertex->getInputEdge(0)->getTokenRate();
			args[1] = vertex->getOutputEdge(0)->getTokenRate();
			break;
		case Broadcast:
			break;
		case Init:
			args[0] = vertex->getOutputEdge(0)->getTokenRate();
			break;
		case End:
			args[0] = vertex->getInputEdge(0)->getTokenRate();
			break;
		}

		curVertexId = vertex->getId();

		Monitor_startTask(vertex);
		functions_tbl[vertex->getFctIx()](inputFIFOs, outputFIFOs, args);
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


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

#include "GraphTransfo.h"
#include "topologyMatrix.h"
#include "LinkVertices.h"
#include "AddVertices.h"
#include "ComputeBRV.h"

#include <graphs/SRDAG/SRDAGCommon.h>
#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGEdge.h>

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/PiSDF/PiSDFParam.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/PiSDF/PiSDFEdge.h>

#include <tools/Queue.h>
#include <tools/Stack.h>
#include <tools/StaticStack.h>

#include <scheduling/MemAlloc.h>
#include <scheduling/MemAlloc/DummyMemAlloc.h>
#include <scheduling/Scheduler.h>
#include <scheduling/Scheduler/ListScheduler.h>

#include <cmath>
#include <cstdlib>
#include <cstring>

#include <launcher/Launcher.h>

#define TRANSFO_STACK_SIZE 1024*1024*1024

static void initJob(transfoJob *job, SRDAGVertex *nextHierVx, Stack* stack){
	job->graph = nextHierVx->getSubGraph();

	/* Add Static and Herited parameter values */
	job->paramValues = CREATE_MUL(stack, job->graph->getNParam(), int);
	for(int paramIx=0; paramIx<job->graph->getNParam(); paramIx++){
		PiSDFParam* param = job->graph->getParam(paramIx);
		switch(param->getType()){
		case PISDF_PARAM_STATIC:
			job->paramValues[paramIx] = param->getStaticValue();
			break;
		case PISDF_PARAM_HERITED:
			job->paramValues[paramIx] = nextHierVx->getInParam(param->getParentId());
			break;
		case PISDF_PARAM_DYNAMIC:
			// Do nothing, cannot be evaluated yet
			job->paramValues[paramIx] = -1;
			break;
		}
//		printf("%s <= %d\n", param->getName(), job->paramValues[paramIx]);
	}

	/* Add edge interfaces in job */
	job->inputIfs = CREATE_MUL(stack, nextHierVx->getNInEdge(), SRDAGEdge*);
	job->outputIfs = CREATE_MUL(stack, nextHierVx->getNOutEdge(), SRDAGEdge*);

	memcpy(job->inputIfs, nextHierVx->getInEdges(), nextHierVx->getNInEdge()*sizeof(SRDAGEdge*));
	memcpy(job->outputIfs, nextHierVx->getOutEdges(), nextHierVx->getNInEdge()*sizeof(SRDAGEdge*));

}

static void freeJob(transfoJob *job, Stack* stack){
	if(job->configs != 0)
		stack->free(job->configs);

	if(job->bodies != 0){
		for(int i=0; i<job->graph->getNBody(); i++){
			if(job->bodies[i] != 0){
				stack->free(job->bodies[i]);
			}
		}
		stack->free(job->bodies);
	}

	stack->free(job->paramValues);
	stack->free(job->inputIfs);
	stack->free(job->outputIfs);
}

static SRDAGVertex* getNextHierVx(SRDAGGraph *topDag){
	for(int i=0; i<topDag->getNVertex(); i++){ // todo check executable
		SRDAGVertex* vertex = topDag->getVertex(i);
		if(vertex->isHierarchical()){
			return vertex;
		}
	}
	return 0;
}

void jit_ms(PiSDFGraph* topPisdf, Archi* archi, SpiderConfig* config){
	SRDAGGraph *topSrdag;

	/* Initialize topDag */
	if(config->createSrdag){
		topSrdag = CREATE(config->transfoStack, SRDAGGraph)(config->transfoStack);
	}else
		topSrdag = config->srdag;

	config->memAlloc->reset();

	Schedule* schedule = CREATE(config->transfoStack, Schedule)(
			archi->getNPE(), 1000, config->transfoStack);

	/* Add initial top actor */
	PiSDFVertex* root = topPisdf->getBody(0);
	if(! root->isHierarchical()){
		printf("Error top graph without subgraph\n");
		abort();
	}
	topSrdag->addVertex(root);

	Queue<transfoJob*> jobQueue(config->transfoStack);

	// Check nb of config //

	/* Look for hierrachical actor in topDag */

	do{
		SRDAGVertex* nextHierVx = getNextHierVx(topSrdag);

		/* Exit loop if no hierarchical actor found */
		if(!nextHierVx) break;

		do{
			/* Fill the transfoJob data */
			transfoJob* job = CREATE(config->transfoStack, transfoJob);
			initJob(job, nextHierVx, config->transfoStack);

			/* Remove Hierachical vertex */
			topSrdag->delVertex(nextHierVx);

			if(job->graph->getNConfig() > 0){
				/* Put CA in topDag */
				addCAVertices(topSrdag, job, config->transfoStack);

				/* Link CA in topDag */
				linkCAVertices(topSrdag, job);

				jobQueue.push(job);
			}else{
				int* brv = CREATE_MUL(config->transfoStack, job->graph->getNBody(), int);
				computeBRV(topSrdag, job, brv, config->transfoStack);

				addSRVertices(topSrdag, job, brv, config->transfoStack);

				linkSRVertices(topSrdag, job, brv);
			}

			/* Find next hierarchical vertex */
			nextHierVx = getNextHierVx(topSrdag);
		}while(nextHierVx);

		topSrdag->updateState();

//		SRDAGWrite(topDag, "topDag_ca.gv", DataRates);

		/* Schedule and launch execution */
		config->memAlloc->alloc(topSrdag);
		config->scheduler->schedule(topSrdag, schedule, archi, config->transfoStack);
		getLrt()->runUntilNoMoreJobs();

		/* Resolve params must be done by itself */
		Launcher::get()->resolveParams(archi, topSrdag);

		while(! jobQueue.isEmpty()){
			/* Pop job from queue */
			transfoJob* job = jobQueue.pop();

			/* Compute BRV */
			int* brv = CREATE_MUL(config->transfoStack, job->graph->getNBody(), int);
			computeBRV(topSrdag, job, brv, config->transfoStack);

			/* Add vertices */
			addSRVertices(topSrdag, job, brv, config->transfoStack);
//			SRDAGWrite(topDag, "topDag_add.gv", DataRates);
//			SRDAGCheck(topDag);

			/* Link vertices */
			linkSRVertices(topSrdag, job, brv);
//			SRDAGWrite(topDag, "topDag_link.gv", DataRates);
//			SRDAGCheck(topDag);

			freeJob(job, config->transfoStack);

			config->transfoStack->free(brv);
			config->transfoStack->free(job);
		}

//        printf("Finish one iter\n");
	}while(1);

	topSrdag->updateState();

	/* Schedule and launch execution */
	config->memAlloc->alloc(topSrdag);
	config->scheduler->schedule(topSrdag, schedule, archi, config->transfoStack);

	getLrt()->runUntilNoMoreJobs();

	schedule->~Schedule();
	config->transfoStack->free(schedule);

	config->transfoStack->freeAll();
}

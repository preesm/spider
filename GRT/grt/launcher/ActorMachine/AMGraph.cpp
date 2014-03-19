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

#include "AMGraph.h"
#include "AMCond.h"
#include "AMVertex.h"

#include "graphs/SRDAG/SRDAGEdge.h"
#include "graphs/SRDAG/SRDAGVertex.h"
#include "graphs/SRDAG/SRDAGGraph.h"
#include "graphs/CSDAG/CSDAGVertex.h"
#include "scheduling/Schedule/Schedule.h"

#include <cstring>
#include <cstdio>

#include <debuggingOptions.h>
#if USE_AM

AMGraph::AMGraph(){
	nbConds = nbVertices = nbActions = 0; initState = 0;
}

void AMGraph::generate(SRDAGVertex* srDagVertex) {
	SRDAGEdge* edge;
	SRDAGGraph* graph = srDagVertex->getBase();
	CondValue condValues[AM_GRAPH_MAX_COND];
	nbConds = nbVertices = nbActions = 0;

	/* Creating Input FIFO Conditions */
	for(int i=0; i<srDagVertex->getNbInputEdge(); i++){
		if(nbConds>AM_GRAPH_MAX_COND){
			printf("AMGraph: nbConds > AM_GRAPH_MAX_COND\n");
			abort();
		}
		edge = srDagVertex->getInputEdge(i);
		conds[nbConds] = AMCond(FIFO_IN, graph->getEdgeIndex(edge), edge->getTokenRate());
		nbConds++;
	}

	/* Creating Output FIFO Conditions */
	for(int i=0; i<srDagVertex->getNbOutputEdge(); i++){
		if(nbConds>AM_GRAPH_MAX_COND){
			printf("AMGraph: nbConds > AM_GRAPH_MAX_COND\n");
			abort();
		}
		edge = srDagVertex->getOutputEdge(i);
		conds[nbConds] = AMCond(FIFO_OUT, graph->getEdgeIndex(edge), edge->getTokenRate());
		nbConds++;
	}

	for(int i=0; i<nbConds; i++){
		condValues[i] = COND_X;
	}

	/* Creating Graph */
	vertices[0] = AMVertex(0, AMVxTypeState, condValues, nbConds);// Init AMVxTypeState
	nbVertices++;

	for(int i=0; i<nbConds; i++){

		vertices[nbVertices] = AMVertex(nbVertices, AMVxTypeTest, i);
		vertices[nbVertices-1].addSuc(nbVertices);

		condValues[i]=COND_false;
		vertices[nbVertices+1] = AMVertex(nbVertices+1, AMVxTypeState, condValues, nbConds);
		vertices[nbVertices].addSuc(nbVertices+1);
		vertices[nbVertices+1].addSuc(nbVertices+2);

		vertices[nbVertices+2] = AMVertex(nbVertices+2, AMVxTypeWait); // AMVxTypeWait
		vertices[nbVertices+2].addSuc(nbVertices-1);

		condValues[i]=COND_TRUE;
		vertices[nbVertices+3] = AMVertex(nbVertices+3, AMVxTypeState, condValues, nbConds);
		vertices[nbVertices].addSuc(nbVertices+3);

		nbVertices+=4;
	}

	vertices[nbVertices] = AMVertex(nbVertices, AMVxTypeExec, srDagVertex->getCsDagReference()->getFunctionIndex());// Execution AMVxTypeState
	vertices[nbVertices-1].addSuc(nbVertices);
	vertices[nbVertices].addSuc(0);
	nbVertices++;

	if(nbVertices>=AM_GRAPH_MAX_VERTEX){
		printf("AMGraph: nbVertices >= AM_GRAPH_MAX_VERTEX\n");
		abort();
	}

}


void AMGraph::generate(Schedule* schedule, UINT32 slave) {
	if(schedule->getNbVertex(slave) == 0){
		nbVertices = 1;
		nbConds = 0;
		vertices[0] = AMVertex(0, AMVxTypeState, NULL, 0);
		vertices[0].addSuc(1);
		vertices[1] = AMVertex(1, AMVxTypeWait);
		vertices[1].addSuc(0);
		return;
	}

	SRDAGGraph* graph = schedule->getVertex(0,0)->getBase();
	CondValue condValues[AM_GRAPH_MAX_COND];
	nbConds = nbVertices = nbActions = 0;

	/* Resetting conditions */
	for(int i=0; i<AM_GRAPH_MAX_COND; i++){
		condValues[i] = COND_X;
	}

	/* Creating Graph */
	vertices[0] = AMVertex(0, AMVxTypeState, condValues, nbConds);// Init AMVxTypeState
	nbVertices++;

	for(int j=0; j<schedule->getNbVertex(slave); j++){
		SRDAGVertex* srvertex = schedule->getVertex(slave, j);
		SRDAGEdge* edge;

		/* Creating Input FIFO Conditions */
		for(int i=0; i<srvertex->getNbInputEdge(); i++){
			edge = srvertex->getInputEdge(i);
			if(!schedule->isPresent(slave, edge->getSource())){
				conds[nbConds] = AMCond(FIFO_IN, graph->getEdgeIndex(edge), edge->getTokenRate());

				vertices[nbVertices] = AMVertex(nbVertices, AMVxTypeTest, nbConds);  /* AMVxTypeTest */
				vertices[nbVertices-1].addSuc(nbVertices); /* Previous AMVxTypeState -> AMVxTypeTest */

				condValues[nbConds]=COND_false;
				vertices[nbVertices+1] = AMVertex(nbVertices+1, AMVxTypeState, condValues, nbConds+1); /* AMVxTypeState false */
				vertices[nbVertices].addSuc(nbVertices+1);  /* AMVxTypeTest -> AMVxTypeState false */
				vertices[nbVertices+2] = AMVertex(nbVertices+2, AMVxTypeWait); /* AMVxTypeWait */
				vertices[nbVertices+1].addSuc(nbVertices+2);/* AMVxTypeState false -> AMVxTypeWait */
				vertices[nbVertices+2].addSuc(nbVertices-1);/* AMVxTypeWait -> Previous AMVxTypeState */

				condValues[nbConds]=COND_TRUE;
				vertices[nbVertices+3] = AMVertex(nbVertices+3, AMVxTypeState, condValues, nbConds+1); /* AMVxTypeState TRUE */
				vertices[nbVertices].addSuc(nbVertices+3); /* AMVxTypeTest -> AMVxTypeState TRUE */

				nbVertices+=4;
				nbConds++;

				if(nbConds>=AM_GRAPH_MAX_COND){
					printf("AMGraph: nbConds > AM_GRAPH_MAX_COND\n");
					abort();
				}


				if(nbVertices>=AM_GRAPH_MAX_VERTEX){
					printf("AMGraph: nbVertices > AM_GRAPH_MAX_VERTEX\n");
					abort();
				}
			}
		}

		/* Creating Output FIFO Conditions */
		for(int i=0; i<srvertex->getNbOutputEdge(); i++){
			edge = srvertex->getOutputEdge(i);
			if(!schedule->isPresent(slave, edge->getSink())){
				conds[nbConds] = AMCond(FIFO_OUT, graph->getEdgeIndex(edge), edge->getTokenRate());

				vertices[nbVertices] = AMVertex(nbVertices, AMVxTypeTest, nbConds);  /* AMVxTypeTest */
				vertices[nbVertices-1].addSuc(nbVertices); /* Previous AMVxTypeState -> AMVxTypeTest */

				condValues[nbConds]=COND_false;
				vertices[nbVertices+1] = AMVertex(nbVertices+1, AMVxTypeState, condValues, nbConds+1); /* AMVxTypeState false */
				vertices[nbVertices].addSuc(nbVertices+1);  /* AMVxTypeTest -> AMVxTypeState false */
				vertices[nbVertices+2] = AMVertex(nbVertices+2, AMVxTypeWait); /* AMVxTypeWait */
				vertices[nbVertices+1].addSuc(nbVertices+2);/* AMVxTypeState false -> AMVxTypeWait */
				vertices[nbVertices+2].addSuc(nbVertices-1);/* AMVxTypeWait -> Previous AMVxTypeState */

				condValues[nbConds]=COND_TRUE;
				vertices[nbVertices+3] = AMVertex(nbVertices+3, AMVxTypeState, condValues, nbConds+1); /* AMVxTypeState TRUE */
				vertices[nbVertices].addSuc(nbVertices+3); /* AMVxTypeTest -> AMVxTypeState TRUE */

				nbVertices+=4;
				nbConds++;


				if(nbConds>=AM_GRAPH_MAX_COND){
					printf("AMGraph: nbConds > AM_GRAPH_MAX_COND\n");
					abort();
				}


				if(nbVertices>=AM_GRAPH_MAX_VERTEX){
					printf("AMGraph: nbVertices > AM_GRAPH_MAX_VERTEX\n");
					abort();
				}
			}
		}

		AMAction* action = &actions[nbActions];
		char name[50];
		sprintf(name, "%s_%d", srvertex->getCsDagReference()->getName(), srvertex->getReferenceIndex());
		action->setName(name);
		action->setFunctionId(srvertex->getCsDagReference()->getFunctionIndex());
		for(int i=0; i<srvertex->getNbInputEdge(); i++)
//			action->addFifoIn(srvertex->getBase()->getEdgeIndex(srvertex->getInputEdge(i)));
		for(int i=0; i<srvertex->getNbOutputEdge(); i++)
//			action->addFifoOut(srvertex->getBase()->getEdgeIndex(srvertex->getOutputEdge(i)));
		if(action->getFunctionId() == 0){
			/* Explode-Implode */
			for(int i=0; i<srvertex->getNbInputEdge(); i++)
				action->addArg(srvertex->getInputEdge(i)->getTokenRate());
			for(int i=0; i<srvertex->getNbOutputEdge(); i++)
				action->addArg(srvertex->getOutputEdge(i)->getTokenRate());
		}else{
			for(int i=0; i<srvertex->getCsDagReference()->getParamNb(); i++){
				action->addArg(srvertex->getCsDagReference()->getParamValue(i));
			}
		}

		vertices[nbVertices] = AMVertex(nbVertices, AMVxTypeExec, nbActions); /* Execution AMVxTypeState */
		vertices[nbVertices-1].addSuc(nbVertices); /* Previous AMVxTypeState -> AMVxTypeExec */

		for(int i=0; i<nbConds-1; i++)
			condValues[i] = COND_X;
		vertices[nbVertices+1] = AMVertex(nbVertices+1, AMVxTypeState, condValues, nbConds); /* Post-AMVxTypeExec AMVxTypeState */
		vertices[nbVertices].addSuc(nbVertices+1); /* AMVxTypeExec -> Post-AMVxTypeExec AMVxTypeState */

		nbVertices+=2;
		nbActions++;

		if(nbVertices>=AM_GRAPH_MAX_VERTEX){
			printf("AMGraph: nbVertices > AM_GRAPH_MAX_VERTEX\n");
			abort();
		}

		if(nbActions>=AM_GRAPH_MAX_ACTIONS){
			printf("AMGraph: nbActions >= AM_GRAPH_MAX_ACTIONS\n");
			abort();
		}
	}
	vertices[nbVertices-1].addSuc(0); /* AMVxTypeExec -> Init AMVxTypeState */

	for(int i=0; i<nbVertices; i++){
		vertices[i].setNbConds(nbConds);
	}
}

void AMGraph::generate(SRDAGGraph* graph, BaseSchedule* schedule, UINT32 slave, launcher* curLaunch) {
	if(schedule->getNbVertices(slave) == 0){
		nbVertices = 1;
		nbConds = 0;
		vertices[0] = AMVertex(0, AMVxTypeState, NULL, 0);
		vertices[0].addSuc(1);
		vertices[1] = AMVertex(1, AMVxTypeWait);
		vertices[1].addSuc(0);
		return;
	}

//	SRDAGGraph* graph = schedule->getVertex(0,0)->getBase();
	CondValue condValues[AM_GRAPH_MAX_COND];
	nbConds = nbVertices = nbActions = 0;

	/* Resetting conditions */
	for(int i=0; i<AM_GRAPH_MAX_COND; i++){
		condValues[i] = COND_X;
	}

	/* Creating Graph */
	vertices[0] = AMVertex(0, AMVxTypeState, condValues, nbConds);// Init AMVxTypeState
	nbVertices++;
	this->initState = 0;

	for(UINT32 j=0; j<schedule->getNbVertices(slave); j++){
		SRDAGVertex* srvertex = (SRDAGVertex*)(schedule->getVertex(slave, j));
		SRDAGEdge* edge;

		/* Creating Input FIFO Conditions */
		for(int i=0; i<srvertex->getNbInputEdge(); i++){
			edge = srvertex->getInputEdge(i);
			SRDAGVertex* srcVertex = edge->getSource();
			if(!schedule->isPresent(slave, srcVertex->getScheduleIndex(), srcVertex)){
				conds[nbConds] = AMCond(FIFO_IN, graph->getEdgeIndex(edge), edge->getTokenRate());

				vertices[nbVertices] = AMVertex(nbVertices, AMVxTypeTest, nbConds);  /* AMVxTypeTest */
				vertices[nbVertices-1].addSuc(nbVertices); /* Previous AMVxTypeState -> AMVxTypeTest */

				condValues[nbConds]=COND_false;
				vertices[nbVertices+1] = AMVertex(nbVertices+1, AMVxTypeState, condValues, nbConds+1); /* AMVxTypeState false */
				vertices[nbVertices].addSuc(nbVertices+1);  /* AMVxTypeTest -> AMVxTypeState false */
				vertices[nbVertices+2] = AMVertex(nbVertices+2, AMVxTypeWait); /* AMVxTypeWait */
				vertices[nbVertices+1].addSuc(nbVertices+2);/* AMVxTypeState false -> AMVxTypeWait */
				vertices[nbVertices+2].addSuc(nbVertices-1);/* AMVxTypeWait -> Previous AMVxTypeState */

				condValues[nbConds]=COND_TRUE;
				vertices[nbVertices+3] = AMVertex(nbVertices+3, AMVxTypeState, condValues, nbConds+1); /* AMVxTypeState TRUE */
				vertices[nbVertices].addSuc(nbVertices+3); /* AMVxTypeTest -> AMVxTypeState TRUE */

				nbVertices+=4;
				nbConds++;

				if(nbConds>=AM_GRAPH_MAX_COND){
					printf("AMGraph: nbConds > AM_GRAPH_MAX_COND\n");
					abort();
				}


				if(nbVertices>=AM_GRAPH_MAX_VERTEX){
					printf("AMGraph: nbVertices > AM_GRAPH_MAX_VERTEX\n");
					abort();
				}
			}
		}

		/* Creating Output FIFO Conditions */
		for(int i=0; i<srvertex->getNbOutputEdge(); i++){
			edge = srvertex->getOutputEdge(i);
			SRDAGVertex* snkVertex = edge->getSink();
			if(!schedule->isPresent(slave, snkVertex->getScheduleIndex(), snkVertex)){
				conds[nbConds] = AMCond(FIFO_OUT, graph->getEdgeIndex(edge), edge->getTokenRate());

				vertices[nbVertices] = AMVertex(nbVertices, AMVxTypeTest, nbConds);  /* AMVxTypeTest */
				vertices[nbVertices-1].addSuc(nbVertices); /* Previous AMVxTypeState -> AMVxTypeTest */

				condValues[nbConds]=COND_false;
				vertices[nbVertices+1] = AMVertex(nbVertices+1, AMVxTypeState, condValues, nbConds+1); /* AMVxTypeState false */
				vertices[nbVertices].addSuc(nbVertices+1);  /* AMVxTypeTest -> AMVxTypeState false */
				vertices[nbVertices+2] = AMVertex(nbVertices+2, AMVxTypeWait); /* AMVxTypeWait */
				vertices[nbVertices+1].addSuc(nbVertices+2);/* AMVxTypeState false -> AMVxTypeWait */
				vertices[nbVertices+2].addSuc(nbVertices-1);/* AMVxTypeWait -> Previous AMVxTypeState */

				condValues[nbConds]=COND_TRUE;
				vertices[nbVertices+3] = AMVertex(nbVertices+3, AMVxTypeState, condValues, nbConds+1); /* AMVxTypeState TRUE */
				vertices[nbVertices].addSuc(nbVertices+3); /* AMVxTypeTest -> AMVxTypeState TRUE */

				nbVertices+=4;
				nbConds++;


				if(nbConds>=AM_GRAPH_MAX_COND){
					printf("AMGraph: nbConds > AM_GRAPH_MAX_COND\n");
					abort();
				}


				if(nbVertices>=AM_GRAPH_MAX_VERTEX){
					printf("AMGraph: nbVertices > AM_GRAPH_MAX_VERTEX\n");
					abort();
				}
			}
		}

		// Creating actions.
		AMAction* action = &actions[nbActions];
		char name[50];
		sprintf(name, "%s_%d", srvertex->getReference()->getName(), srvertex->getReferenceIndex());
		action->setName(name);
		action->setFunctionId(srvertex->getReference()->getFunction_index());

		// Input FIFOs.
		for(int i=0; i<srvertex->getNbInputEdge(); i++){
			SRDAGEdge* edge = srvertex->getInputEdge(i);
			action->setInFifo(curLaunch->getFIFO(graph->getEdgeIndex(edge)));

//			UINT32 size = srvertex->getInputEdge(i)->getTokenRate() * DEFAULT_FIFO_SIZE; // TODO: the size should come within the edge.)
//			action->addFifoIn(srvertex->getBase()->getEdgeIndex(srvertex->getInputEdge(i)),
//							  size,
//							  mem->alloc(size));
		}

		// Output FIFOs.
		for(int i=0; i<srvertex->getNbOutputEdge(); i++){
			SRDAGEdge* edge = srvertex->getOutputEdge(i);
			action->setOutFifo(curLaunch->getFIFO(graph->getEdgeIndex(edge)));

//			UINT32 size = srvertex->getOutputEdge(i)->getTokenRate() * DEFAULT_FIFO_SIZE; // TODO: the size should come within the edge.)
//			action->addFifoOut(srvertex->getBase()->getEdgeIndex(srvertex->getOutputEdge(i)),
//					  size,
//					  mem->alloc(size));
		}

		if(action->getFunctionId() == 0){
			/* Explode-Implode */
			for(int i=0; i<srvertex->getNbInputEdge(); i++)
				action->addArg(srvertex->getInputEdge(i)->getTokenRate());
			for(int i=0; i<srvertex->getNbOutputEdge(); i++)
				action->addArg(srvertex->getOutputEdge(i)->getTokenRate());
		}else{
			for(UINT64 i=0; i<srvertex->getReference()->getNbParameters(); i++){
				action->addArg(srvertex->getReference()->getParameter(i)->getValue());
			}
		}

		vertices[nbVertices] = AMVertex(nbVertices, AMVxTypeExec, nbActions); /* Execution AMVxTypeState */
		vertices[nbVertices-1].addSuc(nbVertices); /* Previous AMVxTypeState -> AMVxTypeExec */

		for(int i=0; i<nbConds-1; i++)
			condValues[i] = COND_X;
		vertices[nbVertices+1] = AMVertex(nbVertices+1, AMVxTypeState, condValues, nbConds); /* Post-AMVxTypeExec AMVxTypeState */
		vertices[nbVertices].addSuc(nbVertices+1); /* AMVxTypeExec -> Post-AMVxTypeExec AMVxTypeState */

		nbVertices+=2;
		nbActions++;

		if(nbVertices>=AM_GRAPH_MAX_VERTEX){
			printf("AMGraph: nbVertices > AM_GRAPH_MAX_VERTEX\n");
			abort();
		}

		if(nbActions>=AM_GRAPH_MAX_ACTIONS){
			printf("AMGraph: nbActions >= AM_GRAPH_MAX_ACTIONS\n");
			abort();
		}
	}
	vertices[nbVertices-1].addSuc(0); /* AMVxTypeExec -> Init AMVxTypeState */

	for(int i=0; i<nbVertices; i++){
		vertices[i].setNbConds(nbConds);
	}
}



void AMGraph::prepare(int slave, launcher* launch){
	launch->addUINT32ToSend(slave, nbVertices);
	launch->addUINT32ToSend(slave, nbConds);
	launch->addUINT32ToSend(slave, nbActions);
	launch->addUINT32ToSend(slave, initState);

	/* Send vertices */
	for(int i=0; i< nbVertices; i++){
		launch->addUINT32ToSend(slave, vertices[i].getType());
		launch->addUINT32ToSend(slave, vertices[i].getSucID(0));
		launch->addUINT32ToSend(slave, vertices[i].getSucID(1));

		switch(vertices[i].getType()){
		case AMVxTypeExec:
			launch->addUINT32ToSend(slave, vertices[i].getAction());
			break;
		case AMVxTypeTest:
			launch->addUINT32ToSend(slave, vertices[i].getCondID());
			break;
		case AMVxTypeWait:
		case AMVxTypeState:
			launch->addUINT32ToSend(slave, 0);
			break;
		default:
			//todo error
			break;
		}
	}

	/* Send Conditions */
	for(int i=0; i< nbConds; i++){
		launch->addUINT32ToSend(slave, conds[i].type);
		launch->addUINT32ToSend(slave, conds[i].fifo.id);
		launch->addUINT32ToSend(slave, conds[i].fifo.size);
	}

	/* Send Actions */
	for(int i=0; i< nbActions; i++){
		AMAction* action = &actions[i];
		launch->addUINT32ToSend(slave, action->getFunctionId());
		launch->addUINT32ToSend(slave, action->getNbInFifos());
		launch->addUINT32ToSend(slave, action->getNbOutFifos());
		launch->addUINT32ToSend(slave, action->getNbArgs());

		for(int j=0; j<action->getNbInFifos(); j++){
			launch->addUINT32ToSend(slave, action->getInFifo(j)->id);
//			launch->addUINT32ToSend(slave, action->getInFifo(j)->size);
			launch->addUINT32ToSend(slave, action->getInFifo(j)->addr);
		}
		for(int j=0; j<action->getNbOutFifos(); j++){
			launch->addUINT32ToSend(slave, action->getOutFifo(j)->id);
//			launch->addUINT32ToSend(slave, action->getOutFifo(j)->size);
			launch->addUINT32ToSend(slave, action->getOutFifo(j)->addr);
		}
		for(int j=0; j<action->getNbArgs(); j++)
			launch->addUINT32ToSend(slave, action->getArg(j));
	}
}




static char types[5][15]={"","doublecircle","ellipse","diamond","box"};

void AMGraph::toDot(const char* filename){
//	char name[AM_GRAPH_MAX_COND+1];
	AMVertex* vertex;
	AMCond* cond;
	AMAction* action;
	FILE * pFile;
	int i,j;
	//char directory[_MAX_PATH];
	//getcwd(directory, sizeof(directory));

	platform_fopen (filename);

	// Writing header
	platform_fprintf("digraph ActorMachine {\n");
	platform_fprintf("node [color=Black];\n");
	platform_fprintf("edge [color=Black];\n");
	platform_fprintf("rankdir=LR;\n");

	for (i=0 ; i<nbVertices; i++)
	{
		vertex = &(vertices[i]);
		switch(vertex->getType()){
		case AMVxTypeState:
			//vertex->printStateVertex(name);
			platform_fprintf("\t%d [label=\"%d\\n0\",shape=%s];\n",vertex->getId(),vertex->getId(),/*name,*/types[vertex->getType()]);
			break;
		case AMVxTypeTest:
			cond = &conds[vertex->getCondID()];
			platform_fprintf("\t%d [label=\"%d\\nF-%d\\n%s %dB\",shape=%s];\n",
					vertex->getId(),
					vertex->getId(),
					cond->fifo.id,
					(cond->type == FIFO_OUT)?("push"):("pop"),
					cond->fifo.size,
					types[vertex->getType()]);
			break;
		case AMVxTypeExec:
			action = &(actions[vertex->getAction()]);
			platform_fprintf("\t%d [label=\"%d\\nFunction F%d\\n",vertex->getId(),vertex->getId(),action->getFunctionId());
			for(j=0; j<action->getNbInFifos(); j++)
				platform_fprintf("Fin  %d\\n",action->getInFifo(j)->id);
			for(j=0; j<action->getNbOutFifos(); j++)
				platform_fprintf("Fout %d\\n",action->getOutFifo(j)->id);
			for(j=0; j<action->getNbArgs(); j++)
				platform_fprintf("Param %d\\n",action->getArg(j));
			platform_fprintf("\",shape=%s];\n",types[vertex->getType()]);
			break;
//				platform_fprintf("\t%d [label=\"%s\",shape=%s];\n",vertex->getId(),actions[vertex->getAction()].getName(),types[vertex->getType()]);
//				break;
		case AMVxTypeWait:
			platform_fprintf("\t%d [label=\"%d\\n\",shape=%s];\n",vertex->getId(),vertex->getId(),types[vertex->getType()]);
			break;
		default:
			break; // todo error
		}
	}

	for (i=0 ; i<nbVertices ; i++)
	{
		vertex = &(vertices[i]);
		switch(vertex->getNbSuc()){
		case 1:
			platform_fprintf("\t%d->%d;\n",vertex->getId(),vertex->getSucID(0));
			break;
		case 2:
			platform_fprintf("\t%d->%d[label=\"yes\"];\n",vertex->getId(),vertex->getSucID(0));
			platform_fprintf("\t%d->%d[label=\"no\"];\n",vertex->getId(),vertex->getSucID(1));
			break;
		default:
			break; // todo error
		}
	}
	platform_fprintf("}\n");

	fclose (pFile);
}

#endif

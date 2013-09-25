/*
 * AMGraph.cpp
 *
 *  Created on: Jun 17, 2013
 *      Author: jheulot
 */

#include "AMGraph.h"
#include "AMCond.h"
#include "AMVertex.h"

#include "../SRDAG/SRDAGEdge.h"
#include "../SRDAG/SRDAGVertex.h"
#include "../SRDAG/SRDAGGraph.h"
#include "../CSDAG/CSDAGVertex.h"
#include "../Schedule/Schedule.h"

#include <cstring>
#include <cstdio>

AMGraph::AMGraph(){
	nbConds = nbVertices = nbActions = 0;
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
	vertices[0] = AMVertex(0, STATE, condValues, nbConds);// Init State
	nbVertices++;

	for(int i=0; i<nbConds; i++){

		vertices[nbVertices] = AMVertex(nbVertices, TEST, i);
		vertices[nbVertices-1].addSuc(nbVertices);

		condValues[i]=COND_false;
		vertices[nbVertices+1] = AMVertex(nbVertices+1, STATE, condValues, nbConds);
		vertices[nbVertices].addSuc(nbVertices+1);
		vertices[nbVertices+1].addSuc(nbVertices+2);

		vertices[nbVertices+2] = AMVertex(nbVertices+2, WAIT); // Wait
		vertices[nbVertices+2].addSuc(nbVertices-1);

		condValues[i]=COND_TRUE;
		vertices[nbVertices+3] = AMVertex(nbVertices+3, STATE, condValues, nbConds);
		vertices[nbVertices].addSuc(nbVertices+3);

		nbVertices+=4;
	}

	vertices[nbVertices] = AMVertex(nbVertices, EXEC, srDagVertex->getCsDagReference()->getFunctionIndex());// Execution State
	vertices[nbVertices-1].addSuc(nbVertices);
	vertices[nbVertices].addSuc(0);
	nbVertices++;

	if(nbVertices>=AM_GRAPH_MAX_VERTEX){
		printf("AMGraph: nbVertices >= AM_GRAPH_MAX_VERTEX\n");
		abort();
	}

}

void AMGraph::generate(Schedule* schedule, int slave) {
	if(schedule->getNbVertex(slave) == 0){
		nbVertices = 1;
		nbConds = 0;
		vertices[0] = AMVertex(0, STATE, NULL, 0);
		vertices[0].addSuc(1);
		vertices[1] = AMVertex(1, WAIT);
		vertices[1].addSuc(0);
		return;
	}

	SRDAGGraph* graph = schedule->getVertex(0,0)->getBase();
	CondValue condValues[AM_GRAPH_MAX_COND];
	nbConds = nbVertices = nbActions = 0;

	/* Creating Output FIFO Conditions */

	for(int i=0; i<AM_GRAPH_MAX_COND; i++){
		condValues[i] = COND_X;
	}

	/* Creating Graph */
	vertices[0] = AMVertex(0, STATE, condValues, nbConds);// Init State
	nbVertices++;

	for(int j=0; j<schedule->getNbVertex(slave); j++){
		SRDAGVertex* srvertex = schedule->getVertex(slave, j);
		SRDAGEdge* edge;

		/* Creating Input FIFO Conditions */
		for(int i=0; i<srvertex->getNbInputEdge(); i++){
			edge = srvertex->getInputEdge(i);
			if(!schedule->isPresent(slave, edge->getSource())){
				conds[nbConds] = AMCond(FIFO_IN, graph->getEdgeIndex(edge), edge->getTokenRate());

				vertices[nbVertices] = AMVertex(nbVertices, TEST, nbConds);  /* Test */
				vertices[nbVertices-1].addSuc(nbVertices); /* Previous State -> Test */

				condValues[nbConds]=COND_false;
				vertices[nbVertices+1] = AMVertex(nbVertices+1, STATE, condValues, nbConds+1); /* State false */
				vertices[nbVertices].addSuc(nbVertices+1);  /* Test -> State false */
				vertices[nbVertices+2] = AMVertex(nbVertices+2, WAIT); /* Wait */
				vertices[nbVertices+1].addSuc(nbVertices+2);/* State false -> Wait */
				vertices[nbVertices+2].addSuc(nbVertices-1);/* Wait -> Previous State */

				condValues[nbConds]=COND_TRUE;
				vertices[nbVertices+3] = AMVertex(nbVertices+3, STATE, condValues, nbConds+1); /* State TRUE */
				vertices[nbVertices].addSuc(nbVertices+3); /* Test -> State TRUE */

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

		for(int i=0; i<srvertex->getNbOutputEdge(); i++){
			edge = srvertex->getOutputEdge(i);
			if(!schedule->isPresent(slave, edge->getSink())){
				conds[nbConds] = AMCond(FIFO_OUT, graph->getEdgeIndex(edge), edge->getTokenRate());

				vertices[nbVertices] = AMVertex(nbVertices, TEST, nbConds);  /* Test */
				vertices[nbVertices-1].addSuc(nbVertices); /* Previous State -> Test */

				condValues[nbConds]=COND_false;
				vertices[nbVertices+1] = AMVertex(nbVertices+1, STATE, condValues, nbConds+1); /* State false */
				vertices[nbVertices].addSuc(nbVertices+1);  /* Test -> State false */
				vertices[nbVertices+2] = AMVertex(nbVertices+2, WAIT); /* Wait */
				vertices[nbVertices+1].addSuc(nbVertices+2);/* State false -> Wait */
				vertices[nbVertices+2].addSuc(nbVertices-1);/* Wait -> Previous State */

				condValues[nbConds]=COND_TRUE;
				vertices[nbVertices+3] = AMVertex(nbVertices+3, STATE, condValues, nbConds+1); /* State TRUE */
				vertices[nbVertices].addSuc(nbVertices+3); /* Test -> State TRUE */

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
			action->addFifoIn(srvertex->getBase()->getEdgeIndex(srvertex->getInputEdge(i)));
		for(int i=0; i<srvertex->getNbOutputEdge(); i++)
			action->addFifoOut(srvertex->getBase()->getEdgeIndex(srvertex->getOutputEdge(i)));
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

		vertices[nbVertices] = AMVertex(nbVertices, EXEC, nbActions); /* Execution State */
		vertices[nbVertices-1].addSuc(nbVertices); /* Previous State -> Exec */

		for(int i=0; i<nbConds-1; i++)
			condValues[i] = COND_X;
		vertices[nbVertices+1] = AMVertex(nbVertices+1, STATE, condValues, nbConds); /* Post-Exec State */
		vertices[nbVertices].addSuc(nbVertices+1); /* Exec -> Post-Exec State */

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
	vertices[nbVertices-1].addSuc(0); /* Exec -> Init State */

	for(int i=0; i<nbVertices; i++){
		vertices[i].setNbConds(nbConds);
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

	pFile = fopen (filename,"w");
	if(pFile != NULL){
		// Writing header
		fprintf (pFile, "digraph ActorMachine {\n");
		fprintf (pFile, "node [color=Black];\n");
		fprintf (pFile, "edge [color=Black];\n");
		fprintf (pFile, "rankdir=LR;\n");

		for (i=0 ; i<nbVertices; i++)
		{
			vertex = &(vertices[i]);
			switch(vertex->getType()){
			case STATE:
				//vertex->printStateVertex(name);
				fprintf (pFile, "\t%d [label=\"%d\\n0\",shape=%s];\n",vertex->getId(),vertex->getId(),/*name,*/types[vertex->getType()]);
				break;
			case TEST:
				cond = &conds[vertex->getCondID()];
				fprintf (pFile, "\t%d [label=\"%d\\nF-%d\\n%s %dB\",shape=%s];\n",vertex->getId(),vertex->getId(),cond->fifo.id,(cond->type == FIFO_OUT)?("push"):("pop"),cond->fifo.size,types[vertex->getType()]);
				break;
			case EXEC:
				action = &(actions[vertex->getAction()]);
				fprintf (pFile, "\t%d [label=\"%d\\nFunction F%d\\n",vertex->getId(),vertex->getId(),action->getFunctionId());
				for(j=0; j<action->getNbFifoIn(); j++)
					fprintf (pFile, "Fin  %d\\n",action->getFifoIn(j));
				for(j=0; j<action->getNbFifoOut(); j++)
					fprintf (pFile, "Fout %d\\n",action->getFifoOut(j));
				for(j=0; j<action->getNbArgs(); j++)
					fprintf (pFile, "Param %d\\n",action->getArg(j));
				fprintf (pFile, "\",shape=%s];\n",types[vertex->getType()]);
				break;
//				fprintf (pFile, "\t%d [label=\"%s\",shape=%s];\n",vertex->getId(),actions[vertex->getAction()].getName(),types[vertex->getType()]);
//				break;
			case WAIT:
				fprintf (pFile, "\t%d [label=\"%d\\n\",shape=%s];\n",vertex->getId(),vertex->getId(),types[vertex->getType()]);
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
				fprintf (pFile, "\t%d->%d;\n",vertex->getId(),vertex->getSucID(0));
				break;
			case 2:
				fprintf (pFile, "\t%d->%d[label=\"yes\"];\n",vertex->getId(),vertex->getSucID(0));
				fprintf (pFile, "\t%d->%d[label=\"no\"];\n",vertex->getId(),vertex->getSucID(1));
				break;
			default:
				break; // todo error
			}
		}
		fprintf (pFile, "}\n");

		fclose (pFile);
	}else{
		printf("Cannot open %s\n", filename);
	}
}

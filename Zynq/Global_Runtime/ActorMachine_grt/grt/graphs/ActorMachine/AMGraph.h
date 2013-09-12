/*
 * AMGraph.h
 *
 *  Created on: Jun 17, 2013
 *      Author: jheulot
 */

#ifndef AMGRAPH_H_
#define AMGRAPH_H_


#include "AMCond.h"
#include "AMVertex.h"
#include "AMAction.h"
#include "../SRDAG/SRDAGVertex.h"
#include "../Schedule/Schedule.h"
#include "../../SchedulerDimensions.h"

class AMGraph {
private:
	int nbVertices;
	AMVertex vertices[AM_GRAPH_MAX_VERTEX];

	int nbConds;
	AMCond conds[AM_GRAPH_MAX_COND];

	int nbActions;
	AMAction actions[AM_GRAPH_MAX_ACTIONS];

public:
	AMGraph();
	AMGraph(SRDAGVertex* vertex);
	AMGraph(Schedule* schedule, int slave);

	int getNbConds(){return nbConds;}
	int getNbVertices(){return nbVertices;}
	int getNbActions(){return nbActions;}

	AMCond* getCond(int i){return &conds[i];}
	AMVertex* getVertex(int i){return &vertices[i];}
	AMAction* getAction(int i){return &actions[i];}

	void toDot(const char* filename);
};

#endif /* AMGRAPH_H_ */

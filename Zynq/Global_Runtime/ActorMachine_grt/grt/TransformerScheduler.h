/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef TRANSFORMERSCHEDULER_H
#define TRANSFORMERSCHEDULER_H

#include "expressionParser/XParser.h"
#include "graphs/SRDAG/SRDAGGraph.h"
#include "graphs/SRDAG/SRDAGVertex.h"
#include "graphs/SRDAG/SRDAGEdge.h"
#include "graphs/CSDAG/CSDAGVertex.h"
#include "graphs/CSDAG/CSDAGEdge.h"
#include "graphs/JobSet/JobSet.h"
#include "tools/DotWriter.h"
#include "tools/ScheduleWriter.h"
#include "transformations/CSDAGTransformer/CSDAGTransformer.h"
#include "transformations/JobTransformer/JobTransformer.h"
#include "scheduling/ListScheduler/ListScheduler.h"
#include "scheduling/FlowShopScheduler/FlowShopScheduler.h"

#define CSDAG_FILE_PATH 			"csdag.gv"
#define STRUCTURE_FILE_PATH 		"srDag.gv"
#define GANTT_FLOW_FILE_PATH 		"ganttEventsFlow.xml"
#define GANTT_LIST_FILE_PATH 		"ganttEventsList.xml"
#define MATLAB_SPEEDUPS_FILE_PATH 	"speedups.dat"
#define MATLAB_SPANS_FILE_PATH 		"spans.dat"
#define MATLAB_WORKS_FILE_PATH 		"works.dat"

int transformAndSchedule(CSDAGGraph* csDag, Architecture* archi);

#endif

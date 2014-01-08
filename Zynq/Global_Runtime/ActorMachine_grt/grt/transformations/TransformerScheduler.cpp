/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/

#include "TransformerScheduler.h"
#include "scheduling/FlowShopScheduler/FlowShopScheduler.h"
#include "scheduling/ListScheduler/ListScheduler.h"
#include <Timer.h>
#include "tools/DotWriter.h"
/**
 * Class handling the whole graph transformation and scheduling
 * 
 * @author mpelcat
 */


/**
 Generates the SRDAG graph and schedules it, 
 displaying graphs and gantt chart if DISPLAY is defined

 @param csDag: input graph
 @param srDag: output graph
 @param archi: slave architecture

 @return: the makespan
*/
int transformAndSchedule(CSDAGGraph* csDag, Architecture* archi){
	FlowShopScheduler flowShopScheduler;
	JobTransformer jobTransformer;
	ListScheduler listScheduler;
	CSDAGTransformer csDAGTransformer;

//	SRDAGVertex* scheduleList[MAX_SRDAG_VERTICES*MAX_SLAVES];

	JobSet jobSet;
	SRDAGGraph srDag;

	DotWriter dotWriter; // DOT graph writer
	ScheduleWriter schedWriter; // schedule Gantt writer

	jobSet.flush(); srDag.flush();
	listScheduler.setArchitecture(archi);
	flowShopScheduler.setArchitecture(archi);

	if(archi->getNbSlaves() < csDag->getNbVertices()){
		printf("Impossible to schedule: nbSlaves < nbCSDAGVertices\n");
		return -1;
	}

	/**
		Graph transformation and mapping
	*/
	//dotWriter.write(csDag, CSDAG_FILE_PATH, 1);

	timer.resetAndStart();
	csDag->resolveTimings(archi);
	csDAGTransformer.transform(csDag, &srDag, archi);
	timer.printAndReset("srDAG Transf.");

	jobTransformer.transform(&srDag, &jobSet, archi);
	timer.printAndReset("jobs Transf.");

	dotWriter.write(&srDag, STRUCTURE_FILE_PATH, 1, 1);

	timer.resetAndStart();
	flowShopScheduler.schedule(csDag, &jobSet);
	timer.printAndReset("Flow Sched.");
	schedWriter.write(csDag, &srDag, archi, GANTT_FLOW_FILE_PATH);

	timer.resetAndStart();
	listScheduler.schedule(csDag, &srDag);
	schedWriter.write(csDag, &srDag, archi, GANTT_LIST_FILE_PATH);
	timer.printAndReset("List Sched.");

	return 0;
}


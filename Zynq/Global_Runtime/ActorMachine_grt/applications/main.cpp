
/********************************************************************************
 * Copyright or � or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-Fran�ois Nezan, Jean-Christophe Prevotet			*
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

//#include "mpeg4_part2.h"
#include "DoubleLoop/PiSDFDoubleLoop.h"
#include <scheduling/Schedule/Schedule.h>
#include <scheduling/Scenario/Scenario.h>
#include <scheduling/ListScheduler/ListScheduler.h>
#include <transformations/PiSDFTransformer/PiSDFTransformer.h>
#include <tools/ExecutionStat.h>
#include "launcher/launcher.h"
#include <tools/DotWriter.h>
#include <tools/ScheduleWriter.h>
#include <tools/ScheduleChecker.h>

#define LAST_EXEC				1
#define PRINT_GRAPH				1
#define PiSDF_FILE_PATH			"pisdf"
#define SUB_SDF_FILE_0_PATH		"subSdf.gv"
#define SRDAG_FILE_PATH			"srDag.gv"
#define SRDAG_FIFO_ID_FILE_PATH	"srDagFifoId.gv"
#define IS_AM 					0
#define STOP					1


UINT32 PiSDFGraph::glbNbConfigVertices = 0;
UINT32 PiSDFGraph::glbNbExecConfigVertices = 0;
UINT32 PiSDFGraph::glbNbExecVertices = 0;
//BaseVertex* PiSDFGraph::ExecutableVertices[MAX_NB_VERTICES] = {NULL};
PiSDFEdge* PiSDFGraph::requiredEdges[MAX_NB_EDGES] = {NULL};
UINT32 PiSDFGraph::glbNbRequiredEdges = 0;
PiSDFIfVertex* PiSDFGraph::visitedIfs[MAX_NB_VERTICES] = {NULL};
UINT32 PiSDFGraph::glbNbVisitedIfs = 0;


static char name[MAX_VERTEX_NAME_SIZE];

Scenario 			scenario;
Architecture 		arch;
ListScheduler 		listScheduler;
ScheduleWriter 		schedWriter;
ScheduleChecker 	scheduleChecker;
BaseSchedule		schedule;
PiSDFTransformer 	transformer;
ExecutionStat 		execStat;
PiSDFGraph 			piSDF;
SDFGraph 			sdf;
SRDAGGraph 			dag;
UINT32 				prevNbConfigVertices;
launcher 			launch;
DotWriter 			dotWriter;


void createArch(Architecture* arch, int nbSlaves){
	static char tempStr[11];
	// Architecture Zynq
	arch->addSlave(0, "ARM", 0.410, 331, 0.4331, 338);
	for(int i=1; i<nbSlaves; i++){
		sprintf(tempStr,"uBlaze%02d",i);
		arch->addSlave(1, tempStr, 0.9267, 435, 0.9252, 430);
	}
}

int main(int argc, char* argv[]){
//	if(argc < 2){
//		printf("Usage: %s nbSlaves\n", argv[0]);
//		return 0;
//	}
//	int nbSlaves = atoi(argv[1]);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	int nbSlaves = 1;

	printf("Starting with %d slaves max\n", nbSlaves);

	createArch(&arch, nbSlaves);
	arch.setNbActiveSlaves(nbSlaves);
	listScheduler.setArchitecture(&arch);
	listScheduler.setScenario(&scenario);
	schedule.setNbActiveSlaves(arch.getNbActiveSlaves());


#if LAST_EXEC == 1
	launch.init(nbSlaves);
//	launch.launchWaitAck(nbSlaves);
#endif


	/*****************************
	 * Multi step algorithm for mapping/scheduling PiSDF graphs.
	 */
	// Getting the PiSDF graph.
	top(&piSDF);
	PiSDFGraph* H = &piSDF;
	SRDAGVertex* currHSrDagVx = 0;
	UINT32 lvlCntr = 0;
	while(H){
	#if PRINT_GRAPH
		// Printing the PiSDF graph.
		sprintf(name, "%s_%d.gv", PiSDF_FILE_PATH, lvlCntr);
		dotWriter.write(H, name, 1);
	#endif

		H->multiStepScheduling(&schedule, &listScheduler, &arch, &launch, &execStat, &dag, currHSrDagVx);

	#if PRINT_GRAPH
		// Printing the dag.
//		dotWriter.write(&dag, SRDAG_FILE_PATH, 1, 1);
//		dotWriter.write(&dag, SRDAG_FIFO_ID_FILE_PATH, 1, 0);
	#endif

		// Finding other hierarchical Vxs.
		H = 0;
		for (int i = 0; i < dag.getNbVertices(); i++) {
			currHSrDagVx = dag.getVertex(i);
			if((currHSrDagVx->getReference()->getType() == pisdf_vertex)&&
					(currHSrDagVx->getState() == SrVxStHierarchy)){
				if(((PiSDFVertex*)(currHSrDagVx->getReference()))->hasSubGraph(&H)){
					lvlCntr++;
					break;
				}
			}
		}
	}
/*********************////



	// Scheduling the DAG.
	listScheduler.schedule(&dag, &schedule, &arch);
	schedWriter.write(&schedule, &dag, &arch, "test.xml");

	launch.clear();

	// Creating FIFOs for executable vxs.
	// Note that some FIFOs have already been created in previous steps.
	launch.prepareFIFOsInfo(&dag, &arch);

	// Preparing tasks' informations
	launch.prepareTasksInfo(&dag, &arch, &schedule, IS_AM, &execStat);

#if LAST_EXEC == 1
	// Launching the execution on LRTs.
	launch.launch(nbSlaves);

	// Clearing the launcher.
	launch.clear();
#endif

	dag.updateExecuted();

}
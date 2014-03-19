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

//#include "mpeg4Decoder/mpeg4_part2.h"
//#include "DoubleLoop/PiSDFDoubleLoop.h"
#include "generatedC++/doubleLoop_gen.h"
#include <scheduling/Schedule/Schedule.h>
#include <scheduling/Scenario/Scenario.h>
#include <scheduling/ListScheduler/ListScheduler.h>
#include <transformations/PiSDFTransformer/PiSDFTransformer.h>
#include <tools/ExecutionStat.h>
#include <launcher/launcher.h>
#include <launcher/messages/ClearTimeMsg.h>
#include <tools/DotWriter.h>
#include <tools/ScheduleWriter.h>
#include <tools/ScheduleChecker.h>
#include <debuggingOptions.h>

#define IS_AM 					0
#define STOP					1

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
launcher 			launch;
DotWriter 			dotWriter;


void createArch(Architecture* arch, int nbSlaves){
	static char tempStr[11];
	// Architecture Zynq
//	arch->addSlave(0, "ARM", 0.410, 331, 0.4331, 338);
	// TODO: Add master "ARM"
	for(int i=0; i<nbSlaves; i++){
		sprintf(tempStr,"uBlaze%02d",i);
		arch->addSlave(1, tempStr, 0.9267, 435, 0.9252, 430);
	}
}

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("Usage: %s nbSlaves\n", argv[0]);
		return 0;
	}
	int nbSlaves = atoi(argv[1]);

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	printf("Starting with %d slaves max\n", nbSlaves);

	/*
	 * These objects should be obtained from the PREESM generator :
	 * Architecture, Scheduling policy.
	 */
	createArch(&arch, nbSlaves);
	arch.setNbActiveSlaves(nbSlaves);
	listScheduler.setArchitecture(&arch);
	listScheduler.setScenario(&scenario);
	schedule.setNbActiveSlaves(arch.getNbActiveSlaves());

	// Getting the PiSDF graph.
	top(&piSDF, &scenario);

#if EXEC == 1
	/*
	 * Initializing the queues and data containers (see launcher class) for communication with local RTs.
	 * This must be done before calling the multiStepScheduling method.
	 */
	launch.init(nbSlaves);
	ClearTimeMsg resetMsg;
	int i;
	for(i=0; i<nbSlaves; i++)
		resetMsg.send(i);
//	launch.launchWaitAck(nbSlaves);
#endif

//	while(1){
		PiSDFGraph* 	H;
		PiSDFVertex* root = (PiSDFVertex*)piSDF.getRootVertex();
		if(!root) exitWithCode(1070);
		if(!root->hasSubGraph(&H)) exitWithCode(1069);
		SRDAGVertex* 	currHSrDagVx = 0;

		UINT32 	lvlCntr = 0;
		UINT8 	stepsCntr;
		/*
		 * H contains the current PiSDF at each hierarchical level.
		 * Note that the loop stops when H is null, i.e. all levels have been treated,
		 * the graph has been completely flatten.
		 */
		while(H){
		#if PRINT_GRAPH
			// Printing the current PiSDF graph.
			sprintf(name, "%s_%d.gv", PiSDF_FILE_PATH, lvlCntr);
			dotWriter.write(H, name, 1);
		#endif

			/*
			 * Calling the multi steps scheduling method. For each hierarchical level, the method :
			 * 1. Executes the configure actors.
			 * 2. Resolves parameter depending expressions.
			 * 3. Computes the Basic Repetition Vector.
			 * 4. Converts into a DAG.
			 * 5. And merges the underlying (local) DAG with the (global) DAG of above levels.
			 * At the end, the "dag" argument will contain the complete flattened model.
			 * A final execution must be launched to complete one iteration, i.e. one complete execution of the application.
			 */
			stepsCntr = 0;
			H->multiStepScheduling(&schedule, &listScheduler, &arch, &launch, &execStat, &dag, currHSrDagVx, lvlCntr, &stepsCntr);

			/*
			 * Finding other hierarchical Vxs. Here the variable "H" gets the next hierarchical level to be flattened.
			 * Remember that when no more hierarchies are found, it marks the end of a complete execution of the model.
			 */
			H = 0;
			for (int i = 0; i < dag.getNbVertices(); i++) {
				currHSrDagVx = dag.getVertex(i);
				if((currHSrDagVx->getReference()->getType() == pisdf_vertex) && (currHSrDagVx->getState() == SrVxStHierarchy)){
					if(((PiSDFVertex*)(currHSrDagVx->getReference()))->hasSubGraph(&H)){
						lvlCntr++;
						break;
					}
				}
			}
		}


		/*
		 * Last scheduling and execution. After all hierarchical levels have been flattened,
		 * there is one more execution to do for completing one complete execution of the model.
		 */
		listScheduler.schedule(&dag, &schedule, &arch);
//		sprintf(name, "%s_%d.xml", SCHED_FILE_NAME, stepsCntr);
		schedWriter.write(&schedule, &dag, &arch, "test.xml");

		launch.clear();

		// Assigning FIFO ids to executable vxs' edges.
		launch.assignFIFOId(&dag, &arch);

		// Preparing tasks' informations
		launch.prepareTasksInfo(&dag, nbSlaves, &schedule, IS_AM, &execStat);

	#if EXEC == 1
		/*
		 * Launching the execution on LRTs. The "true" means that is the last execution
		 * of the current iteration, so the local RTs clear the tasks table and
		 * send back execution information.
		 */
		launch.launch(nbSlaves, true);
		launch.createRealTimeGantt(&arch, &dag, "Gantt.xml");
	#endif

		// Updating states. Sets all executable vxs to executed since their execution was already launched.
		dag.updateExecuted();

	#if PRINT_GRAPH
		// Printing the final dag.
		sprintf(name, "%s.gv", SRDAG_FILE_PATH);
		dotWriter.write(&dag, name, 1, 1);
		// Printing the final dag with FIFO ids.
		sprintf(name, "%s.gv", SRDAG_FIFO_ID_FILE_PATH);
		dotWriter.write(&dag, name, 1, 0);
	#endif

	printf("finished\n");
}

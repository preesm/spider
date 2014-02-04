
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
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


#define PRINT_GRAPH				1
#define PiSDF_FILE_PATH			"pisdf.gv"
#define SUB_SDF_FILE_0_PATH		"subSdf.gv"
#define SRDAG_FILE_PATH			"srDag.gv"
#define SRDAG_FIFO_ID_FILE_PATH	"srDagFifoId.gv"
#define IS_AM 					0
#define EXEC					0
#define STOP					1


UINT32 PiSDFGraph::glbNbConfigVertices = 0;
UINT32 PiSDFGraph::glbNbExecConfigVertices = 0;
UINT32 PiSDFGraph::glbNbExecVertices = 0;
//BaseVertex* PiSDFGraph::ExecutableVertices[MAX_NB_VERTICES] = {NULL};
PiSDFEdge* PiSDFGraph::requiredEdges[MAX_NB_EDGES] = {NULL};
UINT32 PiSDFGraph::glbNbRequiredEdges = 0;
PiSDFIfVertex* PiSDFGraph::visitedIfs[MAX_NB_VERTICES] = {NULL};
UINT32 PiSDFGraph::glbNbVisitedIfs = 0;

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

	int nbSlaves = 1;

	printf("Starting with %d slaves max\n", nbSlaves);

	createArch(&arch, nbSlaves);
	arch.setNbActiveSlaves(nbSlaves);
	listScheduler.setArchitecture(&arch);
	listScheduler.setScenario(&scenario);
	schedule.setNbActiveSlaves(arch.getNbActiveSlaves());
	bool initFIFOs = true;


	// Getting the PiSDF graph.
	top(&piSDF);

#if PRINT_GRAPH
	// Printing the PiSDF graph.
	DotWriter 	dotWriter;
	dotWriter.write(&piSDF, PiSDF_FILE_PATH, 1);
#endif



	piSDF.AlgoMultiStepScheduling(&schedule, &listScheduler, &arch, &launch, &execStat, &dag);




#if EXEC == 1
	launch.init(nbSlaves);
#endif


//	 TODO: all the RB Vxs must be marked as executable and counted (with GlbNbExecVertices).
//	while (!piSDF.getExecutable()){
//		piSDF.multiStepScheduling(&schedule, &listScheduler, &arch, &launch, &execStat, &dag);
//	}

	// execute DAG of pisdf Vxs.

//	do{
//		bool init = true;
//		do{
//			prevNbConfigVertices = sdf1.getNbConfigVertices();
//
//			// Finding executable vertices.
//			sdf1.reset(); // Clears the graph.
//			piSDF.findRequiredEdges();
//
//			if(piSDF.getGlbNbRequiredEdges() > 0)
//			{
//				// Evaluating expressions with resolved parameters.
//				piSDF.evaluateExpressions();
//
//				// Creating a new subgraph with required edges and vertices.
//				piSDF.createSubGraph(&sdf1);
//
//				// Linking the executable vertices.
////				piSDF.connectExecVertices(&sdf1);
//			}
//			else
//				exitWithCode(1062);
//
//		#if PRINT_GRAPH
//			// Printing the SDF sub-graph.
//			dotWriter.write(&sdf1, SUB_SDF_FILE_0_PATH, 1);
//		#endif
//
//			// Flattening subSDF graph and transforming it into DAG.
//			SRDAGGraph dag;
//			transformer.transform(&sdf1, &dag);
//
//			// Printing the DAG.
//		#if PRINT_GRAPH
//			dotWriter.write((SRDAGGraph*)&dag, SRDAG_FILE_PATH, 1, 1);
//			dotWriter.write((SRDAGGraph*)&dag, SRDAG_FIFO_ID_FILE_PATH, 1, 0);
//		#endif
//
//			// Scheduling the DAG.
//			schedule.reset();
//			listScheduler.schedule(&dag, &schedule, &arch);
//			schedWriter.write(&schedule, &dag, &arch, "test.xml");




			// Inserting round buffer vertices.
//			piSDF.insertRoundBuffers();

//			// Creating SrDAG with the configure vertices.
//			// TODO: treat delays
//			piSDF.createSrDAGConfigVertices(&dag);


//
//		#if PRINT_GRAPH
//			// Printing the SDF sub-graph.
//			dotWriter.write(&sdf1, SUB_SDF_FILE_0_PATH, 1);
//		#endif
//
//			// Transforming the SDF into DAG.
//			transformer.transform(&sdf1, &dag);

			// Printing the DAG.
		#if PRINT_GRAPH
			dotWriter.write((SRDAGGraph*)&dag, SRDAG_FILE_PATH, 1, 1);
			dotWriter.write((SRDAGGraph*)&dag, SRDAG_FIFO_ID_FILE_PATH, 1, 0);
		#endif


			// Scheduling the DAG.
			schedule.reset();
			listScheduler.schedule(&dag, &schedule, &arch);
			schedWriter.write(&schedule, &dag, &arch, "test.xml");

			// Preparing FIFOs information.
			launch.prepareFIFOsInfo(&dag);

			// Preparing tasks' informations
			launch.prepareTasksInfo(&dag, &arch, &schedule, IS_AM, &execStat);

#if EXEC == 1
			// Launching execution.
			if(initFIFOs){
				launch.launchWaitAck(nbSlaves);
				initFIFOs = false;
			}
			else{
				launch.launch(nbSlaves);
			}

			// Clearing the launcher.
			launch.clear();
#endif

//			// Resolving parameters.
//			for (UINT32 i = 0; i < piSDF.getNb_config_vertices(); i++) {
//				PiSDFConfigVertex* configVertex = piSDF.getConfig_vertex(i);
//				configVertex->setStatus(executed);
//				for (UINT32 j = 0; j < configVertex->getNbRelatedParams(); j++) {
//					PiSDFParameter* param = configVertex->getRelatedParam(j);
//					// TODO: to find out the returned value when there are several parameters.
//					if (!param->getResolved()){
//#if EXEC == 1
//						UINT32 slaveId;
//						if(schedule.findSlaveId(configVertex->getId(), configVertex, &slaveId)){
//							UINT64 value = RTQueuePop_UINT32(slaveId, RTCtrlQueue);
//							configVertex->getRelatedParam(j)->setValue(value);
//						}
//#else
//						UINT64 value = 352 * 255 / 256; // for the mpeg4 decoder application.
//
////						if(init)
////							value = 0;
////						else
////							value = 1;
//
//						configVertex->getRelatedParam(j)->setValue(value);
//#endif
//					}
//				}
//			}

//			// Resolving production/consumptions.
//			piSDF.evaluateExpressions();
//
//			// Generating SDF from PiSDF excluding the configure vertices.
//			piSDF.createSDF(&sdf);
//
//		#if PRINT_GRAPH
//			// Printing the SDF sub-graph.
//			dotWriter.write(&sdf, SUB_SDF_FILE_0_PATH, 1);
//		#endif
//
//			// Computing BRV of normal vertices.
//			transformer.computeBVR(&sdf);
//
//			// Updating the productions of the round buffer vertices.
//			sdf.updateRBProd();
//
//		#if PRINT_GRAPH
//			// Printing the SDF sub-graph.
//			dotWriter.write(&sdf, SUB_SDF_FILE_0_PATH, 1);
//		#endif




//			// Clearing intra-iteration variables, e.g. required edges.
//			piSDF.clearIntraIteration();
//
//		}while(prevNbConfigVertices < piSDF.getGlbNbConfigVertices());
//
//		piSDF.clearAfterVisit();
//	}while(!STOP);
}

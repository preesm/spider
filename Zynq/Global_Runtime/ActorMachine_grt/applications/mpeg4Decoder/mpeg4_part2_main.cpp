
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

#include "mpeg4_part2.h"
#include <scheduling/Schedule/Schedule.h>
#include <scheduling/Scenario/Scenario.h>
#include <scheduling/ListScheduler/ListScheduler.h>
#include <transformations/PiSDFTransformer/PiSDFTransformer.h>
#include <tools/ExecutionStat.h>
#include "launcher/launcher.h"
#include <tools/DotWriter.h>
#include <tools/ScheduleWriter.h>
#include <tools/ScheduleChecker.h>


#define PRINT_GRAPH			1
#define PiSDF_FILE_PATH		"pisdf.gv"
#define SUB_SDF_FILE_0_PATH	"subSdf.gv"
#define SRDAG_FILE_PATH		"srDag.gv"
#define EXEC				1


UINT32 PiSDFGraph::glbNbConfigVertices = 0;
//UINT32 PiSDFGraph::nbExecutableVertices = 0;
//BaseVertex* PiSDFGraph::ExecutableVertices[MAX_NB_VERTICES] = {NULL};

void createArch(Architecture* arch, int nbSlaves){
	static char tempStr[11];
	// Architecture Zynq
	arch->addSlave(0, "ARM", 0.410, 331, 0.4331, 338);
	for(int i=1; i<nbSlaves; i++){
		sprintf(tempStr,"uBlaze%02d",i);
		arch->addSlave(1, tempStr, 0.9267, 435, 0.9252, 430);
	}
}

void mpeg4_part2_main(int nbSlaves)
{
	// Creating the architecture
	Architecture arch;
	createArch(&arch, nbSlaves);
	arch.setNbActiveSlaves(nbSlaves);

	// Creating the graph.
	PiSDFGraph piSDF;
	create_PiSDF_mpeg_part2(&piSDF);

#if PRINT_GRAPH
	// Printing the PiSDF graph.
	DotWriter 	dotWriter;
	dotWriter.write(&piSDF, PiSDF_FILE_PATH, 1);
#endif

	// Creating the scenario.
	Scenario scenario;


	// Creating the scheduler.
	ListScheduler 	listScheduler;
	listScheduler.setArchitecture(&arch);
	listScheduler.setScenario(&scenario);

	ScheduleWriter 	schedWriter;
	ScheduleChecker scheduleChecker;

	BaseSchedule	schedule;
	schedule.setNbActiveSlaves(arch.getNbActiveSlaves());


	PiSDFTransformer transformer;

	ExecutionStat execStat;
	SDFGraph sdf1;
	UINT32 prevNbConfigVertices;
	launcher launch;
	bool initFIFOs = true;

#if EXEC == 1
	launch.init(nbSlaves);
#endif
	bool isAM = false;
	do{
		bool init = true;
		do{
			prevNbConfigVertices = sdf1.getNbConfigVertices();

			// Getting executable vertices.
			sdf1.reset(); // Clears the graph.
			piSDF.getSDFGraph(&sdf1);

			// Linking the executable vertices.
			if(sdf1.getNbVertices() > 0) piSDF.linkExecutableVertices(&sdf1);

		#if PRINT_GRAPH
			// Printing the SDF sub-graph.
			dotWriter.write(&sdf1, SUB_SDF_FILE_0_PATH, 1);
		#endif

			// Flattening subSDF graph and transforming it into DAG.
			SRDAGGraph dag;
			transformer.transform(&sdf1, &dag);

			// Printing the DAG.
		#if PRINT_GRAPH
			dotWriter.write((SRDAGGraph*)&dag, SRDAG_FILE_PATH, 1);
		#endif

			// Scheduling the DAG.
			schedule.reset();
			listScheduler.schedule(&dag, &schedule, &arch);
			schedWriter.write(&schedule, &dag, &arch, "test.xml");

			if(initFIFOs){
				// Preparing FIFOs information.
				launch.prepareFIFOsInfo(&dag);
#if EXEC == 0
				initFIFOs = false;
#endif
			}

			// Preparing tasks informations
			launch.prepareTasksInfo(&dag, &arch, &schedule, isAM, &execStat);

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

			// Resolving parameters.
			for (UINT32 i = 0; i < sdf1.getNbConfigVertices(); i++) {
				PiSDFConfigVertex* configVertex = (PiSDFConfigVertex*)(sdf1.getConfigVertex(i));
		//		UINT32 slaveId;
				for (UINT32 j = 0; j < configVertex->getNbRelatedParams(); j++) {
					PiSDFParameter* param = configVertex->getRelatedParam(j);
					if (!param->getResolved()){
		//				if(schedule.findSlaveId(configVertex->getId(), configVertex, &slaveId)){
		//					UINT64 value = RTQueuePop_UINT32(slaveId, RTCtrlQueue);
							UINT64 value;
							if(init)
								value = 0;
							else
								value = 1;

							configVertex->getRelatedParam(j)->setValue(value);
		//				}
					}
				}
			}
#endif
			init = false;
		}while(prevNbConfigVertices < piSDF.getGlbNbConfigVertices());

		piSDF.clearAfterVisit();
	}while(!isAM);
}


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
#define SUB_SDF_FILE_0_PATH	"subSdf_0.gv"
#define SUB_SDF_FILE_1_PATH	"subSdf_1.gv"
#define SRDAG_FILE_PATH		"srDag.gv"



UINT32 PiSDFGraph::nbExecutableVertices = 0;
BaseVertex* PiSDFGraph::ExecutableVertices[MAX_NB_VERTICES] = {NULL};

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

	 // Creating the launcher.
//	launcher launch(nbSlaves);

	ExecutionStat execStat;

	// TODO: Skip the loop if there is no configuration actor.
//	while(true){
	// Updating the list of executable vertices (i.e. configuration vertices).
//	piSDF.resetExecutableVertices();
//	piSDF.setExecutableVertices();

	SDFGraph sdf1;
	piSDF.getSDFGraph(&sdf1);

	// Linking the executable vertices.
	if(sdf1.getNbVertices() > 0) piSDF.linkExecutableVertices(&sdf1);

#if PRINT_GRAPH
	// Printing the SDF sub-graph.
	dotWriter.write(&sdf1, SUB_SDF_FILE_0_PATH, 1);
#endif

//	// Scheduling the executable vertices.
//	schedule.reset();
//	listScheduler.schedule(piSDF.getExecutableVertices(), piSDF.getNbExecutableVertices(), &schedule);
//	schedWriter.write(&schedule, &srDag, &arch, "test.xml");
////	scheduleChecker.checkSchedule(&srDag, &schedule, &arch);
//
//	ExecutionStat execStat;
//
//	// Preparing the execution of configuration vertices.
//	launch.reset();
//	launch.prepareConfigExec(
//			piSDF.getExecutableVertices(),
//			piSDF.getNbExecutableVertices(),
//			&arch,
//			&schedule,
//			&execStat);
//
//
//	// Launching the execution of configuration vertices.
//	launch.launch(nbSlaves);
//
//	// Resolving parameters.
//	launch.resolvePiSDFParameters(
//			piSDF.getExecutableVertices(),
//			piSDF.getNbExecutableVertices(),
//			&schedule,
//			&arch);
//	}

//	// Locating executable sub-graph (parameters have been resolved).
//	SDFGraph sdf2;
//	piSDF.resetVisitedVertices();
//	piSDF.getSDFGraph(&sdf2);
//
//
//#if PRINT_GRAPH
//	// Printing the SDF sub-graph.
//	dotWriter.write(&sdf2, SUB_SDF_FILE_1_PATH, 1);
//#endif


	// Flattening subSDF graph and transforming it into DAG.
	SRDAGGraph dag;
	PiSDFTransformer transformer;
//	CSDAGTransformer csDAGTransformer;
//	csDAGTransformer.transform(&csDag, &dag, (Architecture*)0);

	transformer.transform(&sdf1, &dag);


	// Printing the DAG.
#if PRINT_GRAPH
	dotWriter.write((SRDAGGraph*)&dag, SRDAG_FILE_PATH, 1);
#endif


	// Scheduling the DAG.
	schedule.reset();
	listScheduler.schedule(&dag, &schedule, &arch);

	schedWriter.write(&schedule, &dag, &arch, "test.xml");
//	scheduleChecker.checkSchedule(&dag, &schedule, &arch);
//
	// Creating the execution stats.

//	execStat.nbSRDAGVertices = srDag.getNbVertices();
//	execStat.nbSRDAGEdges = srDag.getNbEdges();
//	execStat.nbFunction = csDag.getNbVertices()+1;

	 // Creating the launcher.
	launcher launch(nbSlaves);

	// Preparing and launching execution.
	launch.reset();
	launch.prepare(&dag, &arch, &schedule, false, &execStat);
	launch.launch(nbSlaves);
}

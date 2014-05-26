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
//#include "SimpleLoop/PiSDFSimpleLoop.h"
//#include "MedianSobel/PiSDFMedianSobel.h"
//#include "generatedC++/doubleLoop_gen.h"
//#include "MedianSobelDL/PiSDFMedianSobelDL.h"
#include "mpSched/PiSDFMpSched.h"

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
#include <platform.h>
#include <platform_time.h>
#include <stdio.h>
#include <execution/execution.h>

#define IS_AM 					0
#define STOP					1

static char tempStr[MAX_SLAVE_NAME_SIZE];




void createArch(Architecture* arch, int nbSlaves){
	arch->reset();
	arch->addSlave(0, "Master", 0.9267, 435, 0.9252, 430);

	for(int i=1; i<nbSlaves; i++){
		UINT32 len = snprintf(tempStr, MAX_SLAVE_NAME_SIZE, "PE%02d", i);
		if(len > MAX_SLAVE_NAME_SIZE){
			exitWithCode(1073);
		}
		arch->addSlave(0, tempStr, 0.9267, 435, 0.9252, 430);
	}
}

int main(int argc, char* argv[]){

//	if(argc < 2){
//		printf("Usage: %s nbSlaves\n", argv[0]);
//		return 0;
//	}
//	int nbSlaves = atoi(argv[1]);
//
//	setvbuf(stdout, NULL, _IONBF, 0);
//	setvbuf(stderr, NULL, _IONBF, 0);
	int nbSlaves = 8;
//	static Scenario 			scenario;
	static Architecture 		arch;
	static ListScheduler 		listScheduler;
	static ExecutionStat 		execStat;
	static SRDAGGraph 			topDag;
	static PiSDFGraph 			pisdfGraphs[MAX_NB_PiSDF_GRAPHS];
	static PiSDFGraph 			*topPisdf;
	static BaseSchedule 		schedule;

	printf("Starting with %d slaves max\n", nbSlaves);
	platform_init(nbSlaves);


	createArch(&arch, nbSlaves);

	/*
	 * These objects should be obtained from the PREESM generator :
	 * Architecture, Scheduling policy.
	 */

//	int iter=1;{

	// Getting the PiSDF graph.
	topPisdf = initPisdf_mpSched(pisdfGraphs, 10, 4000);
	Launcher::init();

#if EXEC == 1
//	for(int i=0; i<nbSlaves; i++)
//		ClearTimeMsg::send(i);
	platform_time_reset();
	initExecution();
#endif
	schedule.reset();

	for(int iter=1; iter<=ITER_MAX; iter++){
		arch.setNbActiveSlaves(nbSlaves);

		listScheduler.reset();
		topDag.reset();
		Launcher::reset();
		topPisdf->resetRefs();

		listScheduler.setArchitecture(&arch);
//		listScheduler.setScenario(&scenario);


		// Add topActor to topDag
		SRDAGVertex* topActor = topDag.addVertex();
		topActor->setReference(topPisdf->getVertex(0));
		topActor->setReferenceIndex(0);
		topActor->setIterationIndex(0);


		PiSDFTransformer::multiStepScheduling(&arch, topPisdf, &listScheduler, &schedule, &topDag, &execStat);

		UINT32 time;
		do{
			time = platform_time_getValue();
		}while(time < iter*PERIOD);

	}

	printf("reporting...\n");
#if EXEC == 1
	Launcher::createRealTimeGantt(&arch, &topDag, "Gantt.xml", &execStat);
#endif

	char file[100];
	printf("time\n");
	sprintf(file,"/home/jheulot/dev/mp-sched/compa_latencies.csv");
	FILE *f = fopen(file,"w+");
	fprintf(f, "iter, latency\n");
	for(int iter=0; iter<ITER_MAX; iter++){
		fprintf(f,"%d,%d\n",iter+1, execStat.latencies[iter]);
		printf("%d: %d\n",iter+1, execStat.latencies[iter]);
	}
	fclose(f);

//#if PRINT_REAL_GANTT
//		UINT32 len = snprintf(file, MAX_FILE_NAME_SIZE, "Gantt_simulated.xml");
//		if(len > MAX_FILE_NAME_SIZE)
//			exitWithCode(1072);
//		ScheduleWriter::write(&schedule, &topDag, &arch, file);
//#endif
//#endif

//	printf("%d: EndTime %d SpeedUp %.1f\n", iter, execStat[iter].globalEndTime, execStat[1].globalEndTime/((float)execStat[iter].globalEndTime));
//	printf("Explode %d, Implode %d, RB %d, BR %d\n", execStat[iter].explodeTime, execStat[iter].implodeTime, execStat[iter].roundBufferTime, execStat[iter].broadcastTime);
//
//	for(int k=0; k<execStat[iter].nbActor; k++){
//		printf("%s %d (%d times) \n", execStat[iter].actors[k]->getName(), execStat[iter].actorTimes[k], execStat[iter].actorIterations[k]);
//	}

//	FILE* f = fopen("/home/jheulot/dev/compa_res.csv", "w+");
//	fprintf(f, "nbPEs, endTime, SpeedUp, graphTransfo, Scheduling\n");
//	for(int iter=1; iter<=ITER_MAX; iter++){
//		fprintf(f, "%d,%d,%f,%d,%d\n", iter, execStat[iter].globalEndTime, execStat[1].globalEndTime/((float)execStat[iter].globalEndTime), execStat[iter].graphTransfoTime, execStat[iter].schedulingTime);
//	}
//	fclose(f);

	printf("finished\n");
}

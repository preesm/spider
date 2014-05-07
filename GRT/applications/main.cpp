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

#define IS_AM 					0
#define STOP					1

static char tempStr[MAX_SLAVE_NAME_SIZE];




void createArch(Architecture* arch, int nbSlaves){
	arch->reset();
	for(int i=0; i<nbSlaves; i++){
		UINT32 len = snprintf(tempStr, MAX_SLAVE_NAME_SIZE, "uBlaze%02d", i);
		if(len > MAX_SLAVE_NAME_SIZE){
			exitWithCode(1073);
		}
		arch->addSlave(0, tempStr, 0.9267, 435, 0.9252, 430);
	}
}

int main(int argc, char* argv[]){
#define ITER_MAX 7

//	if(argc < 2){
//		printf("Usage: %s nbSlaves\n", argv[0]);
//		return 0;
//	}
//	int nbSlaves = atoi(argv[1]);
//
//	setvbuf(stdout, NULL, _IONBF, 0);
//	setvbuf(stderr, NULL, _IONBF, 0);
	int nbSlaves = 1;
//	static Scenario 			scenario;
	static Architecture 		arch;
	static ListScheduler 		listScheduler;
	static ExecutionStat 		execStat[ITER_MAX+1];
	static SRDAGGraph 			topDag;
	static PiSDFGraph 			pisdfGraphs[MAX_NB_PiSDF_GRAPHS];
	static PiSDFGraph 			*topPisdf;

	printf("Starting with %d slaves max\n", nbSlaves);
	platform_init(nbSlaves);


	createArch(&arch, nbSlaves);

	/*
	 * These objects should be obtained from the PREESM generator :
	 * Architecture, Scheduling policy.
	 */

	int iter=1;{
//	for(int iter=1; iter<=ITER_MAX; iter++){
		arch.setNbActiveSlaves(iter);

		listScheduler.reset();
		topDag.reset();
		Launcher::init();


		listScheduler.setArchitecture(&arch);
//		listScheduler.setScenario(&scenario);

		// Getting the PiSDF graph.
		topPisdf = initPisdf_mpSched(pisdfGraphs, 3, 16000);

		// Add topActor to topDag
		SRDAGVertex* topActor = topDag.addVertex();
		topActor->setReference(topPisdf->getRootVertex());
		topActor->setReferenceIndex(0);
		topActor->setIterationIndex(0);


	#if EXEC == 1
		for(int i=0; i<nbSlaves; i++)
			ClearTimeMsg::send(i);
		platform_time_reset();
	#endif

		PiSDFTransformer::multiStepScheduling(&arch, topPisdf, &listScheduler,/* &scenario,*/ &topDag, &(execStat[iter]));
		printf("%d: EndTime %d SpeedUp %.1f\n", iter, execStat[iter].globalEndTime, execStat[1].globalEndTime/((float)execStat[iter].globalEndTime));
		printf("Explode %d, Implode %d, RB %d, BR %d\n", execStat[iter].explodeTime, execStat[iter].implodeTime, execStat[iter].roundBufferTime, execStat[iter].broadcastTime);

		for(int k=0; k<execStat[iter].nbActor; k++){
			printf("%s %d\n", execStat[iter].actors[k]->getName(), execStat[iter].actorTimes[k]);
		}
	}

	FILE* f = fopen("/home/jheulot/dev/compa_res.csv", "w+");
	fprintf(f, "nbPEs, endTime, SpeedUp, graphTransfo, Scheduling\n");
	for(int iter=1; iter<=ITER_MAX; iter++){
		fprintf(f, "%d,%d,%f,%d,%d\n", iter, execStat[iter].globalEndTime, execStat[1].globalEndTime/((float)execStat[iter].globalEndTime), execStat[iter].graphTransfoTime, execStat[iter].schedulingTime);
	}
	fclose(f);

	printf("finished\n");
}

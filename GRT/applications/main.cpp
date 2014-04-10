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
#include "SimpleLoop/PiSDFSimpleLoop.h"
//#include "generatedC++/doubleLoop_gen.h"
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
	// Architecture Zynq
//	arch->addSlave(0, "ARM", 0.410, 331, 0.4331, 338);
	// TODO: Add master "ARM"
	for(int i=0; i<nbSlaves; i++){
		UINT32 len = snprintf(tempStr, MAX_SLAVE_NAME_SIZE, "uBlaze%02d", i);
		if(len > MAX_SLAVE_NAME_SIZE){
			exitWithCode(1073);
		}
		arch->addSlave(1, tempStr, 0.9267, 435, 0.9252, 430);
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
	int nbSlaves = 1;
	static Scenario 			scenario;
	static Architecture 		arch;
	static ListScheduler 		listScheduler;
//	static ExecutionStat 		execStat;
	static PiSDFGraph 			piSDF;
	static SRDAGGraph 			topDag;

	UINT32 endTimes[101];

	printf("Starting with %d slaves max\n", nbSlaves);
	platform_init(nbSlaves);


	/*
	 * These objects should be obtained from the PREESM generator :
	 * Architecture, Scheduling policy.
	 */
	createArch(&arch, nbSlaves);
	arch.setNbActiveSlaves(nbSlaves);

	for(int iter=1; iter<=100; iter++){
		listScheduler.reset();
		piSDF.reset();
		topDag.reset();
		clearAllGraphs();
		Launcher::init();


		listScheduler.setArchitecture(&arch);
		listScheduler.setScenario(&scenario);

		// Getting the PiSDF graph.
		top(&piSDF, &scenario, iter);

		// Add topActor to topDag
		SRDAGVertex* topActor = topDag.addVertex();
		topActor->setReference(piSDF.getRootVertex());
		topActor->setReferenceIndex(0);
		topActor->setIterationIndex(0);


	#if EXEC == 1
		for(int i=0; i<nbSlaves; i++)
			ClearTimeMsg::send(i);
		platform_time_reset();
	#endif

		endTimes[iter] = PiSDFTransformer::multiStepScheduling(&arch, &piSDF, &listScheduler, &topDag);
		printf("%d: EndTime %d\n", iter, endTimes[iter]);
	}

	FILE* f = fopen("/home/jheulot/dev/compa_times.csv", "w+");
	for(int iter=1; iter<=100; iter++){
		fprintf(f, "%d,%d\n", iter, endTimes[iter]);
	}
	fclose(f);

	printf("finished\n");
}

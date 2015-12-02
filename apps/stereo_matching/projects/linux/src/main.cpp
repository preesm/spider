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

#include <spider.h>
#include <platformLinux.h>
#include "stereo.h"

#include <stdio.h>
#include <stdlib.h>

#define SH_MEM_SIZE 0x04000000

#define PISDF_SIZE 		4	*1024*1024
#define SRDAG_SIZE 		8	*1024*1024
#define ARCHI_SIZE 		128	*1024
#define TRANSFO_SIZE 	1	*1024*1024
#define LRT_SIZE 		16	*1024

static char transfoStack[TRANSFO_SIZE];
static char srdagStack[SRDAG_SIZE];
static char pisdfStack[PISDF_SIZE];
static char archiStack[ARCHI_SIZE];
static char lrtStack[LRT_SIZE];

int main(int argc, char* argv[]){
	SpiderConfig cfg;
	ExecutionStat stat;

	cfg.memAllocType = MEMALLOC_SPECIAL_ACTOR;
	cfg.memAllocStart = (void*)0;
	cfg.memAllocSize = SH_MEM_SIZE;

	cfg.schedulerType = SCHEDULER_LIST;

	cfg.archiStack.type = STACK_STATIC;
	cfg.archiStack.name = "ArchiStack";
	cfg.archiStack.size = ARCHI_SIZE;
	cfg.archiStack.start = archiStack;

	cfg.pisdfStack.type = STACK_STATIC;
	cfg.pisdfStack.name = "PiSDFStack";
	cfg.pisdfStack.size = PISDF_SIZE;
	cfg.pisdfStack.start = pisdfStack;

	cfg.srdagStack.type = STACK_STATIC;
	cfg.srdagStack.name = "SrdagStack";
	cfg.srdagStack.size = SRDAG_SIZE;
	cfg.srdagStack.start = srdagStack;

	cfg.transfoStack.type = STACK_STATIC;
	cfg.transfoStack.name = "TransfoStack";
	cfg.transfoStack.size = TRANSFO_SIZE;
	cfg.transfoStack.start = transfoStack;

	cfg.lrtStack.type = STACK_STATIC;
	cfg.lrtStack.name = "LrtStack";
	cfg.lrtStack.size = LRT_SIZE;
	cfg.lrtStack.start = lrtStack;

	cfg.useGraphOptim = true;
	cfg.useActorPrecedence = true;

	Spider spider(cfg);

	PlatformLinux platform(1, SH_MEM_SIZE, stereo_fcts, N_FCT_STEREO);

	printf("Start\n");

//	try{
		int i=1;
//	for(int i=1; i<=1; i++){
		printf("NStep = %d\n", i);

		PiSDFGraph *topPisdf = init_stereo(platform.getArchi());

		Platform::get()->rstTime();

		spider.setGraph(topPisdf);
		spider.setArchi(platform.getArchi());
		spider.iterate();

		spider.printGantt("stereo.pgantt", "latex.tex", &stat);
		spider.printSRDAG("stereo.gv");

		printf("EndTime = %d ms\n", stat.globalEndTime/1000000);

		printf("Memory use = ");
		if(stat.memoryUsed < 1024)
			printf("\t%5.1f B", stat.memoryUsed/1.);
		else if(stat.memoryUsed < 1024*1024)
			printf("\t%5.1f KB", stat.memoryUsed/1024.);
		else if(stat.memoryUsed < 1024*1024*1024)
			printf("\t%5.1f MB", stat.memoryUsed/1024./1024.);
		else
			printf("\t%5.1f GB", stat.memoryUsed/1024./1024./1024.);
		printf("\n");

//		printf("Actors:\n");
//		for(int j=0; j<stat.nPiSDFActor; j++){
//			printf("\t%12s:", stat.actors[j]->getName());
//			for(int k=0; k<archi->getNPETypes(); k++)
//				printf("\t%d (x%d)",
//						stat.actorTimes[j][k]/stat.actorIterations[j][k],
//						stat.actorIterations[j][k]);
//			printf("\n");
//		}

		free_stereo(topPisdf);
//	}
//	}catch(const char* s){
//		printf("Exception : %s\n", s);
//	}
	printf("finished\n");

	return 0;
}

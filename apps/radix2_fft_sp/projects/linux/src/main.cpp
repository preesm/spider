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
#include "pi_top_fft.h"

#include <stdio.h>
#include <stdlib.h>

void initActors();

int main(int argc, char* argv[]){
	SpiderConfig cfg;
	ExecutionStat stat;

	initActors();

	DynStack pisdfStack("PisdfStack");
	DynStack archiStack("ArchiStack");

#define SH_MEM 0x00500000
	PlatformLinux platform(1, SH_MEM, &archiStack, top_fft_fcts, N_FCT_TOP_FFT);
	Archi* archi = platform.getArchi();

	cfg.memAllocType = MEMALLOC_SPECIAL_ACTOR;
	cfg.memAllocStart = (void*)0;
	cfg.memAllocSize = SH_MEM;

	cfg.schedulerType = SCHEDULER_LIST;

	cfg.srdagStack = {STACK_DYNAMIC, "SrdagStack", 0, 0};
	cfg.transfoStack = {STACK_DYNAMIC, "TransfoStack", 0, 0};

	spider_init(cfg);

	printf("Start\n");

//	try{
	for(int i=1; i<=1; i++){
		printf("NStep = %d\n", i);
		char ganttPath[30];
		sprintf(ganttPath, "radixFFT_%d.pgantt", i);
		char srdagPath[30];
		sprintf(srdagPath, "radixFFT_%d.gv", i);

		pisdfStack.freeAll();

		PiSDFGraph *topPisdf = init_top_fft(archi, &pisdfStack);
		topPisdf->print("topPisdf.gv");

		Platform::get()->rstTime();

		spider_launch(archi, topPisdf);
//
		spider_printGantt(archi, spider_getLastSRDAG(), ganttPath, "latex.tex", &stat);
		spider_getLastSRDAG()->print(srdagPath);

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

		printf("Actors:\n");
		for(int j=0; j<stat.nbActor; j++){
			printf("\t%12s:", stat.actors[j]->getName());
			for(int k=0; k<archi->getNPETypes(); k++)
				printf("\t%d (x%d)",
						stat.actorTimes[j][k]/stat.actorIterations[j][k],
						stat.actorIterations[j][k]);
			printf("\n");
		}

		free_top_fft(topPisdf, &pisdfStack);
	}
//	}catch(const char* s){
//		printf("Exception : %s\n", s);
//	}
	printf("finished\n");

	spider_free();

	return 0;
}

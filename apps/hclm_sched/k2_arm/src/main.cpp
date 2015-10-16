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
#include <platformK2Arm.h>
#include "top_hclm.h"
#include "tests.h"

#include <stdio.h>
#include <stdlib.h>

#define SRDAG_SIZE 		1	*1024*1024
#define TRANSFO_SIZE 	1	*1024*1024
#define PISDF_SIZE 		32	*1024
#define ARCHI_SIZE 		3	*1024

static char transfoStack[TRANSFO_SIZE];
static char srdagStack[SRDAG_SIZE];
static char pisdfStackMem[PISDF_SIZE];
static char archiStackMem[ARCHI_SIZE];

void printOptimsStats();

int main(int argc, char* argv[]){
	SpiderConfig cfg;
	ExecutionStat stat;

	StaticStack pisdfStack("PisdfStack", pisdfStackMem, PISDF_SIZE);
	StaticStack archiStack("ArchiStack", archiStackMem, ARCHI_SIZE);

#define SH_MEM 0x00500000
	PlatformK2Arm platform(4, 8, USE_MSMC, SH_MEM, &archiStack, top_hclm_fcts, N_FCT_TOP_HCLM);
	Archi* archi = platform.getArchi();

	cfg.memAllocType = MEMALLOC_SPECIAL_ACTOR;
	cfg.memAllocStart = (void*)0;
	cfg.memAllocSize = SH_MEM;

	cfg.schedulerType = SCHEDULER_LIST;

	cfg.srdagStack.type = STACK_STATIC;
	cfg.srdagStack.name = "SrdagStack";
	cfg.srdagStack.size = SRDAG_SIZE;
	cfg.srdagStack.start = srdagStack;

	cfg.transfoStack.type = STACK_STATIC;
	cfg.transfoStack.name = "TransfoStack";
	cfg.transfoStack.size = TRANSFO_SIZE;
	cfg.transfoStack.start = transfoStack;

	cfg.useGraphOptim = true;
	cfg.useActorPrecedence = true;

	spider_init(cfg);

	printf("Start\n");

//	testOptims(&pisdfStack, archi);
//
//	PiSDFGraph *topPisdf;
//	spider_setActorPrecedence(false);
//
//	pisdfStack.freeAll();
//	topPisdf = init_top_hclm_opt(
//			archi, &pisdfStack,
//			/*MNext*/	0,
//			/*MStart*/	5,
//			/*NMax*/ 	20,
//			/*NVal*/ 	5,
//			/*NbS*/		4000);
//
//	Platform::get()->rstTime();
//	spider_launch(archi, topPisdf);
//
//	spider_printGantt(archi, spider_getLastSRDAG(), "gantt_no_actPre.pgantt", "latex.tex", &stat);
//	free_top_hclm_opt(topPisdf, &pisdfStack);
//
//	spider_setActorPrecedence(true);
//
//	pisdfStack.freeAll();
//	topPisdf = init_top_hclm_opt(
//			archi, &pisdfStack,
//			/*MNext*/	0,
//			/*MStart*/	5,
//			/*NMax*/ 	20,
//			/*NVal*/ 	5,
//			/*NbS*/		4000);
//
//	Platform::get()->rstTime();
//	spider_launch(archi, topPisdf);
//
//	spider_printGantt(archi, spider_getLastSRDAG(), "gantt_actPre.pgantt", "latex.tex", &stat);
//	free_top_hclm_opt(topPisdf, &pisdfStack);

//	testSpeActor(&pisdfStack, archi);
//
//	testExtPisdf(&pisdfStack, archi);

	testOpenmp(&pisdfStack, archi);

	printf("finished\n");

	printOptimsStats();

	spider_free();

	return 0;
}

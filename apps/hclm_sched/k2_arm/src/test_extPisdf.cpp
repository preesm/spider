
#include <spider.h>
#include <stdio.h>
#include "tests.h"
#include "top_hclm.h"

#define N_M_MAX 6

void spider_setMemAllocType(MemAllocType type, int start, int size);

void testExtPisdf(Stack* pisdfStack, Archi* archi){
	ExecutionStat stat;
	FILE* fres;

#define SH_MEM 0x00500000
	spider_setMemAllocType(MEMALLOC_SPECIAL_ACTOR, 0, SH_MEM);
	spider_setGraphOptim(true);
	spider_setActorPrecedence(true);

	try{
		fres = fopen("extNotN.csv","w+");
		fprintf(fres,"N,M,nSRActors,shMem,schedTime,execTime,optimTime,mappingTime\n");

		/* Sweep N */
		for(int iter=1; iter<=N_M_MAX; iter++){
			printf("N=%d\n", iter);
			char ganttPath[30];
			sprintf(ganttPath, "noOptims_noExt_M_%d.pgantt", iter);
			char srdagPath[30];
			sprintf(srdagPath, "noOptims_noExt_M_%d.gv", iter);

			pisdfStack->freeAll();

			PiSDFGraph *topPisdf = init_top_hclm(
					archi, pisdfStack,
					/*MNext*/	0,
					/*MStart*/	iter,
					/*NMax*/ 	20,
					/*NVal*/ 	N_M_MAX,
					/*NbS*/		4000);

			Platform::get()->rstTime();
			spider_launch(archi, topPisdf);

			spider_printGantt(archi, spider_getLastSRDAG(), ganttPath, "latex.tex", &stat);
//			spider_getLastSRDAG()->print(srdagPath);

			fprintf(fres,
					"%d,%d,%d,%lu,%lu,%lu,%lu\n",
					iter,
					stat.nSRDAGActor,
					stat.memoryUsed,
					stat.schedTime,
					stat.execTime,
					stat.optimTime,
					stat.mappingTime
					);

			free_top_hclm(topPisdf, pisdfStack);
		}
		fclose(fres);


		spider_setMemAllocType(MEMALLOC_SPECIAL_ACTOR, 0, SH_MEM);

		fres = fopen("extOptN.csv","w+");
		fprintf(fres,"N,M,nSRActors,shMem,schedTime,execTime,optimTime,mappingTime\n");

		for(int iter=1; iter<=N_M_MAX; iter++){
			printf("N=%d\n", iter);
			char ganttPath[30];
			sprintf(ganttPath, "optims_noExt_M_%d.pgantt", iter);
			char srdagPath[30];
			sprintf(srdagPath, "optims_noExt_M_%d.gv", iter);

			pisdfStack->freeAll();

			PiSDFGraph *topPisdf = init_top_hclm_opt(
					archi, pisdfStack,
					/*MNext*/	0,
					/*MStart*/	iter,
					/*NMax*/ 	20,
					/*NVal*/ 	N_M_MAX,
					/*NbS*/		4000);

			Platform::get()->rstTime();
			spider_launch(archi, topPisdf);

			spider_printGantt(archi, spider_getLastSRDAG(), ganttPath, "latex.tex", &stat);
//			spider_getLastSRDAG()->print(srdagPath);

			fprintf(fres,
					"%d,%d,%d,%lu,%lu,%lu,%lu\n",
					iter,
					stat.nSRDAGActor,
					stat.memoryUsed,
					stat.schedTime,
					stat.execTime,
					stat.optimTime,
					stat.mappingTime
					);

			free_top_hclm_opt(topPisdf, pisdfStack);
		}
		fclose(fres);
	}catch(const char* s){
		printf("Exception : %s\n", s);
	}
}


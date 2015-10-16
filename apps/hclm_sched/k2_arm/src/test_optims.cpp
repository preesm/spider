
#include <spider.h>
#include <stdio.h>
#include "tests.h"
#include "top_hclm.h"

#define N_M_MAX 6

void testOptims(Stack* pisdfStack, Archi* archi){
	ExecutionStat stat;
	FILE* fres;

	try{
		spider_setGraphOptim(false);

//		/* no Optims no Ext */
//		fres = fopen("noOptims_noExt_N.csv","w+");
//		fprintf(fres,"N,M,nSRActors,shMem,schedTime,execTime,optimTime,mappingTime\n");
//		spider_setGraphOptim(false);
//
//		/* Sweep M */
//		for(int iter=1; iter<=N_M_MAX; iter++){
//			printf("N=%d\n", iter);
//			char ganttPath[30];
//			sprintf(ganttPath, "noOptims_noExt_N_%d.pgantt", iter);
//			char srdagPath[30];
//			sprintf(srdagPath, "noOptims_noExt_N_%d.gv", iter);
//
//			pisdfStack->freeAll();
//
//			PiSDFGraph *topPisdf = init_top_hclm(
//					archi, pisdfStack,
//					/*MNext*/	0,
//					/*MStart*/	N_M_MAX,
//					/*NMax*/ 	20,
//					/*NVal*/ 	iter,
//					/*NbS*/		4000);
//
//			Platform::get()->rstTime();
//			spider_launch(archi, topPisdf);
//
//			spider_printGantt(archi, spider_getLastSRDAG(), ganttPath, "latex.tex", &stat);
////			spider_getLastSRDAG()->print(srdagPath);
//
//			fprintf(fres,
//					"%d,%d,%d,%d,%lu,%lu,%lu,%lu\n",
//					iter, 10,
//					stat.nSRDAGActor,
//					stat.memoryUsed,
//					stat.schedTime,
//					stat.execTime,
//					stat.optimTime,
//					stat.mappingTime
//					);
//
//			free_top_hclm(topPisdf, pisdfStack);
//		}
//
//		fclose(fres);
		fres = fopen("noSRDAGOptims.csv","w+");
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
					"%d,%d,%d,%d,%lu,%lu,%lu,%lu\n",
					10, iter,
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


		spider_setGraphOptim(true);
//		fres = fopen("optims_noExt_N.csv","w+");
//		fprintf(fres,"N,M,nSRActors,shMem,schedTime,execTime,optimTime,mappingTime\n");
//		for(int iter=1; iter<=N_M_MAX; iter++){
//			printf("N=%d\n", iter);
//			char ganttPath[30];
//			sprintf(ganttPath, "optims_noExt_N_%d.pgantt", iter);
//			char srdagPath[30];
//			sprintf(srdagPath, "optims_noExt_N_%d.gv", iter);
//
//			pisdfStack->freeAll();
//
//			PiSDFGraph *topPisdf = init_top_hclm(
//					archi, pisdfStack,
//					/*MNext*/	0,
//					/*MStart*/	N_M_MAX,
//					/*NMax*/ 	20,
//					/*NVal*/ 	iter,
//					/*NbS*/		4000);
//
//			Platform::get()->rstTime();
//			spider_launch(archi, topPisdf);
//
//			spider_printGantt(archi, spider_getLastSRDAG(), ganttPath, "latex.tex", &stat);
////			spider_getLastSRDAG()->print(srdagPath);
//
//			fprintf(fres,
//					"%d,%d,%d,%d,%lu,%lu,%lu,%lu\n",
//					iter, 10,
//					stat.nSRDAGActor,
//					stat.memoryUsed,
//					stat.schedTime,
//					stat.execTime,
//					stat.optimTime,
//					stat.mappingTime
//					);
//
//			free_top_hclm(topPisdf, pisdfStack);
//		}
//
//		fclose(fres);
		fres = fopen("SRDAGOptims.csv","w+");
		fprintf(fres,"N,M,nSRActors,shMem,schedTime,execTime,optimTime,mappingTime\n");

		for(int iter=1; iter<=N_M_MAX; iter++){
			printf("N=%d\n", iter);
			char ganttPath[30];
			sprintf(ganttPath, "optims_noExt_M_%d.pgantt", iter);
			char srdagPath[30];
			sprintf(srdagPath, "optims_noExt_M_%d.gv", iter);

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
					"%d,%d,%d,%d,%lu,%lu,%lu,%lu\n",
					10, iter,
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
	}catch(const char* s){
		printf("Exception : %s\n", s);
	}
}


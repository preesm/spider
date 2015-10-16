
#include <spider.h>
#include <stdio.h>
#include "tests.h"
#include "top_hclm.h"

void testOpenmp(Stack* pisdfStack, Archi* archi){
	ExecutionStat stat;
	char ganttPath[30];
	char ganttDatPath[30];
	FILE* fres;

	try{
		fres = fopen("omp/spider_homo.csv","w+");
		fprintf(fres,"N,execTime,globalEndTime\n");

		/* Homogeneous N */
		for(int iter=1; iter<=17; iter++){
			printf("N=%d\n", iter);
			sprintf(ganttPath, "omp/spider_homo_%d.pgantt", iter);
			sprintf(ganttDatPath, "omp/spider_homo_%d.dat", iter);

			pisdfStack->freeAll();

			PiSDFGraph *topPisdf = init_top_hclm_opt(
					archi, pisdfStack,
					/*MNext*/	0,
					/*MStart*/	12,
					/*NMax*/ 	20,
					/*NVal*/ 	iter,
					/*NbS*/		4000);

			Platform::get()->rstTime();
			spider_launch(archi, topPisdf);

			spider_printGantt(
					archi, spider_getLastSRDAG(),
					ganttPath,
					ganttDatPath,
					&stat);

			fprintf(fres,
					"%d,%lu,%lu\n",
					iter,
					stat.execTime,
					stat.globalEndTime
					);

			free_top_hclm_opt(topPisdf, pisdfStack);
		}
		fclose(fres);

		fres = fopen("omp/spider_inc.csv","w+");
		fprintf(fres,"N,execTime,globalEndTime\n");

		/* Homogeneous N */
		for(int iter=1; iter<=17; iter++){
			printf("N=%d\n", iter);
			sprintf(ganttPath, "omp/spider_inc_%d.pgantt", iter);
			sprintf(ganttDatPath, "omp/spider_inc_%d.dat", iter);

			pisdfStack->freeAll();

			PiSDFGraph *topPisdf = init_top_hclm_opt(
					archi, pisdfStack,
					/*MNext*/	1,
					/*MStart*/	1,
					/*NMax*/ 	20,
					/*NVal*/ 	iter,
					/*NbS*/		4000);

			Platform::get()->rstTime();
			spider_launch(archi, topPisdf);

			spider_printGantt(
					archi, spider_getLastSRDAG(),
					ganttPath,
					ganttDatPath,
					&stat);

			fprintf(fres,
					"%d,%lu,%lu\n",
					iter,
					stat.execTime,
					stat.globalEndTime
					);

			free_top_hclm_opt(topPisdf, pisdfStack);
		}
		fclose(fres);

		fres = fopen("omp/spider_dec.csv","w+");
		fprintf(fres,"N,execTime,globalEndTime\n");

		/* Homogeneous N */
		for(int iter=1; iter<=17; iter++){
			printf("N=%d\n", iter);
			sprintf(ganttPath, "omp/spider_dec_%d.pgantt", iter);
			sprintf(ganttDatPath, "omp/spider_dec_%d.dat", iter);

			pisdfStack->freeAll();

			PiSDFGraph *topPisdf = init_top_hclm_opt(
					archi, pisdfStack,
					/*MNext*/	-1,
					/*MStart*/	iter,
					/*NMax*/ 	20,
					/*NVal*/ 	iter,
					/*NbS*/		4000);

			Platform::get()->rstTime();
			spider_launch(archi, topPisdf);

			spider_printGantt(
					archi, spider_getLastSRDAG(),
					ganttPath,
					ganttDatPath,
					&stat);

			fprintf(fres,
					"%d,%lu,%lu\n",
					iter,
					stat.execTime,
					stat.globalEndTime
					);

			free_top_hclm_opt(topPisdf, pisdfStack);
		}
		fclose(fres);
	}catch(const char* s){
		printf("Exception : %s\n", s);
	}
}


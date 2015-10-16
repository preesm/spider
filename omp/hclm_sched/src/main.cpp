//#include <ti/omp/omp.h>

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "actors.h"
#include "ompMonitor.h"
#include "hclm_sched.h"
#include "platform.h"


#define CHECK 0

#if DSP
	#include "platformK2Dsp.h"
#else
	#include "platformLinux.h"
#endif

int main(){
	#if DSP
		PlatformK2Dsp platform(0);
	#else
		PlatformLinux platform(0);
	#endif


	FILE* fres;
	printf("Start\n");
	try{
		fres = fopen("omp/omp_homo.csv", "w+");
		int iter = 1;
		for(iter=1; iter<=17; iter++){
			printf("homo %d\n", iter);
			OmpMonitor monitor(0);
			hclm_sched(
					/*MNext*/ 	0,
					/*MStart*/ 	12,
					/*N*/ 		iter,
					/*NbS*/ 	4000,
					&monitor);
			monitor.saveData(iter, "homo");
			fprintf(fres, "%d,%d\n", iter, monitor.getEndTime());
		}
		fclose(fres);

		fres = fopen("omp/omp_inc.csv", "w+");
		for(iter=1; iter<=17; iter++){
			printf("inc %d\n", iter);
			OmpMonitor monitor(0);
			hclm_sched(
					/*MNext*/ 	1,
					/*MStart*/ 	1,
					/*N*/ 		iter,
					/*NbS*/ 	4000,
					&monitor);
			monitor.saveData(iter, "inc");
			fprintf(fres, "%d,%d\n", iter, monitor.getEndTime());
		}
		fclose(fres);

		fres = fopen("omp/omp_dec.csv", "w+");
		for(iter=1; iter<=17; iter++){
			printf("dec %d\n", iter);
			OmpMonitor monitor(0);
			hclm_sched(
					/*MNext*/ 	-1,
					/*MStart*/ 	iter,
					/*N*/ 		iter,
					/*NbS*/ 	4000,
					&monitor);
			monitor.saveData(iter, "dec");
			fprintf(fres, "%d,%d\n", iter, monitor.getEndTime());
		}
		fclose(fres);

	}catch(char const* ex){
		printf("Error: %s\n",ex);
	}
	printf("End\n");

	return 0;
}

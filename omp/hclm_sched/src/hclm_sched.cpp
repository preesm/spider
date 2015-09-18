//#include <ti/omp/omp.h>

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "type.h"
#include "platform.h"
#include "actors.h"
#include "ompMonitor.h"

#define CHECK 1

#define NVAL 15
#define NBSAMPLES 4000

static float temp[2*NBSAMPLES];

#if CHECK
	static float input[NVAL*NBSAMPLES];
	static float output[NVAL*NBSAMPLES];
#endif

void hclm_sched(Param MNext, Param MStart, Param N, Param NbS, OmpMonitor* monitor){
#if CHECK
	src(NbS, N, input);
#endif

	Platform::get()->rstTime();

	char M[N];
	for(int i=0; i<N; i++){
		M[i] = MStart + i*MNext;
	}

	int j, i;
	int n= N;

//	#pragma omp parallel for private(j,temp) firstprivate(input) shared(output) schedule(dynamic)
	#pragma omp parallel for private(j,temp) schedule(dynamic)
	for(i=0; i<n; i++){
		float *int_in, *int_out;
		int_in 	= input+i*NbS;
		int_out = temp;

		for(j=0; j<M[i]; j++){
			int task = monitor->startTask(i*100+j, omp_get_thread_num());

			if(j == M[i]-1)
				int_out = output + i*NbS;

			FIR(NbS, 0, int_in, int_out);

			int_in = temp + (j%2)*NbS;
			int_out = temp + ((j+1)%2)*NbS;;

			monitor->endTask(task);

		}
	}

	monitor->endApp();

#if CHECK
	snk(NbS, N, output, M);
#endif

}

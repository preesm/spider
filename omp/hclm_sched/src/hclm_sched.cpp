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

#define NVAL 20
#define NBSAMPLES 4000

static float temp[2*NBSAMPLES];

#if CHECK
	static float input[NBSAMPLES];
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

	for(int i=0; i<N; i++){
		memcpy(temp, input, NbS*sizeof(float));
	}

	int j;


	#pragma omp parallel for private(j,temp) firstprivate(input) shared(output) schedule(dynamic)
	for(int i=0; i<N; i++){
		float *int_in, *int_out;
		int_in 	= input;
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

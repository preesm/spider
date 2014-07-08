//#include <ti/omp/omp.h>
#include <xdc/runtime/Timestamp.h>

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ti/omp/omp.h>

#include <ti/csl/csl_xmcAux.h>
#include <ti/csl/csl_cacheAux.h>

#include "platform_time.h"
#include "actors.h"
#include "lrt_monitor.h"

//#define NVAL	6
//#define MMAX 	5
//#define NBITER 	5
//
//static UINT8 nValues[5][6] = {
//		{3,3,3,3,3,3},
//		{2,3,3,3,3,4},
//		{1,2,3,3,4,5},
//		{1,1,2,4,5,5},
//		{1,1,1,5,5,5},
//	};

#define NVAL	15
#define MMAX 	12
#define NBITER 	10

#define PERIOD 600000

//static UINT8 nValues[10][10] = {
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{5,6,6,6,6, 6, 6, 6, 6, 7},
//		{4,5,6,6,6, 6, 6, 6, 7, 8},
//		{3,4,5,6,6, 6, 6, 7, 8, 9},
//		{2,3,4,5,6, 6, 7, 8, 9,10},
//		{1,2,3,4,5, 7, 8, 9,10,11},
//		{1,1,2,3,4, 8, 9,10,11,11},
//		{1,1,1,2,3, 9,10,11,11,11},
//		{1,1,1,1,2,10,11,11,11,11},
//		{1,1,1,1,1,11,11,11,11,11},
//	};
//static UINT8 nValues[10][10] = {
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//		{6,6,6,6,6, 6, 6, 6, 6, 6},
//	};
#define MVALS	nValues

#define NBSAMPLES 4000

#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a<b)?a:b)

#define CHECK 0

UINT8 CACHE_getCaching  (UINT8 mar){
	return CSL_FEXT (hCache->MAR[mar], CGEM_MAR0_PC);
}

UINT8 CACHE_getPrefetch  (UINT8 mar){
	return CSL_FEXT (hCache->MAR[mar], CGEM_MAR0_PFX);
}

void main(){
	FILE* f;
	int i,j;//,pi;//,pj;
	int k, my_k;
//	UINT32 endTime;
//	int my_i;
//	int iter=0;
//	UINT32 times[11];
	static UINT32 core_endTime[15][2];
	static UINT32 core_schedTime[15][2];
	static UINT32 latency[NBITER];
	static UINT32 time[17];

#if CHECK
	static float input[NBSAMPLES];
	static float output_test[NBSAMPLES];
	static char file[100];
	static float output[NVAL][NBSAMPLES];
#endif


	static float temp[2*NVAL*NBSAMPLES];

#define tempVal(a,b,c) (temp+a*NVAL*NBSAMPLES + b*NBSAMPLES + c)
#define tempArray(a,b) 	(temp+a*NVAL*NBSAMPLES + b*NBSAMPLES)
#define tempDArray (a) 	(temp+a*NVAL*NBSAMPLES)



#if CHECK
	f = fopen("/home/jheulot/dev/mp-sched/input.dat", "rb");
	fread(input, sizeof(float), NBSAMPLES, f);
	fclose(f);
#endif

//	#pragma omp parallel for
//	for(i=0;i<11;i++){
////		times[i]=0;
//	}

	int iter;

//	for(i=0;i<10;i++){
//		printf("Vals %d : ",i);
//		for(j=0;j<10;j++){
//			printf("%d,",MVALS[i][j]);
//		}
//		printf("\n");
//	}

	// Increasing N
//	platform_time_init();
//
//#define MVAL 8
//#define MAX_N 12
//
//	for(iter=1; iter<=MAX_N; iter++){
//		int N = iter;
//		printf("Start %d\n",iter);
//		Monitor_init();
//		platform_time_reset();
//
//		#pragma omp parallel for private(j) schedule(dynamic)
//		for(i=0; i<N; i++){
//			float *int_in, *int_out;
//			int_in 	= tempArray(0,i);
//			int_out = tempArray(1,i);
//
//			for(j=0; j<MVAL; j++){
//				UINT32 task = Monitor_startTask(i*100+j, omp_get_thread_num());
//
//				fir(int_in,int_out,NBSAMPLES);
//
//				float *tmp = int_in;
//				int_in = int_out;
//				int_out = tmp;
//
//				Monitor_endTask(task);
//			}
//		}
//		Monitor_setEndTime();
//
//		UINT32 schedTime;
//		time[iter-1] = Monitor_printData(iter,"Nvar", &schedTime);
//	}
//	printf("End\n");
//
//	char file[40];
//	printf("time\n");
//	sprintf(file,"/home/jheulot/dev/mp-sched/ederc/openMP_nVar.csv");
//	f = fopen(file,"w+");
//	fprintf(f, "iter, latency\n");
//	for(iter=0; iter<MAX_N; iter++){
//		fprintf(f,"%d,%d\n",iter+1, time[iter]);
//		printf("%d: %d\n",iter+1, time[iter]);
//	}
//	fclose(f);

	/* MultiIt */
	platform_time_init();
	Monitor_init();
	printf("Start Stage %d\n",iter);
	platform_time_reset();

	#define MVAL 8
	#define MAX_N 12

	for(iter=0; iter<2; iter++){

#if EXEC
		for(i=0; i<NVAL; i++){
			memcpy(temp[0][i], input, NBSAMPLES*sizeof(float));
		}
#endif

//		printf("Start Stage %d\n",iter);
//		platform_time_reset();

		#pragma omp parallel for private(j) schedule(dynamic)
		for(i=0; i<8; i++){
			float *int_in, *int_out;
			int_in 	= tempArray(0,i);
			int_out = tempArray(1,i);

			for(j=0; j<8-i; j++){
				UINT32 task = Monitor_startTask(i*100+j, omp_get_thread_num());

				fir(int_in,int_out,NBSAMPLES);

				float *tmp = int_in;
				int_in = int_out;
				int_out = tmp;

				Monitor_endTask(task);
			}

		}
//		Monitor_setEndTime();
//		printf("End Stage\n");

		latency[iter] = platform_time_getValue() - iter*PERIOD;

		UINT32 time;
		do{
			time = platform_time_getValue();
		}while(time < (iter+1)*PERIOD);

//		core_endTime[iter][0] = Monitor_printData(iter,"stage", &(core_schedTime[iter][0]));
	}
	Monitor_setEndTime();
	printf("End Stage\n");

	core_endTime[iter][0] = Monitor_printData(iter,"", &(core_schedTime[iter][0]));

	char file[40];
	printf("time\n");
	sprintf(file,"/home/jheulot/dev/mp-sched/ederc/openMP_latencies.csv");
	f = fopen(file,"w+");
	fprintf(f, "iter, latency\n");
	for(iter=0; iter<10; iter++){
		fprintf(f,"%d,%d\n",iter+1, latency[iter]);
		printf("%d: %d\n",iter+1, latency[iter]);
	}
	fclose(f);

//	for(iter=0; iter<NBITER; iter++){
//		platform_time_init();
//		Monitor_init();
//
//#if EXEC
//		for(i=0; i<NVAL; i++){
//			memcpy(temp[0][i], input, NBSAMPLES*sizeof(float));
//		}
//#endif
//
//
//		printf("Start Pipe %d\n",iter);
//		platform_time_reset();
//
//		for(j=0; j<MMAX; j++){
//
//			printf("=== j = %d (0x%x) ===\n", j, &j);
//
//			#pragma omp parallel for shared(j) schedule(dynamic)
//			for(i=0; i<NVAL; i++){
//
//				float *int_in, *int_out;
////				int_in 	= temp[i%2][j];
////				int_out = temp[(i+1)%2][j];
//				int_in 	= tempArray(0,i);
//				int_out = tempArray(1,i);
//
//				printf("j = %d (0x%x) \n", j, &j);
//
//				if(j < MVALS[iter][i]){
//
//					UINT32 task = Monitor_startTask(i*100+j, omp_get_thread_num());
//
//#if EXEC
//					if(i == MVALS[iter][j]-1){
//						fir(int_in,output[j],NBSAMPLES);
//					}else{
//						fir(int_in,int_out,NBSAMPLES);
//					}
//#else
//					fir(int_in,int_out,NBSAMPLES);
//#endif
//
//					Monitor_endTask(task);
//				}
//			}
//		}
//
//		Monitor_setEndTime();
//		printf("End Pipe\n");
//
//		core_endTime[iter][1] = Monitor_printData(iter,"pipe", &(core_schedTime[iter][1]));
//	}

#if CHECK
	for(i=0; i<NVAL; i++){
		sprintf(file, "/home/jheulot/dev/mp-sched/output_%d_%d.dat", NBSAMPLES, MVALS[i]);
		f = fopen(file, "rb");
		fread(output_test, sizeof(float), NBSAMPLES, f);
		fclose(f);

		for(j=0; j<NBSAMPLES; j++){
			if(output_test[j] != output[i][j]){
				printf("Error at sample %d for stage %d : get %f instead of %f\n", j,i, output[i][j], output_test[j]);
			}
		}
		printf("Verification done for stage %d\n",i);
	}
#endif

//	char file[40];
//	printf("time\n");
//	sprintf(file,"/home/jheulot/dev/mp-sched/openMP_times.csv");
//	f = fopen(file,"w+");
//	fprintf(f, "nbCores, execSL, speedUpSL, schedSL, execDL, speedupDL, schedDL\n");
//	for(iter=0; iter<10; iter++){
////		fprintf(f,"%d,%d,%f,%d,%d,%f,%d\n",iter, core_endTime[iter][SL],((float)core_endTime[1][SL])/core_endTime[iter][SL], core_schedTime[iter][SL],
////		                                   core_endTime[iter][DL],((float)core_endTime[1][DL])/core_endTime[iter][DL], core_schedTime[iter][DL]);
//		printf("%d: %d : %d\n",iter, core_endTime[iter][0], core_endTime[iter][1]);
//	}
//	fclose(f);
}

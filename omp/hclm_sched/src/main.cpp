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

	printf("Start\n");
	try{
		OmpMonitor monitor(0);
		hclm_sched(1, 6, 9, 4000, &monitor);
		monitor.saveData(1, "test");
	}catch(char const* ex){
		printf("Error: %s\n",ex);
	}
	printf("End\n");

	return 0;
}

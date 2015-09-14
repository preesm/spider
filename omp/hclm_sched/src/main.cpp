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
		Platform* platform = new PlatformK2Dsp();
	#else
		Platform* platform = new PlatformLinux();
	#endif

	try{
		OmpMonitor* monitor = new OmpMonitor();
		hclm_sched(0, 7, 7, 4000, monitor);
		monitor->saveData(1, "test");
	}catch(char const* ex){
		printf("Error: %s\n",ex);
	}

	return 0;
}

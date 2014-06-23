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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lrt_monitor.h"
#include "platform_types.h"
#include "platform_time.h"

typedef struct{
	UINT32 vertexID;
	UINT32 core;
	UINT32 start;
	UINT32 end;
} taskTime;

static taskTime taskTimes[800];
static UINT32 nbTaskTime;
static UINT32 endTime;

void Monitor_init(){
	nbTaskTime = 0;
}

void Monitor_setEndTime(){
	endTime = platform_time_getValue();
}

UINT32 Monitor_startTask(UINT32 vertexID, UINT32 core){
	UINT32 result;
#pragma omp critical
	{
		result = nbTaskTime++;
	}
	taskTimes[result].vertexID = vertexID;
	taskTimes[result].core = core;
	taskTimes[result].start = platform_time_getValue();
	return result;
}

void Monitor_endTask(UINT32 task){
	taskTimes[task].end = platform_time_getValue();
}

void regenerateColor(int refInd, char* color){
	int ired = (refInd & 0x3)*50 + 100;
	int igreen = ((refInd >> 2) & 0x3)*50 + 100;
	int iblue = ((refInd >> 4) & 0x3)*50 + 100;

	sprintf(color,"#%02x%02x%02x",ired, igreen, iblue);
}

static struct {UINT32 start; UINT32 end;} schedulingTimes[100];
static UINT32 nbSchedulingTimes;

void removeRange(UINT32 start, UINT32 end){
	int i;
	for(i=0; i<nbSchedulingTimes; i++){
		if(start > schedulingTimes[i].start && start < schedulingTimes[i].end){
			if(end >= schedulingTimes[i].end){
				schedulingTimes[i].end = start;
			}else{
				schedulingTimes[nbSchedulingTimes].start = end;
				schedulingTimes[nbSchedulingTimes].end = schedulingTimes[i].end;
				nbSchedulingTimes++;

				schedulingTimes[i].end = start;
			}
		}else if(end > schedulingTimes[i].start && end < schedulingTimes[i].end ){
			schedulingTimes[i].start = end;
		}
	}
}

UINT32 Monitor_printData(int i, char* type, UINT32* schedTime){
	UINT32 task;
	UINT32 globalStartTime = 0;
	UINT32 globalEndTime = 0;
	UINT32 sumMedian = 0;
	UINT32 sumSobel = 0;

	nbSchedulingTimes=1;
	schedulingTimes[0].start = 0;
	schedulingTimes[0].end = endTime;

	// Creating the Gantt with real times.
	char file[100];
	char name[30];
	char color[10];
	sprintf(file, "/home/jheulot/dev/mp-sched/ederc/gantt_%d_%s.xml", i, type);
	FILE* f = fopen(file, "w+");

	sprintf(file, "/home/jheulot/dev/mp-sched/ederc/gantt_%d_%s_latex", i, type);
	FILE* flatex = fopen(file, "w+");

	if(f == NULL){
		printf("failed open file %s\n", file);
		return 0;
	}

	// Writing header
	fprintf(f,"<data>\n");
	fprintf(flatex,"{");

	// Writing execution data for each slave.
	for(task=0; task<nbTaskTime; task++){
//		if(taskTimes[task].vertexID >= 10000){
//			sprintf(name, "median_%d_%d", (taskTimes[task].vertexID-10000)/100, taskTimes[task].vertexID%100);
//			sumMedian += taskTimes[task].end - taskTimes[task].start;
//		}else{
//			sprintf(name, "sobel_%d", taskTimes[task].vertexID);
//			sumSobel += taskTimes[task].end - taskTimes[task].start;
//		}
		sprintf(name, "fir_%d", taskTimes[task].vertexID);
		sumSobel += taskTimes[task].end - taskTimes[task].start;

		fprintf(f,"\t<event\n");
		fprintf(f,"\t\tstart=\"%u\"\n", taskTimes[task].start);
		fprintf(f,"\t\tend=\"%u\"\n",	taskTimes[task].end);
		fprintf(f,"\t\ttitle=\"%s\"\n", name);
		fprintf(f,"\t\tmapping=\"%d\"\n", taskTimes[task].core);

		regenerateColor(taskTimes[task].vertexID, color);
		fprintf(f,"\t\tcolor=\"%s\"\n", color);

//		fprintf(f,"\t\tcolor=\"#808080\"\n");

		fprintf(f,"\t\t>%s.</event>\n", name);

		fprintf(flatex, "%d/", taskTimes[task].start/1000 );/*start*/
		fprintf(flatex, "%d/", (taskTimes[task].end - taskTimes[task].start)/1000);/*duration*/
		fprintf(flatex, "%d/",	 taskTimes[task].core);/*core index*/
		fprintf(flatex, "%s/",   "");/*name*/
		fprintf(flatex, "color%d,\n",taskTimes[task].vertexID%100); // color graphtransfo

		removeRange(taskTimes[task].start, taskTimes[task].end);

		if(globalEndTime < taskTimes[task].end)
			globalEndTime = taskTimes[task].end;
	}

	int j;
	*schedTime = 0;
	for(j=0; j<nbSchedulingTimes; j++){
		*schedTime += schedulingTimes[j].end - schedulingTimes[j].start;
//		printf("Sched: %d -> %d\n", schedulingTimes[j].start, schedulingTimes[j].end);
	}

	fprintf(f, "</data>\n");
	fclose(f);

	fprintf(flatex, "}\n");
	fclose(flatex);

//	printf("Median %d Sobel %d\n", sumMedian, sumSobel);
	return endTime;
}


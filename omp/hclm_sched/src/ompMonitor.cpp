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

#include "ompMonitor.h"
#include "platform.h"

typedef struct{
	int task;
	int core;
	Time start;
	Time end;
} taskTime;

static taskTime taskTimes[800];
static int nbTaskTime;

OmpMonitor::OmpMonitor(int i){
	nbTaskTime = 0;
	endTime = 0;
}

OmpMonitor::~OmpMonitor(){

}

Time OmpMonitor::getEndTime(){
	return endTime;
}

void OmpMonitor::endApp(){
	endTime = Platform::get()->getTime();
}

int OmpMonitor::startTask(int task, int core){
	int result;
#pragma omp critical
	{
		result = nbTaskTime++;
	}
	taskTimes[result].task = task;
	taskTimes[result].core = core;
	taskTimes[result].start = Platform::get()->getTime();
	return result;
}

void OmpMonitor::endTask(int id){
	taskTimes[id].end = Platform::get()->getTime();
}

static inline void regenerateColor(int refInd, char* color){
	int ired = (refInd & 0x3)*50 + 100;
	int igreen = ((refInd >> 2) & 0x3)*50 + 100;
	int iblue = ((refInd >> 4) & 0x3)*50 + 100;

	sprintf(color,"#%02x%02x%02x",ired, igreen, iblue);
}

void OmpMonitor::saveData(int iter, const char* ganttName){
	int task;

	// Creating the Gantt with real times.
	char file[100];
	char name[30];
	char color[10];
	sprintf(file, "omp/gantt_%s_%d.pgantt", ganttName, iter);
	FILE* f = fopen(file, "w+");
	if(f == NULL){
		printf("failed open file %s\n", file);
		return;
	}

	sprintf(file, "omp/gantt_%s_%d.dat", ganttName, iter);
	FILE* flatex = fopen(file, "w+");
	if(flatex == NULL){
		printf("failed open file %s\n", file);
		return;
	}

	// Writing header
	fprintf(f,"<data>\n");
	fprintf(flatex,"start,length,core,color\n");

	// Writing execution data for each slave.
	for(task=0; task<nbTaskTime; task++){
		sprintf(name, "fir_%d", taskTimes[task].task);

		fprintf(f,"\t<event\n");
		fprintf(f,"\t\tstart=\"%lu\"\n", taskTimes[task].start);
		fprintf(f,"\t\tend=\"%lu\"\n",	taskTimes[task].end);
		fprintf(f,"\t\ttitle=\"%s\"\n", name);
		fprintf(f,"\t\tmapping=\"%d\"\n", taskTimes[task].core);

		regenerateColor(taskTimes[task].task%100, color);
		fprintf(f,"\t\tcolor=\"%s\"\n", color);

//		fprintf(f,"\t\tcolor=\"#808080\"\n");

		fprintf(f,"\t\t>%s.</event>\n", name);

		fprintf(flatex, "%lu,", taskTimes[task].start/1000 );/*start*/
		fprintf(flatex, "%lu,", (taskTimes[task].end - taskTimes[task].start)/1000);/*duration*/
		fprintf(flatex, "%d,",	 taskTimes[task].core);/*core index*/
		fprintf(flatex, "color%d\n",taskTimes[task].task%100); // color graphtransfo
	}

	fprintf(f, "</data>\n");
	fclose(f);
	fclose(flatex);
}


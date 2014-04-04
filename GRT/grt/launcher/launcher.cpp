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

#include <time.h>
#include "Memory.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "launcher.h"
#include <launcher/messages/CreateTaskMsg.h>
#include <launcher/messages/SendInfoData.h>
#include <platform_queue.h>
#include <platform.h>
#include <algorithm>
#include <platform_file.h>
#include <platform_time.h>


#define PRINT_ACTOR_IN_DOT_FILE		0

static Memory memory = Memory(0x0, 0xffffffff);
static UINT32 nbFifo;
static UINT32 nbParamToRecv;


static UINT32 timeStartScheduling[MAX_PISDF_STEPS];
static UINT32 timeEndScheduling[MAX_PISDF_STEPS];
static UINT32 nbSteps=0;

void Launcher::initSchedulingTime(){
	timeStartScheduling[nbSteps] = platform_time_getValue();
}
void Launcher::endSchedulingTime(){
	timeEndScheduling[nbSteps] = platform_time_getValue();
	nbSteps++;
}


static char* regenerateColor(int refInd){
	static char color[8];
	color[0] = '\0';

	int ired = (refInd & 0x3)*50 + 100;
	int igreen = ((refInd >> 2) & 0x3)*50 + 100;
	int iblue = ((refInd >> 4) & 0x3)*50 + 100;
	char red[5];
	char green[5];
	char blue[5];
	if(ired <= 0xf){
		sprintf(red,"0%x",ired);
	}
	else{
		sprintf(red,"%x",ired);
	}

	if(igreen <= 0xf){
		sprintf(green,"0%x",igreen);
	}
	else{
		sprintf(green,"%x",igreen);
	}

	if(iblue <= 0xf){
		sprintf(blue,"0%x",iblue);
	}
	else{
		sprintf(blue,"%x",iblue);
	}

	strcpy(color,"#");
	strcat(color,red);
	strcat(color,green);
	strcat(color,blue);

	return color;
}

void Launcher::assignFifo(SRDAGGraph* graph){
	/* Creating fifos for executable vxs.*/
	for (UINT32 i = 0; i < graph->getNbVertices(); i++) {
		SRDAGVertex* vx = graph->getVertex(i);
		if(vx->getState() == SrVxStExecutable){
			for (UINT32 j = 0; j < vx->getNbOutputEdge(); j++){
				SRDAGEdge* edge = vx->getOutputEdge(j);
				if(edge->getFifoId() == -1){
					edge->setFifoId(nbFifo++);
					edge->setFifoAddress(memory.alloc(edge->getTokenRate()));
				}
			}
		}
	}
}

void Launcher::launch(SRDAGGraph* graph, Architecture* arch, BaseSchedule* schedule){
	/* Creating Tasks */
	for(UINT32 slave=0; slave < arch->getNbSlaves(); slave++){
		for(UINT32 i=0; i < schedule->getNbVertices(slave); i++){
			SRDAGVertex* vertex = (SRDAGVertex*)schedule->getVertex(slave,i);
			if(vertex->getState() == SrVxStExecutable){
				CreateTaskMsg::send(slave, vertex);
				if(vertex->getType() == ConfigureActor)
					nbParamToRecv += ((PiSDFConfigVertex*)(vertex->getReference()))->getNbRelatedParams();
			}
		}
	}
}


void Launcher::createRealTimeGantt(Architecture *arch, SRDAGGraph *dag, const char *filePathName){
	// Creating the Gantt with real times.
	platform_fopen(filePathName);

	// Writing header
	platform_fprintf("<data>\n");

	UINT32 endTime = 0;
	char name[MAX_VERTEX_NAME_SIZE];

	// Writing execution data for the master.
	for (UINT32 j=0 ; j<nbSteps; j++){
		platform_fprintf("\t<event\n");
		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartScheduling[j]);
		platform_fprintf("\t\tend=\"%u\"\n",	timeEndScheduling[j]);
		platform_fprintf("\t\ttitle=\"Step_%d\"\n", j);
		platform_fprintf("\t\tmapping=\"Master\"\n");
		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
		platform_fprintf("\t\t>Step_%d.</event>\n", j);
	}

	// Writing execution data for each slave.
	for(UINT32 slave=0; slave<arch->getNbSlaves(); slave++){
		SendInfoData::send(slave);

		UINT32 msgType = platform_QPopUINT32(slave, platformCtrlQ);
		if(msgType != MSG_EXEC_TIMES)
			exitWithCode(1068);

		UINT32 nbTasks = platform_QPopUINT32(slave, platformCtrlQ);

		for (UINT32 j=0 ; j<nbTasks; j++){
			SRDAGVertex* vertex = dag->getVertex(platform_QPopUINT32(slave, platformCtrlQ)); // data[0] contains the vertex's id.
			UINT32 startTime = platform_QPopUINT32(slave, platformCtrlQ);
			UINT32 execTime = platform_QPopUINT32(slave, platformCtrlQ);
			vertex->getName(name, MAX_VERTEX_NAME_SIZE);

			platform_fprintf("\t<event\n");
			platform_fprintf("\t\tstart=\"%u\"\n", startTime);
			platform_fprintf("\t\tend=\"%u\"\n",	startTime + execTime);
			platform_fprintf("\t\ttitle=\"%s\"\n", name);
			platform_fprintf("\t\tmapping=\"%s\"\n", arch->getSlaveName(slave));
			platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(vertex->getId()));
			platform_fprintf("\t\t>%s.</event>\n", name);


			if(endTime < startTime + execTime)
				endTime = startTime + execTime;
		}
	}
	platform_fprintf("</data>\n");
	platform_fclose();

	printf("EndTime %d\n", endTime);
}

void Launcher::resolveParameters(Architecture *arch, SRDAGGraph* topDag){
	UINT32 slave = 0;
	while(nbParamToRecv != 0){
		UINT32 msgType;
		if(platform_QNonBlockingPop(slave, platformCtrlQ, &msgType, sizeof(UINT32)) == sizeof(UINT32)){
			if(msgType != MSG_PARAM_VALUE) exitWithCode(1068);
			UINT32 vxId = platform_QPopUINT32(slave, platformCtrlQ);
			PiSDFConfigVertex* refConfigVx = (PiSDFConfigVertex*)(topDag->getVertex(vxId)->getReference());
			for(UINT32 j = 0; j < refConfigVx->getNbRelatedParams(); j++){
				refConfigVx->getRelatedParam(j)->setValue(platform_QPopUINT32(slave, platformCtrlQ));
			}
			slave = (slave+1)%arch->getNbSlaves();
			nbParamToRecv -= refConfigVx->getNbRelatedParams();
		}
	}
}

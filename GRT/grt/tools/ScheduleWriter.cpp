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

#include "ScheduleWriter.h"
#include <cstdio>
#include <platform_file.h>

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

void ScheduleWriter::write(BaseSchedule* schedule, SRDAGGraph* dag, Architecture* archi, const char* path){
//	// Getting sure that the timings expressions are resolved
//	csGraph->resolveTimings(archi);

	platform_fopen (path);
	char name[MAX_VERTEX_NAME_SIZE];

	// Writing header
	platform_fprintf ("<data>\n");

	// Exporting for gantt display
	for(int slave=0; slave<archi->getNbActiveSlaves(); slave++){
		for (UINT32 i=0 ; i<schedule->getNbVertices(slave); i++){
			SRDAGVertex* vertex = (SRDAGVertex*)(schedule->getVertex(slave, i));
//				int vertexID = dag->getVertexIndex(vertex);

//			int duration = vertex->getCsDagReference()->getIntTiming(archi->getSlaveType(vertex->getSlaveIndex()));

			vertex->getName(name, MAX_VERTEX_NAME_SIZE);

			platform_fprintf ("\t<event\n");
			platform_fprintf ("\t\tstart=\"%d\"\n", schedule->getVertexStartTime(vertex->getScheduleIndex(), vertex));
			platform_fprintf ("\t\tend=\"%d\"\n",	schedule->getVertexEndTime(vertex->getScheduleIndex(), vertex));
			platform_fprintf ("\t\ttitle=\"%s\"\n", name);
			platform_fprintf ("\t\tmapping=\"%s\"\n", archi->getSlaveName(slave));
			platform_fprintf ("\t\tcolor=\"%s\"\n",regenerateColor(vertex->getId()));
			platform_fprintf ("\t\t>%s.</event>\n", name);
		}
	}
	platform_fprintf ("</data>\n");
	platform_fclose();
}

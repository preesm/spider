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

/**
 Constructor
*/
ScheduleWriter::ScheduleWriter()
{
}

/**
 Destructor
*/
ScheduleWriter::~ScheduleWriter()
{
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

/**
 * Writes a schedule file from a SRDAG graph. The schedule 
 * is written in a XML file in two formats:
 * - one that can be read by the Java Gantt Chart Plotter
 * - one (in a comment tag) that generates Latex Gantt for the paper

 @param csGraph: reference CSDAG
 @param hGraph: implemented SRDAG
 @param archi: architecture
 @param path: output file path
*/
void ScheduleWriter::write(CSDAGGraph* csGraph, SRDAGGraph* hGraph, Architecture* archi, const char* path){
	// Getting sure that the timings expressions are resolved
	csGraph->resolveTimings(archi);
	FILE * pFile;

	pFile = fopen (path,"w");
	char name[MAX_VERTEX_NAME_SIZE];
	if(pFile != NULL){
		// Writing header
		fprintf (pFile, "<data>\n");
		
		// Exporting for gantt display
		for (int i=0 ; i<hGraph->getNbVertices() ; i++)
		{
			SRDAGVertex* vertex = hGraph->getVertex(i);
			int duration = vertex->getCsDagReference()->getIntTiming(archi->getSlaveType(vertex->getSlaveIndex()));
			sprintf(name,"%s_%d",vertex->getCsDagReference()->getName(),vertex->getReferenceIndex());
			fprintf (pFile, "\t<event\n");
			fprintf (pFile, "\t\tstart=\"%d\"\n",	(vertex->getTLevel()) );
			fprintf (pFile, "\t\tend=\"%d\"\n",		(vertex->getTLevel() + duration) );
			fprintf (pFile, "\t\ttitle=\"%s\"\n",name);
			fprintf (pFile, "\t\tmapping=\"%s\"\n",archi->getSlaveName(vertex->getSlaveIndex()));
			fprintf (pFile, "\t\tcolor=\"%s\"\n",regenerateColor(vertex->getReferenceIndex()));
			fprintf (pFile, "\t\t>%s.</event>\n",name);
		}

		// Exporting for Latex
		fprintf (pFile, "</data>\n");
		fprintf (pFile, "<!-- latex\n");
		fprintf (pFile, "{");
		for (int i=0 ; i<hGraph->getNbVertices() ; i++)
		{
			SRDAGVertex* vertex = hGraph->getVertex(i);
			int duration = vertex->getCsDagReference()->getIntTiming(archi->getSlaveType(vertex->getSlaveIndex()));
			sprintf(name,"%s-%d",vertex->getCsDagReference()->getName(),vertex->getReferenceIndex());
			fprintf (pFile, "%.3f/",	(float)(vertex->getTLevel())/1000 );/*start*/
			fprintf (pFile, "%.3f/", (float)duration/1000 );/*duration*/
			fprintf (pFile, "%d/",vertex->getSlaveIndex());/*core index*/
			fprintf (pFile, "%s/","");/*name*/
			fprintf (pFile, "color%d",vertex->getReferenceIndex());
			if(i!=hGraph->getNbVertices()-1){
				fprintf (pFile, ",");
			}
		}
		fprintf (pFile, "}\n");
		fprintf (pFile, "latex -->\n");
		
		fclose (pFile);
	}else{
		printf("Cannot open %s\n", path);
	}
}

void ScheduleWriter::write(Schedule* schedule, SRDAGGraph* hGraph, Architecture* archi, const char* path){
//	// Getting sure that the timings expressions are resolved
//	csGraph->resolveTimings(archi);
	FILE * pFile;

	pFile = fopen (path,"w");
	char name[MAX_VERTEX_NAME_SIZE];
	if(pFile != NULL){
		// Writing header
		fprintf (pFile, "<data>\n");

		// Exporting for gantt display
		for(int slave=0; slave<archi->getNbSlaves(); slave++){
			for (int i=0 ; i<schedule->getNbVertex(slave); i++){
				SRDAGVertex* vertex = schedule->getVertex(slave, i);
				int vertexID = hGraph->getVertexIndex(vertex);

	//			int duration = vertex->getCsDagReference()->getIntTiming(archi->getSlaveType(vertex->getSlaveIndex()));
				sprintf(name,"%s_%d",vertex->getCsDagReference()->getName(),vertex->getReferenceIndex());
				fprintf (pFile, "\t<event\n");
				fprintf (pFile, "\t\tstart=\"%d\"\n",	schedule->getVertexStartTime(vertexID) );
				fprintf (pFile, "\t\tend=\"%d\"\n",		schedule->getVertexEndTime(vertexID) );
				fprintf (pFile, "\t\ttitle=\"%s\"\n",name);
				fprintf (pFile, "\t\tmapping=\"%s\"\n",archi->getSlaveName(slave));
				fprintf (pFile, "\t\tcolor=\"%s\"\n",regenerateColor(vertex->getReferenceIndex()));
				fprintf (pFile, "\t\t>%s.</event>\n",name);
			}

			for (int i=0 ; i<schedule->getNbComs(slave); i++){
				sprintf(name,"com_%d",i);
				fprintf (pFile, "\t<event\n");
				fprintf (pFile, "\t\tstart=\"%d\"\n",	schedule->getComStartTime(slave, i) );
				fprintf (pFile, "\t\tend=\"%d\"\n",		schedule->getComEndTime(slave, i) );
				fprintf (pFile, "\t\ttitle=\"%s\"\n",name);
				fprintf (pFile, "\t\tmapping=\"%s_com\"\n",archi->getSlaveName(slave));
				fprintf (pFile, "\t\tcolor=\"%s\"\n",regenerateColor(i));
				fprintf (pFile, "\t\t>%s.</event>\n",name);
			}
		}
		fprintf (pFile, "</data>\n");

//
//
//		// Exporting for Latex
//		fprintf (pFile, "</data>\n");
//		fprintf (pFile, "<!-- latex\n");
//		fprintf (pFile, "{");
//		for (int i=0 ; i<hGraph->getNbVertices() ; i++)
//		{
//			SRDAGVertex* vertex = hGraph->getVertex(i);
//			int duration = vertex->getCsDagReference()->getIntTiming(archi->getSlaveType(vertex->getSlaveIndex()));
//			sprintf(name,"%s-%d",vertex->getCsDagReference()->getName(),vertex->getReferenceIndex());
//			fprintf (pFile, "%.3f/",	(float)(vertex->getTLevel())/1000 );/*start*/
//			fprintf (pFile, "%.3f/", (float)duration/1000 );/*duration*/
//			fprintf (pFile, "%d/",vertex->getSlaveIndex());/*core index*/
//			fprintf (pFile, "%s/","");/*name*/
//			fprintf (pFile, "color%d",vertex->getReferenceIndex());
//			if(i!=hGraph->getNbVertices()-1){
//				fprintf (pFile, ",");
//			}
//		}
//		fprintf (pFile, "}\n");
//		fprintf (pFile, "latex -->\n");

		fclose (pFile);
	}else{
		printf("Cannot open %s\n", path);
	}
}


void ScheduleWriter::write(BaseSchedule* schedule, SRDAGGraph* dag, Architecture* archi, const char* path){
//	// Getting sure that the timings expressions are resolved
//	csGraph->resolveTimings(archi);
	FILE * pFile;

	pFile = fopen (path,"w");
	char name[MAX_VERTEX_NAME_SIZE];
	if(pFile != NULL){
		// Writing header
		fprintf (pFile, "<data>\n");

		// Exporting for gantt display
		for(int slave=0; slave<archi->getNbSlaves(); slave++){
			for (UINT32 i=0 ; i<schedule->getNbVertices(slave); i++){
				SRDAGVertex* vertex = (SRDAGVertex*)(schedule->getVertex(slave, i));
//				int vertexID = dag->getVertexIndex(vertex);

	//			int duration = vertex->getCsDagReference()->getIntTiming(archi->getSlaveType(vertex->getSlaveIndex()));
				sprintf(name,"%s_%d", vertex->getName(), vertex->getId());
				fprintf (pFile, "\t<event\n");
				fprintf (pFile, "\t\tstart=\"%d\"\n", schedule->getVertexStartTime(vertex->getScheduleIndex(), vertex));
				fprintf (pFile, "\t\tend=\"%d\"\n",	schedule->getVertexEndTime(vertex->getScheduleIndex(), vertex));
				fprintf (pFile, "\t\ttitle=\"%s\"\n", name);
				fprintf (pFile, "\t\tmapping=\"%s\"\n", archi->getSlaveName(slave));
				fprintf (pFile, "\t\tcolor=\"%s\"\n",regenerateColor(vertex->getId()));
				fprintf (pFile, "\t\t>%s.</event>\n", name);
			}

//			for (int i=0 ; i<schedule->getNbComs(slave); i++){
//				sprintf(name,"com_%d",i);
//				fprintf (pFile, "\t<event\n");
//				fprintf (pFile, "\t\tstart=\"%d\"\n",	schedule->getComStartTime(slave, i) );
//				fprintf (pFile, "\t\tend=\"%d\"\n",		schedule->getComEndTime(slave, i) );
//				fprintf (pFile, "\t\ttitle=\"%s\"\n",name);
//				fprintf (pFile, "\t\tmapping=\"%s_com\"\n",archi->getSlaveName(slave));
//				fprintf (pFile, "\t\tcolor=\"%s\"\n",regenerateColor(i));
//				fprintf (pFile, "\t\t>%s.</event>\n",name);
//			}
		}
		fprintf (pFile, "</data>\n");
		fclose (pFile);
	}else{
		printf("Cannot open %s\n", path);
	}
}


/**
 Exports the speedups to display them with matlab. 
 Should be combined with a CSDAG config setting increasing
nb_user and nb_cb and finishing with nb_user = nb_cb = 100

 @param csGraph: reference CSDAG
 @param hGraph: implemented SRDAG
 @param archi: architecture
 @param path: output file path
*/
void ScheduleWriter::exportSpeedups(CSDAGGraph* csGraph, SRDAGGraph* hGraph, Architecture* archi, char* speedupsPath, char* spansPath, char* worksPath){
	SRDAGGraph* hGraph_cast = static_cast<SRDAGGraph *>(hGraph);
	
	static int spans[100][100];
	static int works[100][100];

	int maxEndTime = -1;
	int totalLength = 0;
	for (int i=0 ; i<hGraph_cast->getNbVertices() ; i++)
	{
		SRDAGVertex* v = hGraph_cast->getVertex(i);
		int currentLength =  v->getCsDagReference()->getIntTiming(archi->getSlaveType(v->getSlaveIndex()));
		totalLength += currentLength;
		int newEndTime = v->getTLevel()+ currentLength;
		if(newEndTime>maxEndTime){
			maxEndTime  = newEndTime;
		}
	}

	int ul_nb_cb = globalParser.parse("ul_nb_cb");
	int ul_nb_user = globalParser.parse("ul_nb_user");

	spans[ul_nb_cb-1][ul_nb_user-1] = maxEndTime;
	works[ul_nb_cb-1][ul_nb_user-1] = totalLength;


	if(spans[ul_nb_cb-1][ul_nb_user-1] <= 0){
		printf ("span error %d %d", ul_nb_cb, ul_nb_user);
	}

	if(works[ul_nb_cb-1][ul_nb_user-1] <= 0){
		printf ("work error %d %d", ul_nb_cb, ul_nb_user);
	}

	if(ul_nb_cb == 100 && ul_nb_user == 100){
		FILE * pFile;

		pFile = fopen (spansPath,"wt");
		if(pFile != NULL){
			for(int i=0; i<100;i++){
				for(int j=0; j<100;j++){
					if(spans[i][j] == 0){
						fprintf (pFile, "N");
						fprintf (pFile, "a");
						fprintf (pFile, "N");
						fprintf (pFile, " ");
					}
					else{
						fprintf (pFile, "%d ",spans[i][j]);
					}
				}
				fprintf (pFile, "\n");
			}
			fclose (pFile);
		}else{
			printf("Cannot open %s\n", spansPath);
		}

		pFile = fopen (worksPath,"wt");
		if(pFile != NULL){
			for(int i=0; i<100;i++){
				for(int j=0; j<100;j++){
					if(works[i][j] == 0){
						fprintf (pFile, "N");
						fprintf (pFile, "a");
						fprintf (pFile, "N");
						fprintf (pFile, " ");
					}
					else{
						fprintf (pFile, "%d ",works[i][j]);
					}
				}
				fprintf (pFile, "\n");
			}
			fclose (pFile);
		}else{
			printf("Cannot open %s\n", worksPath);
		}
	}
}

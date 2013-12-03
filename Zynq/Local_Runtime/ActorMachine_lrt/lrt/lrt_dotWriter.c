
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/


#include <stdio.h>
#include "lrt_definitions.h"
#include "lrt_dotWriter.h"

#if defined ARM || defined DESKTOP
#define MAX_VERTEX_NAME_SIZE 200
static char types[5][15]={"","doublecircle","ellipse","diamond","box"};
#endif

void dotWriter(OS_TCB* graph, const char* path){
#if defined ARM || defined DESKTOP
	FILE * pFile;
	AM_ACTOR_COND_STRUCT* cond;
	AM_ACTOR_ACTION_STRUCT* action;
	AM_VERTEX_STRUCT* vertex;
	int i,j;
	//char directory[_MAX_PATH];
	//getcwd(directory, sizeof(directory));

	pFile = fopen (path,"w");
	if(pFile != NULL){
		// Writing header
		fprintf (pFile, "digraph ActorMachine {\n");
		fprintf (pFile, "node [color=Black];\n");
		fprintf (pFile, "edge [color=Black];\n");
		fprintf (pFile, "rankdir=LR;\n");

		for (i=0 ; i<graph->nbVertices ; i++)
		{
			vertex = &(graph->am_vertices[i]);
			switch(vertex->type){
			case STATE:
				fprintf (pFile, "\t%d [label=\"%d\",shape=%s];\n",i,i,types[vertex->type]);
				break;
			case TEST:
				cond = &(graph->am_conditions[vertex->condID]);
				fprintf (pFile, "\t%d [label=\"F-%d\\n%s %dB\",shape=%s];\n",i,cond->fifo.id,(cond->type == FIFO_OUT)?("push"):("pop"),cond->fifo.size,types[vertex->type]);
				break;
			case EXEC:
				action = &(graph->am_actions[vertex->actionID]);
				fprintf (pFile, "\t%d [label=\"Action T%d\\n",i,action->functionID);
				for(j=0; j<action->nb_fifo_in; j++)
					fprintf (pFile, "Fin  %d\\n",action->fifo_in_id[j]);
				for(j=0; j<action->nb_fifo_out; j++)
					fprintf (pFile, "Fout %d\\n",action->fifo_out_id[j]);
				fprintf (pFile, "\",shape=%s];\n",types[vertex->type]);
				break;
			case WAIT:
				fprintf (pFile, "\t%d [label=\"\",shape=%s];\n",i,types[vertex->type]);
				break;
			default:
				break;
			}
		}

		for (i=0 ; i<graph->nbVertices ; i++)
		{
			AM_VERTEX_STRUCT* vertex = &(graph->am_vertices[i]);
			switch(vertex->type){
			case STATE:
			case EXEC:
			case WAIT:
				fprintf (pFile, "\t%d->%d;\n",i,vertex->successor_ix[0]);
				break;
			case TEST:
				fprintf (pFile, "\t%d->%d[label=\"yes\"];\n",i,vertex->successor_ix[0]);
				fprintf (pFile, "\t%d->%d[label=\"no\"];\n",i,vertex->successor_ix[1]);
				break;
			default:
				break;
			}

		}
		fprintf (pFile, "}\n");

		fclose (pFile);
	}else{
		printf("Cannot open %s\n", path);
	}
#endif
}

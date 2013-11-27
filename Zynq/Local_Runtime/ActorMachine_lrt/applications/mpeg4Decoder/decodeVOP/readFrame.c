
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
#include <stdlib.h>
#include "definitions.h"
#include <lrt_prototypes.h>
#include <hwQueues.h>

static long filePosition;
static FILE* pFile = NULL;
static uchar buffer[BUFFER_SIZE];
static uchar VOPCounter = 0;

void readFrame(){
	AM_ACTOR_ACTION_STRUCT* action;
	uint nbBytesRead;

	// Reading file position (once per complete decoding process).
	if(VOPCounter == 0){ // Indicates that the decoding process just begins.
		VOPCounter++;
		// Receiving data.
		action = OSCurActionQuery();
		read_input_fifo(action->fifo_in_id[0], sizeof(long), (UINT8*)&filePosition); // Initial file position after the VOL.
	}

	// Opening video file.
	pFile = fopen(M4V_FILE_PATH, "rb");
	if (pFile == NULL)
	{
	  printf("Cannot open m4v_file file '%s' \n", M4V_FILE_PATH);
	  exit(-1);
	}

	// Repositioning file's position.
	fseek(pFile, filePosition, SEEK_SET);

	// Reading Video Object Plane (the same as in readVOP action).
	nbBytesRead = 0;
	readUpToNextStartCode(pFile, buffer, &nbBytesRead);

	if(feof(pFile))
		// Indicating a restarting of the decoding process.
		VOPCounter = 0;
	else
		// Storing the file's position for the next iteration.
		filePosition = ftell(pFile);

	// Closing video file.
	fclose(pFile);

	// Sending data.
}

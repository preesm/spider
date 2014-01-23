
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
#include "lrt_1W1RfifoMngr.h"
#include "lrt_core.h"
#include "lrt_taskMngr.h"
#include "hwQueues.h"


//static uchar VOPCounter = 0;
FILE* pFile = NULL;
uchar FrmData[BUFFER_SIZE];
struct_VOLsimple VOL;
uchar complexity[5];
int VOLEndPos;


readVOPStateInData stInData;
readVOPOutData VOP;


void readVOP(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[])
{
	uint nbBytesRead;
	UINT32	NbMb;	// Number of Macroblocks.

	// Reading VOL info.
	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(struct_VOLsimple), (UINT8*)&VOL);
	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(long), (UINT8*)&VOLEndPos);
	readFifo(inputFIFOIds[2], inputFIFOAddrs[2], sizeof(uchar) * 5, (UINT8*)complexity);
	readFifo(inputFIFOIds[3], inputFIFOAddrs[3], sizeof(readVOPStateInData), (UINT8*)&stInData);

	if(stInData.VOPCntr == 0){
		stInData.VOPStartPos = VOLEndPos;
		stInData.VOPCntr++;
	}



	/* Opening video file.*/
	pFile = fopen(M4V_FILE_PATH, "rb");
	if (pFile == NULL)
	{
	  printf("Cannot open m4v_file file '%s' \n", M4V_FILE_PATH);
	  exit(-1);
	}

	// Repositioning file's position.
	fseek(pFile, stInData.VOPStartPos, SEEK_SET);

	// Reading Video Object Plane.
	nbBytesRead = 0;
	readUpToNextStartCode(pFile, FrmData, &nbBytesRead);

	if(feof(pFile))
		// Indicating a restarting of the decoding process.
		stInData.VOPCntr = 0;
	else
		// Storing the file's position for the next iteration.
		stInData.VOPStartPos = ftell(pFile);

	/* Closing video file.*/
	fclose(pFile);




	// Reading the VOP from the FrmData.
	VideoObjectPlaneI(
			BUFFER_START_POSITION,
			&FrmData[4],						// Skipping the start code.
			&VOL,
			complexity,
			&VOP.VideoObjectPlane_pos,
			&VOP.VideoObjectPlane_VOP,
			&VOP.VideoObjectPlane_vop_coding_type);

	// Sending parameters' values to Global Runtime.
	NbMb = (VOL.video_object_layer_height * VOL.video_object_layer_width)/256;
//	RTQueuePush_UINT32(RTInfoQueue, VOP.VideoObjectPlane_vop_coding_type);
//	RTQueuePush_UINT32(RTInfoQueue, NbMb);

	// Sending data.
	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(readVOPOutData), (UINT8*)&VOP);
	writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], BUFFER_SIZE, (UINT8*)&FrmData);
	writeFifo(outputFIFOIds[2], outputFIFOAddrs[2], sizeof(readVOPStateInData), (UINT8*)&stInData);
}

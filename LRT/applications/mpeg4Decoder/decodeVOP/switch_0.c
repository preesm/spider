
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
#include <lrt_taskMngr.h>
#include "lrt_1W1RfifoMngr.h"
#include <hwQueues.h>

static readVOPOutData VOPData;
static struct_VOLsimple VideoObjectLayer_VOLsimple;
static uchar buffer[BUFFER_SIZE];

void switch_0(UINT32 inputFIFOIds[],
		  UINT32 inputFIFOAddrs[],
		  UINT32 outputFIFOIds[],
		  UINT32 outputFIFOAddrs[],
		  UINT32 params[]){

	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(readVOPOutData), (UINT8*)&VOPData);
	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(struct_VOLsimple), (UINT8*)&VideoObjectLayer_VOLsimple);
	readFifo(inputFIFOIds[2], inputFIFOAddrs[2], BUFFER_SIZE, (UINT8*)&buffer);


	if (params[0] == 0) { // decode I
		writeFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(readVOPOutData), (UINT8*)&VOPData);
		writeFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(struct_VOLsimple), (UINT8*)&VideoObjectLayer_VOLsimple);
		writeFifo(inputFIFOIds[2], inputFIFOAddrs[2], BUFFER_SIZE, (UINT8*)&buffer);
	}
	else { // decode P
		writeFifo(inputFIFOIds[3], inputFIFOAddrs[3], sizeof(readVOPOutData), (UINT8*)&VOPData);
		writeFifo(inputFIFOIds[4], inputFIFOAddrs[4], sizeof(struct_VOLsimple), (UINT8*)&VideoObjectLayer_VOLsimple);
		writeFifo(inputFIFOIds[5], inputFIFOAddrs[5], BUFFER_SIZE, (UINT8*)&buffer);

	}

}

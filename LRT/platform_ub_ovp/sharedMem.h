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

#ifndef SHAREDMEM_H_
#define SHAREDMEM_H_


#define SH_MEM_BASE_ADDR			0x0
#define SH_MEM_HDR_REGION_SIZE		MAX_NB_FIFOs * FIFO_MUTEX_SIZE
#define SH_MEM_DATA_REGION_SIZE		0x10000000
//#define SH_MEM_SIZE					SH_MEM_HDR_REGION_SIZE + SH_MEM_DATA_REGION_SIZE
#define SH_MEM_SIZE					0xffffffff
#define SH_MEM_FILE_PATH			"../shMem"


#include "types.h"
#include "lrt_definitions.h"

typedef struct OS_SHMEM {
	UINT32 	base;
//	UINT32 	dataBase;
	UINT32 	length;
//	FILE*	file;
//	char	file_name[50];
} OS_SHMEM;


void addOSShMem(UINT32 base, UINT32 dataBase, UINT32 length, const char* filename);

void OS_ShMemInit();

UINT32 OS_ShMemRead(UINT32 address, void* data, UINT32 size);
UINT32 OS_ShMemWrite(UINT32 address, void* data, UINT32 size);

#endif /* SHAREDMEM_H_ */

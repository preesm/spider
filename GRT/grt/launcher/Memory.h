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

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <platform_types.h>
#include <platform_file.h>
#include <grt_definitions.h>

#define FIFO_HEADER_SIZE 	8
#define MEM_INIT_POS		0

typedef struct Section{
	UINT32 start;
	UINT32 size;
	struct Section* prev;
	struct Section* next;
} Section;

class Memory {
private:
	UINT32 base;
	UINT32 size;
	UINT32 last;
	char file_path[FILE_PATH_LENGTH];

	UINT32 fifoAddr[MAX_NB_HW_FIFO];
	UINT32 fifoSize[MAX_NB_HW_FIFO];
	UINT32 fifoNb;

public:
	Memory(UINT32 _base, UINT32 _size){
		base = _base;
		size = _size;
		last = MEM_INIT_POS;
		fifoNb = 0;
	}

	virtual ~Memory(){}

	void reset(){
		last = MEM_INIT_POS;
		fifoNb = 0;
	}

	UINT32 getTotalAllocated(){
		return last;
	}

	UINT32 alloc(UINT32 sectionSize){
		UINT32 start;
		if(last+sectionSize >= size){
			printf("Can't allocate, not enough shared memory\n");
			return (UINT32)-1;
		}
		start = last+base;
		last += sectionSize;

		if(last%CACHELINE_SIZE){
			last = (last/CACHELINE_SIZE+1)*CACHELINE_SIZE;
		}

		fifoSize[fifoNb] = sectionSize;
		fifoAddr[fifoNb++] = start;
		return start;
	}

	void exportMem(const char* path){
		platform_fopen(path);
		for (UINT32 i=0 ; i<fifoNb ; i++){
			platform_fprintf ("%d,%d,%d\n",i,fifoAddr[i], fifoSize[i]);
		}
		platform_fclose();
	}
};

#endif /* MEMORY_H_ */

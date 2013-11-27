
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#ifndef Memory_H_
#define Memory_H_

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <types.h>
#include <grt_definitions.h>

#define FIFO_HEADER_SIZE 	8
#define MEM_INIT_POS		0

typedef struct Section{
	int start;
	int size;
	struct Section* prev;
	struct Section* next;
} Section;

class Memory {
private:
	int base;
	int size;
//	Section* first;
	int last;
	char file_path[FILE_PATH_LENGTH];

	int fifoAddr[MAX_NB_HW_FIFO];
	int fifoSize[MAX_NB_HW_FIFO];
	int fifoNb;

public:
	Memory(int _base, int _size){
		base = _base;
		size = _size;
		last = MEM_INIT_POS;
		fifoNb = 0;
//		first = NULL;
	}

	Memory(int _base, int _size, char* file_path){
		base = _base;
		size = _size;
		last = MEM_INIT_POS;
		fifoNb = 0;
		memcpy(this->file_path, file_path, sizeof(file_path));
	}

	virtual ~Memory(){}

	void reset(){
		last = MEM_INIT_POS;
		fifoNb = 0;
	}

	int getTotalAllocated(){
//		Section* cur = first;
//		if(first==NULL) return 0;
//
//		while(cur->next != NULL) cur = cur->next;
//		return cur->start+cur->size;
		return last;
	}

	int alloc(int sectionSize){
		int start;
		sectionSize += FIFO_HEADER_SIZE+1;
		if(last+sectionSize >= size){
			printf("Can't allocate, not enough shared memory\n");
			return -1;
		}
		start = last+base;
		last += sectionSize;

		fifoSize[fifoNb] = sectionSize;
		fifoAddr[fifoNb++] = start;
		return start;

//		Section* cur = first, *newSection;
//
//		sectionSize += FIFO_HEADER_SIZE;
//
//		if(cur == NULL){
//			if(sectionSize <= size){
//				newSection = new Section;
//				newSection->start = 0;
//				newSection->size = sectionSize;
//				newSection->prev = NULL;
//				newSection->next = NULL;
//
//				first = newSection;
//				return newSection->start+base;
//			}else return -1;
//		}
//
//		if(cur->next == NULL){
//			if(size - cur->start - cur->size >= sectionSize){
//				newSection = new Section;
//				newSection->start = cur->start + cur->size;
//				newSection->size = sectionSize;
//				newSection->prev = cur;
//				newSection->next = NULL;
//
//				cur->next = newSection;
//				return newSection->start+base;
//			}else return -1;
//		}
//
//		while(cur != NULL){
//			if(cur->next != NULL && cur->next->start - cur->start - cur->size >= sectionSize ){
//				newSection = new Section;
//				newSection->start = cur->start + cur->size;
//				newSection->size = sectionSize;
//				newSection->prev = cur;
//				newSection->next = cur->next;
//
//				cur->next->prev = newSection;
//				cur->next = newSection;
//				return newSection->start+base;
//			}else if(cur->next == NULL && size - cur->start - cur->size >= sectionSize){
//				newSection = new Section;
//				newSection->start = cur->start + cur->size;
//				newSection->size = sectionSize;
//				newSection->prev = cur;
//				newSection->next = NULL;
//
//				return newSection->start+base;
//			}else{
//				cur = cur->next;
//			}
//		}
//		return -1;

	}

	UINT32 read(const int address, void* data, const int size){
		UINT16 i, res = 0;
		for (i = 0; i < this->fifoNb && res == 0; i++) {
			if (this->fifoAddr[i] <= address &&
				this->fifoAddr[i] + this->fifoSize[i] > address + size){
//					&& this->fifoAddr[i].base <= address + size
//					&& this->fifoAddr[i].base + this->fifoAddr[i].length > address + size) {
				FILE* file = fopen(this->file_path, "rb");
				fseek(file, address - this->fifoAddr[i], SEEK_SET);
				res = fread(data, size, 1, file);
				fclose(file);
				return res;
			}
		}
		printf("Memory not found 0x%x\n", address);
		return res;
	}

	void exportMem(const char* path){
		FILE * pFile;

		pFile = fopen (path,"w+");
		if(pFile != NULL){
			for (int i=0 ; i<fifoNb ; i++){
				fprintf (pFile, "%d,%d,%d\n",i,fifoAddr[i], fifoSize[i]);
			}
			fclose(pFile);
		}
	}

	void free(Section* freeSect){
//		if(freeSect->prev != NULL){
//			freeSect->prev->next = freeSect->next;
//		}else{
//			first = first->next;
//		}
//		if(freeSect->next != NULL){
//			freeSect->next->prev = freeSect->prev;
//		}
//		delete freeSect;
	}
};

#endif /* Memory_H_ */

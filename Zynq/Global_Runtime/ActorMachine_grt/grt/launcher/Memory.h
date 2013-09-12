/*
 * Memory.h
 *
 *  Created on: May 31, 2013
 *      Author: jheulot
 */

#ifndef Memory_H_
#define Memory_H_

#include <cstdlib>
#include <cstdio>

#include "../SchedulerDimensions.h"

#define FIFO_HEADER_SIZE 8

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

	int fifoAddr[MAX_NB_FIFO];
	int fifoSize[MAX_NB_FIFO];
	int fifoNb;

public:
	Memory(int _base, int _size){
		base = _base;
		size = _size;
		last = 0;
		fifoNb = 0;
//		first = NULL;
	}
	virtual ~Memory(){}

	void reset(){
		last = 0;
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
			printf("Can't allocate, not enought sharem memory\n");
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

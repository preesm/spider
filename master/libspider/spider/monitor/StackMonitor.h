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

#ifndef STACKMONITOR_H
#define STACKMONITOR_H

#include <spider.h>
#include <tools/Stack.h>
#include <tools/DynStack.h>
#include <tools/StaticStack.h>

typedef enum{
	PISDF_STACK,
	SRDAG_STACK,
	ARCHI_STACK,
	TRANSFO_STACK,
	LRT_STACK,
	STACK_COUNT
} SpiderStack;


#define CREATE(stackId, type) new(StackMonitor::alloc(stackId, sizeof(type))) type
#define CREATE_MUL(stackId, size, type) new(StackMonitor::alloc(stackId, size*sizeof(type))) type[size]


class StackMonitor {
public:
	static inline void initStack(SpiderStack id, StackConfig cfg);
	static inline void cleanAllStack();
	static inline void* alloc(SpiderStack id, int size);
	static inline void free(SpiderStack id, void* ptr);
	static inline void freeAll(SpiderStack id);

	static void printStackStats();

private:
	static Stack* stacks_[STACK_COUNT];
};

void StackMonitor::initStack(SpiderStack stackId, StackConfig cfg){
	switch(cfg.type){
	case STACK_DYNAMIC:
		stacks_[stackId] = new DynStack(cfg.name);
		break;
	case STACK_STATIC:
		stacks_[stackId] = new StaticStack(cfg.name, cfg.start, cfg.size);
		break;
	}
}

void StackMonitor::cleanAllStack(){
	for(int i=0; i<STACK_COUNT; i++){
		delete stacks_[i];
	}
}

void* StackMonitor::alloc(SpiderStack stackId, int size){
	return stacks_[stackId]->alloc(size);
}

void StackMonitor::free(SpiderStack stackId, void* ptr){
	return stacks_[stackId]->free(ptr);
}

void StackMonitor::freeAll(SpiderStack stackId){
	return stacks_[stackId]->freeAll();
}

#endif /* STACKMONITOR_H */

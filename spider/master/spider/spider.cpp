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

#include "spider.h"

static SpiderCommunicator* spiderCom = 0;
static LRT* lrt = 0;

static Stack* srdagStack = 0;
static Stack* transfoStack = 0;

static SRDAGGraph* srdag = 0;

static MemAlloc* memAlloc;
static Scheduler* scheduler;

void spider_init(SpiderConfig cfg){
	spider_setMemAllocType(cfg.memAllocType, (long)cfg.memAllocStart, cfg.memAllocSize);
	spider_setSchedulerType(cfg.schedulerType);
	spider_setSrdagStack(cfg.srdagStack);
	spider_setTransfoStack(cfg.transfoStack);
}

void spider_free(){
	if(srdag != 0)
		delete srdag;
	if(memAlloc != 0)
		delete memAlloc;
	if(scheduler != 0)
		delete scheduler;
	if(srdagStack != 0)
		delete srdagStack;
	if(transfoStack != 0)
		delete transfoStack;
}

void spider_launch(
		Archi* archi,
		PiSDFGraph* pisdf){
	delete srdag;
	srdagStack->freeAll();
	memAlloc->reset();

	srdag = new SRDAGGraph(srdagStack);

	jit_ms(pisdf, archi, srdag, transfoStack, memAlloc, scheduler);

}

void spider_setMemAllocType(MemAllocType type, int start, int size){
	if(memAlloc != 0){
		delete memAlloc;
	}
	switch(type){
	case MEMALLOC_DUMMY:
		memAlloc = new DummyMemAlloc(start, size);
		break;
	}
}

void spider_setSchedulerType(SchedulerType type){
	if(scheduler != 0){
		delete scheduler;
	}
	switch(type){
	case SCHEDULER_LIST:
		scheduler = new ListScheduler();
		break;
	}
}

void spider_setSrdagStack(StackConfig cfg){
	if(srdagStack != 0){
		delete srdagStack;
	}
	switch(cfg.type){
	case STACK_DYNAMIC:
		srdagStack = new DynStack(cfg.name);
		break;
	case STACK_STATIC:
		srdagStack = new StaticStack(cfg.name, cfg.start, cfg.size);
		break;
	}
}

void spider_setTransfoStack(StackConfig cfg){
	if(transfoStack != 0){
		delete transfoStack;
	}
	switch(cfg.type){
	case STACK_DYNAMIC:
		transfoStack = new DynStack(cfg.name);
		break;
	case STACK_STATIC:
		transfoStack = new StaticStack(cfg.name, cfg.start, cfg.size);
		break;
	}
}

SRDAGGraph* spider_getLastSRDAG(){
	return srdag;
}

void setSpiderCommunicator(SpiderCommunicator* com){
	spiderCom = com;
}

SpiderCommunicator* getSpiderCommunicator(){
	return spiderCom;
}

void setLrt(LRT* l){
	lrt = l;
}
LRT* getLrt(){
	return lrt;
}

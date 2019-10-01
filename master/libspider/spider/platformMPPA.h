/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet,            *
 * Hugo Miomandre                                                           *
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

#ifndef PLATFORM_MPPA_H
#define PLATFORM_MPPA_H

#include "platform.h"
#include <pthread.h>
#ifdef __k1dp__ // for pthread_barrier_t
#include <utask.h>
#endif

#include <lrt.h>

#include <mppa_async.h>

#define IO_CPUID_OFFSET (128*16)

#ifdef __k1io__
#define QUEUE_SPIDER_TO_LRT_SIZE (1<<15)	//taille max constatée = 9792 bytes sur x86, + de 24k sur mppa
#else
#define QUEUE_SPIDER_TO_LRT_SIZE (512)
#endif

#define QUEUE_LRT_TO_SPIDER_SIZE (32*2)		//taille max constatée = 32 bytes

#define MPPA_SPIDER_NB_CLUSTER_MAX (16)
#define MPPA_SPIDER_NB_CLUSTER_PE_MAX (16)

#define MPPA_SPIDER_NB_IO_MAX (2)
#define MPPA_SPIDER_NB_IO_PE_MAX (4)

// #define LRT_SIZE (43 * 1024)
#define LRT_SIZE (7 * 1024)
#define AVAIL_MEM_BASE (1428 * 1024)


struct Arg_lrt;

class PlatformMPPA: public Platform{
public:
	/** File Handling */
	virtual FILE* fopen(const char* name);
	virtual void fprintf(FILE* id, const char* fmt, ...);
	virtual void fclose(FILE* id);

	/** Shared Memory Handling */
	virtual void* virt_to_phy(void* address);
	virtual long getMinAllocSize();
	virtual int getCacheLineSize();

	/** Time Handling */
	virtual void rstTime();
	virtual void rstTime(struct ClearTimeMsg* msg);
	virtual Time getTime();

	virtual void rstJobIx();

	/** Platform Core Handling **/
	virtual void idleLrt(int i);
	virtual void wakeLrt(int i);
	virtual void idle();

	/** Platform getter/setter */
	inline LRT* getLrt();
	inline LrtCommunicator* getLrtCommunicator();
	inline SpiderCommunicator* getSpiderCommunicator();

	inline void setStack(SpiderStack id, Stack* stack);
	inline Stack* getStack(SpiderStack id);
	inline Stack* getStack(int id);

	inline int getThreadNumber();
	inline int getThreadNumber(int lrtIx);

	inline int getLrtId(int cid);

	inline int getMaxActorAllocSize(int pe);

	inline int getNbCluster();

	inline int getNbLrt();

	inline int getLrtJobIx(int lrtIx);

	inline void setLrtJobIx(int lrtIx, int jobIx);

	inline void rstLrtJobIx();

	/* Fonction de thread */
	void lrtPThreads(Arg_lrt *argument_lrt);

	PlatformMPPA(int nLrtIo, int nLrtCc, int nbCc, int shMemSize, lrtFct* fcts, int nLrtFcts, SpiderStackConfig &stackConfig);
	virtual ~PlatformMPPA();


private:
	static Time mappingTime(int nActors, int nPE);

	int nLrt_;
	int nLrtIo_;
	int nLrtCc_;
	int nbCc_;
	int nLrtLocal_;
	int* thread_ID_tab_;

	// must be long long type to support mppa_async_peek and mppa_async_poke
	long long* jobTab_;

	//Pointeurs vers les stacks
	Stack* stackPisdf;
	Stack* stackSrdag;
	Stack* stackTransfo;
	Stack** stackLrt;
	Stack** stackArchi;

	//Pointeurs vers les fifo
	mppa_async_segment_t* fifoSpidertoLRT;
	mppa_async_segment_t* fifoLRTtoSpider;

	LRT** lrt_;
	LrtCommunicator** lrtCom_;
	SpiderCommunicator* spiderCom_;

	mppa_async_segment_t jobTab_segment_;
	mppa_async_segment_t dataMem_segment_;
	mppa_async_segment_t* notif_segment_;
	mppa_async_segment_t* trace_rdma_segment_;

	#ifdef __k1dp__
	pthread_barrier_t lrtCcBarrier_;
	#endif
};


inline void PlatformMPPA::setStack(SpiderStack id, Stack* stack){
	switch(id){
	case PISDF_STACK :
		stackPisdf = stack;
		break;
	case SRDAG_STACK :
		stackSrdag = stack;
		break;
	case TRANSFO_STACK :
		stackTransfo = stack;
		break;
	case ARCHI_STACK :
		stackArchi[getThreadNumber()] = stack;
		break;
	case LRT_STACK :
		stackLrt[getThreadNumber()] = stack;
		break;
	default :
		throw "Error in stack index\n";
	}
}

inline Stack* PlatformMPPA::getStack(SpiderStack id){
	switch(id){
	case PISDF_STACK :
		return stackPisdf;
		break;
	case SRDAG_STACK :
		return stackSrdag;
		break;
	case TRANSFO_STACK :
		return stackTransfo;
		break;
	case ARCHI_STACK :
		return stackArchi[getThreadNumber()];
		break;
	case LRT_STACK :
		return stackLrt[getThreadNumber()];
		break;
	default :
		throw "Error in stack index\n";
	}
}

inline Stack* PlatformMPPA::getStack(int id){
	switch(id){
	case PISDF_STACK :
		return stackPisdf;
		break;
	case SRDAG_STACK :
		return stackSrdag;
		break;
	case TRANSFO_STACK :
		return stackTransfo;
		break;
	case ARCHI_STACK :
		return stackArchi[getThreadNumber()];
		break;
	case LRT_STACK :
		return stackLrt[getThreadNumber()];
		break;
	default :
		throw "Error in stack index\n";
	}
}


inline int PlatformMPPA::getLrtId(int cid){
	unsigned int i = 0;

	while(thread_ID_tab_[i] != cid) i++;

	return i;
}


inline int PlatformMPPA::getMaxActorAllocSize(int pe){
	if (pe < nLrtIo_) return 1024 * 1024 * 1024; //1 Go
	else return (AVAIL_MEM_BASE - nLrtCc_ * LRT_SIZE);
}

inline int PlatformMPPA::getThreadNumber(int lrtIx){
	
	return thread_ID_tab_[lrtIx];
}

// Give the calling cpuId to locate it in a cluster/io
inline int PlatformMPPA::getThreadNumber(){
#ifdef __k1io__

	for(int i = 0;i < nLrt_;i++){
		if (thread_ID_tab_[i] == __k1_get_cpu_id() + IO_CPUID_OFFSET)
			return i;
	}
	throw "Error undefined ID\n";


#else
	for(int i = 0;i < nLrt_;i++){
		if (thread_ID_tab_[i] ==  __k1_get_cluster_id()*MPPA_SPIDER_NB_CLUSTER_MAX+__k1_get_cpu_id())
			//return i - nLrtIo_ - __k1_get_cluster_id()*nLrtCc_;
			return __k1_get_cpu_id();
	}
	throw "Error undefined ID\n";
#endif
}

inline LRT* PlatformMPPA::getLrt(){
	return lrt_[getThreadNumber()];
}

inline LrtCommunicator* PlatformMPPA::getLrtCommunicator(){
	return lrtCom_[getThreadNumber()];
}

inline SpiderCommunicator* PlatformMPPA::getSpiderCommunicator(){
	if(spiderCom_)
		return spiderCom_;
	else
		throw "Error undefined spider communicator\n";
}

inline int PlatformMPPA::getNbCluster(){
	return nbCc_;
}

inline int PlatformMPPA::getNbLrt(){
	return nLrt_;
}

inline int PlatformMPPA::getLrtJobIx(int lrtIx){
	return __builtin_k1_ldu(&jobTab_[lrtIx]);
}

inline void PlatformMPPA::setLrtJobIx(int lrtIx, int jobIx){
	__builtin_k1_sdu(&jobTab_[lrtIx],jobIx);
}

inline void PlatformMPPA::rstLrtJobIx(){
	memset(jobTab_,0,sizeof(long long)*nLrt_);
	// __builtin_k1_wpurge();
	// __builtin_k1_fence();
}

// Structure de passage d'argument dans le thread
typedef struct Arg_lrt {
	PlatformMPPA *instance;
	int shMemSize;
	lrtFct* fcts;
	int nLrtFcts;
	int indice;
	//StackConfig archiStack;
	StackInfo lrtStack;
}Arg_lrt;

typedef union{
	struct{
		long long bitmask[9];
		long long unlockJobIx[256 + 4];
		long long semaphore[9];
		long long jobIx;
		long long peek_semaphore[9];
	};
	long long payload[9 + 256 + 4 + 9 + 1 + 9];
}Notif_t;


// Fonction wrapper pour lancer un thread sur une méthode d'objet
void* lrtPThreads_helper(void *voidArgs);

#endif/*PLATFORM_MPPA_H*/

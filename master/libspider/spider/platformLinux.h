/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#ifndef PLATFORM_LINUX_H
#define PLATFORM_LINUX_H

#include "platform.h"
#include <signal.h>

class PlatformLinux: public Platform{
public:
	/** File Handling */
	virtual FILE* fopen(const char* name);
	virtual void fprintf(FILE* id, const char* fmt, ...);
	virtual void fclose(FILE* id);

	/** Shared Memory Handling */
	virtual void* virt_to_phy(void* address);
	virtual int getMinAllocSize();
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

	PlatformLinux(int nLrt, int shMemSize, lrtFct* fcts, int nLrtFcts, StackConfig archiStack,
			StackConfig lrtStack, StackConfig pisdfStack, StackConfig srdagStack, StackConfig transfoStack);
	virtual ~PlatformLinux();

private:
	enum{
		SIG_IDLE = SIGUSR1,
		SIG_WAKE = SIGUSR2
	};

	int* cpIds_;

	static Time mappingTime(int nActors);
	static void sig_handler(int signo);

	Stack* stacks[STACK_COUNT];

	LRT* lrt_;
	LrtCommunicator* lrtCom_;
	SpiderCommunicator* spiderCom_;
};


inline LRT* PlatformLinux::getLrt(){
	if(lrt_)
		return lrt_;
	else
		throw "Error undefined LRT\n";
}

inline LrtCommunicator* PlatformLinux::getLrtCommunicator(){
	if(lrtCom_)
		return lrtCom_;
	else
		throw "Error undefined LRT Communicator\n";
}

inline SpiderCommunicator* PlatformLinux::getSpiderCommunicator(){
	if(spiderCom_)
		return spiderCom_;
	else
		throw "Error undefined Spider Communicator\n";
}

inline void PlatformLinux::setStack(SpiderStack id, Stack* stack){
	stacks[id] = stack;
}

inline Stack* PlatformLinux::getStack(SpiderStack id){
	return stacks[id];
}

inline Stack* PlatformLinux::getStack(int id){
	return stacks[id];
}


#endif/*PLATFORM_LINUX_H*/

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

#include "Schedule.h"
#include <string.h>
#include <stdio.h>

Schedule::Schedule(int nPE, int nJobMax, Stack *stack){
	stack_ = stack;
	nPE_ = nPE;
	nJobMax_ = nJobMax;
	nJobPerPE_ = CREATE_MUL(stack, nPE_, int);
	readyTime_ = CREATE_MUL(stack, nPE_, Time);
	schedules_ = CREATE_MUL(stack, nPE_*nJobMax_, SRDAGVertex*);

	memset(nJobPerPE_, 0, nPE_*sizeof(int));
	memset(readyTime_, 0, nPE_*sizeof(Time));
}

Schedule::~Schedule(){
	stack_->free(nJobPerPE_);
	stack_->free(readyTime_);
	stack_->free(schedules_);
}

void Schedule::addJob(int pe, SRDAGVertex* job, Time start, Time end){
	if(pe < 0 || pe >= nPE_)
		throw "Schedule: Accessing bad PE\n";
	if(nJobPerPE_[pe] >= nJobMax_)
		throw "Schedule: too much jobs\n";

	schedules_[pe*nJobMax_+nJobPerPE_[pe]] = job;
	nJobPerPE_[pe]++;
	readyTime_[pe] = std::max(readyTime_[pe], end);
	job->setStartTime(start);
	job->setEndTime(end);
}

void Schedule::print(const char* path){
	int file = Platform::get()->fopen(path);
	char name[100];

	// Writing header
	Platform::get()->fprintf(file, "<data>\n");

	// Exporting for gantt display
	for(int pe=0; pe < nPE_; pe++){
		for (int job=0 ; job < nJobPerPE_[pe]; job++){
			SRDAGVertex* vertex = getJob(pe, job);

			vertex->toString(name, 100);
			Platform::get()->fprintf(file, "\t<event\n");
			Platform::get()->fprintf(file, "\t\tstart=\"%d\"\n", vertex->getStartTime());
			Platform::get()->fprintf(file, "\t\tend=\"%d\"\n",	vertex->getEndTime());
			Platform::get()->fprintf(file, "\t\ttitle=\"%s\"\n", name);
			Platform::get()->fprintf(file, "\t\tmapping=\"PE%d\"\n", pe);

			int ired = (vertex->getId() & 0x3)*50 + 100;
			int igreen = ((vertex->getId() >> 2) & 0x3)*50 + 100;
			int iblue = ((vertex->getId() >> 4) & 0x3)*50 + 100;
			Platform::get()->fprintf(file, "\t\tcolor=\"#%02x%02x%02x\"\n", ired, igreen, iblue);

			Platform::get()->fprintf(file, "\t\t>%s.</event>\n", name);
		}
	}
	Platform::get()->fprintf(file, "</data>\n");
	Platform::get()->fclose(file);
}

bool Schedule::check(){
	bool result = true;

	/* Check core concurrency */
	for(int pe=0; pe<nPE_ && result; pe++){
		for(int i=0; i<nJobPerPE_[pe]-1 && result; i++){
			SRDAGVertex* vertex = getJob(pe, i);
			SRDAGVertex* nextVertex = getJob(pe, i+1);

			if(vertex->getEndTime() > nextVertex->getStartTime()){
				result = false;
				char name[100];
				vertex->toString(name, 100);
				printf("Schedule: Superposition: task %s ", name);
				nextVertex->toString(name, 100);
				printf("and %s\n", name);
			}
		}
	}

	/* Check Communications */
	for(int pe=0; pe<nPE_ && result; pe++){
		for(int i=0; i<nJobPerPE_[pe]-1 && result; i++){
			SRDAGVertex* vertex = getJob(pe, i);

			for(int i=0; i<vertex->getNConnectedInEdge() && result; i++){
				SRDAGVertex* precVertex = vertex->getInEdge(i)->getSrc();

				if(vertex->getStartTime() < precVertex->getEndTime()){
					result = false;
					char name[100];
					vertex->toString(name, 100);
					printf("Schedule: Communication: task %s ", name);
					precVertex->toString(name, 100);
					printf("and %s\n", name);
				}
			}
		}
	}

	return result;
}

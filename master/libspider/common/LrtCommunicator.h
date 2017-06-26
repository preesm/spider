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

#ifndef LRT_COMMUNICATOR_H
#define LRT_COMMUNICATOR_H

#include <stdint.h>

#include "Message.h"

class LrtCommunicator {
public:
	virtual ~LrtCommunicator(){}

	virtual void* ctrl_start_send(int size) = 0;
	virtual void ctrl_end_send(int size) = 0;

	virtual int ctrl_start_recv(void** data) = 0;
	virtual void ctrl_end_recv() = 0;

	virtual void* trace_start_send(int size) = 0;
	virtual void trace_end_send(int size) = 0;

	virtual uintptr_t data_start_send(Fifo* f) = 0;
	virtual void data_end_send(Fifo* f) = 0;

	virtual uintptr_t data_recv(Fifo* f) = 0;

	virtual void allocateDataBuffer(int nbInput, Fifo* fIn, int nbOutput, Fifo* fOut) {};
	virtual void freeDataBuffer(int nbInput, int nbOutput) {};

	virtual void setLrtJobIx(int lrtIx, int jobIx){};
	virtual long getLrtJobIx(int lrtIx) = 0;

	virtual void rstLrtJobIx(int lrtIx) {};

	virtual void waitForLrtUnlock(int nbDependency, int* blkLrtIx, int* blkLrtJobIx, int jobIx) {};
	virtual void unlockLrt(int jobIx) {};


protected:
	LrtCommunicator(){}
};

#endif/*LRT_COMMUNICATOR_H*/

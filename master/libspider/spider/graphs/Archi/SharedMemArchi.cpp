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

#include "SharedMemArchi.h"
#include <tools/Stack.h>

SharedMemArchi::SharedMemArchi(int nPE, int nPEType, int spiderPe, MappingTimeFct mapFct) {
	nPE_ = nPE;
	//printf("SPIDER %d\n", nPE_);
	nPEType_ = nPEType;
	spiderPe_ = spiderPe;
	peType_ = CREATE_MUL(ARCHI_STACK, nPE_, int);
	peActive_ = CREATE_MUL(ARCHI_STACK, nPE_, bool);
	peName_ = CREATE_MUL(ARCHI_STACK, nPE_, char*);
	peTypeASend_ = CREATE_MUL(ARCHI_STACK, nPEType_, float);
	peTypeBSend_ = CREATE_MUL(ARCHI_STACK, nPEType_, float);
	peTypeARecv_ = CREATE_MUL(ARCHI_STACK, nPEType_, float);
	peTypeBRecv_ = CREATE_MUL(ARCHI_STACK, nPEType_, float);
	mapFct_ = mapFct;

	nPEperType_ = CREATE_MUL(ARCHI_STACK, nPEType_, int);
	
	memset(nPEperType_,0,nPEType_ * sizeof(*nPEperType_));
	memset(peActive_,false,nPE_ * sizeof(bool));
}


SharedMemArchi::~SharedMemArchi() {
	for(int i=0; i<nPE_; i++)
		StackMonitor::free(ARCHI_STACK, peName_[i]);

	/** Free allocated arrays **/
	StackMonitor::free(ARCHI_STACK, peType_);
	StackMonitor::free(ARCHI_STACK, peActive_);
	StackMonitor::free(ARCHI_STACK, peName_);
	StackMonitor::free(ARCHI_STACK, peTypeASend_);
	StackMonitor::free(ARCHI_STACK, peTypeBSend_);
	StackMonitor::free(ARCHI_STACK, peTypeARecv_);
	StackMonitor::free(ARCHI_STACK, peTypeBRecv_);

	StackMonitor::free(ARCHI_STACK, nPEperType_);

	/** Reset values **/
	nPE_ = 0;
	nPEType_ = 0;
	peType_ = 0;
	peName_ = 0;
	peTypeASend_ = 0;
	peTypeBSend_ = 0;
	peTypeARecv_ = 0;
	peTypeBRecv_ = 0;

	nPEperType_ = 0;
}


int SharedMemArchi::getNPE() const{
	return nPE_;
}

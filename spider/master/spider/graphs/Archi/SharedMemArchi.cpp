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

SharedMemArchi::SharedMemArchi(Stack* stack, int nPE, int nPEType) {
	stack_ = stack;
	nPE_ = nPE;
	nPEType_ = nPEType;
	peType_ = CREATE_MUL(stack, nPE_, int);
	peName_ = CREATE_MUL(stack, nPE_, char*);
	peTypeASend_ = CREATE_MUL(stack, nPEType_, float);
	peTypeBSend_ = CREATE_MUL(stack, nPEType_, float);
	peTypeARecv_ = CREATE_MUL(stack, nPEType_, float);
	peTypeBRecv_ = CREATE_MUL(stack, nPEType_, float);
}

SharedMemArchi::~SharedMemArchi() {
	for(int i=0; i<nPE_; i++)
		stack_->free(peName_[i]);

	/** Free allocated arrays **/
	stack_->free(peType_);
	stack_->free(peName_);
	stack_->free(peTypeASend_);
	stack_->free(peTypeBSend_);
	stack_->free(peTypeARecv_);
	stack_->free(peTypeBRecv_);

	/** Reset values **/
	nPE_ = 0;
	nPEType_ = 0;
	peType_ = 0;
	peName_ = 0;
	peTypeASend_ = 0;
	peTypeBSend_ = 0;
	peTypeARecv_ = 0;
	peTypeBRecv_ = 0;
}


int SharedMemArchi::getNPE() const{
	return nPE_;
}

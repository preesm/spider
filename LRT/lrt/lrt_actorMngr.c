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

#include "lrt_fifoMngr.h"
#include <hwQueues.h>
#include "lrt_actorMngr.h"


//void createActor(OS_TCB* newTCB){
//	UINT32 i;
//	LRTActor* actor;
//
//	actor = &LRTActorTbl[newTCB->OSTCBId];
//
//	actor->nbInputFifos = RTQueuePop_UINT32(RTCtrlQueue);
//	actor->nbOutputFifos = RTQueuePop_UINT32(RTCtrlQueue);
//	for (i = 0; i < actor->nbInputFifos; i++) {
//		actor->inputFifoId[i] = RTQueuePop_UINT32(RTCtrlQueue);
//		actor->inputFifoDataOff[i] = RTQueuePop_UINT32(RTCtrlQueue);
//		// TODO: get the FIFO' size
//	}
//	for (i = 0; i < actor->nbOutputFifos; i++) {
//		actor->outputFifoId[i] = RTQueuePop_UINT32(RTCtrlQueue);
//		actor->outputFifoDataOff[i] = RTQueuePop_UINT32(RTCtrlQueue);
//		// TODO: get the FIFO' size
//	}
//	for ( i = 0; i < actor->nbParams; i++) {
//		actor->params[i] = RTQueuePop_UINT32(RTCtrlQueue);
//	}
//}


void verifyActorCond(LRTActor* actor){
//	UINT32 i;
//	// Checking input FIFOs.
//	for (i = 0; i < actor->nbInputFifos; i++) {
//		while(!check_input_fifo(actor->inputFifoId[i], actor->readDataSize[i]));
//	}
//
//	// Checking output FIFOs.
//	for (i = 0; i < actor->nbOutputFifos; i++) {
//		while(!check_output_fifo(actor->outputFifoId[i], actor->writeDataSize[i]));
//	}
}

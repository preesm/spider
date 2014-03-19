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

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "hwQueues.h"
#include "swfifoMngr.h"

#define IN_CTRL_QUEUE_BASE		0x20000000
#define OUT_CTRL_QUEUE_BASE		0x20000600

#define MBOX_SIZE				1024

//#define WRITE_REG_OFFSET	0x00	/**< Mbox write register */
//#define READ_REG_OFFSET		0x08	/**< Mbox read register */
//#define STATUS_REG_OFFSET	0x10	/**< Mbox status reg  */
//#define STATUS_FIFO_EMPTY	0x00000001 /**< Receive FIFO is Empty */
//#define STATUS_FIFO_FULL	0x00000002 /**< Send FIFO is Full */
//
//typedef struct MBox {
//	UINT32 	base;
////	UINT32 	dataBase;
//	UINT32 	length;
////	UINT32	WrDataReg;
////	UINT32	RdDataReg;
////	UINT32	StatusReg;
////	FILE*	file;
////	char	file_name[50];
//} MBOX;

static LRT_FIFO_HNDLE RTQueue[nbQueueTypes][2];
int cpuId;

void RTQueuesInit(){
	create_swfifo(&(RTQueue[RTCtrlQueue][RTInputQueue]), MBOX_SIZE, IN_CTRL_QUEUE_BASE);
	flush_swfifo(&(RTQueue[RTCtrlQueue][RTInputQueue]));
//	RTQueue[RTInfoQueue][RTInputQueue] =
//	RTQueue[RTJobQueue][RTInputQueue] =
//
	create_swfifo(&(RTQueue[RTCtrlQueue][RTOutputQueue]), MBOX_SIZE, OUT_CTRL_QUEUE_BASE);
	flush_swfifo(&(RTQueue[RTCtrlQueue][RTOutputQueue]));
//	RTQueue[RTInfoQueue][RTOutputQueue] =
//	RTQueue[RTJobQueue][RTOutputQueue] =
}


UINT32 RTQueuePush(RTQueueType queueType, void* data, int size){
	write_output_swfifo(&RTQueue[queueType][RTOutputQueue], size, data);
	return size;
}


UINT32 RTQueuePush_UINT32(RTQueueType queueType, UINT32 value){
	return RTQueuePush(queueType, &value, sizeof(UINT32));
}


UINT32 RTQueuePop(RTQueueType queueType, void* data, int size){
	read_input_swfifo(&RTQueue[queueType][RTInputQueue], size, data);
	return size;
}


UINT32 RTQueuePop_UINT32(RTQueueType queueType){
	UINT32 data;
	RTQueuePop(queueType, &data, sizeof(UINT32));
	return data;
}


UINT32 RTQueueNonBlockingPop(RTQueueType queueType, void* data, int size){
	if(check_input_swfifo(&RTQueue[queueType][RTInputQueue], size)){
		read_input_swfifo(&RTQueue[queueType][RTInputQueue], size, data);
		return size;
	}
	else
		return 0;
}

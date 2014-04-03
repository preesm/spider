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

#include <platform_types.h>
#include <platform_queue.h>
#include <platform.h>

#include "qmss_utils.h"
#include "queue_buffer.h"

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

#define PACKET_SIZE (160-12)
#define EMPTY_CTRL 	896

#define CTRL_IN		(897   +platform_getCoreId())
#define CTRL_OUT	(897+ 7+platform_getCoreId())
#define INFO_IN		(897+14+platform_getCoreId())
#define INFO_OUT	(897+21+platform_getCoreId())
#define JOB_IN		(897+28+platform_getCoreId())
#define JOB_OUT		(897+35+platform_getCoreId())

static UINT32 queues[PlatformNbQueueTypes][2];

typedef enum{
	INIT,
	NORMAL
}MSG_Types;

void delay(UINT32 cycles){
	UINT64      value;
    /* Get the current TSC  */
    value = CSL_tscRead ();
    while ((CSL_tscRead ()  - value) < cycles);
}

void platform_queue_Init(){
	CSL_tscEnable();

	/* Wait on Core0 init*/
	MNAV_MonolithicPacketDescriptor *mono_pkt;
	do{
		mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(CTRL_IN);
		delay(100);
	}while(mono_pkt == 0);

	if (mono_pkt->packet_type == INIT)
		push_queue(EMPTY_CTRL,1,0,(UINT32)mono_pkt);
	else{
		printf("Error in Queue Init\n");
		abort();
	}

	queues[PlatformCtrlQueue][PlatformInputQueue] 	= CTRL_IN;
	queues[PlatformCtrlQueue][PlatformOutputQueue]	= CTRL_OUT;
	queues[PlatformInfoQueue][PlatformInputQueue] 	= INFO_IN;
	queues[PlatformInfoQueue][PlatformOutputQueue] 	= INFO_OUT;
	queues[PlatformJobQueue] [PlatformInputQueue] 	= JOB_IN;
	queues[PlatformJobQueue] [PlatformOutputQueue] 	= JOB_OUT;
}

UINT32 platform_queue_push_data(PlatformQueueType queueType, void* data, int size){
	MNAV_MonolithicPacketDescriptor *mono_pkt;
	do{
		mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(EMPTY_CTRL);
		delay(100);
	}while(mono_pkt == 0);

	mono_pkt->type_id = 0x2;
	mono_pkt->packet_type = NORMAL;
	mono_pkt->data_offset = 12;
	mono_pkt->packet_length = size+12;
	mono_pkt->epib = 0;
	mono_pkt->pkt_return_qnum = EMPTY_CTRL;
	mono_pkt->src_tag_lo = 1; //copied to .flo_idx of streaming i/f

	void* data_pkt = (void*)(((UINT32)mono_pkt) + mono_pkt->data_offset);
	memcpy(data_pkt, data, size);

	push_queue(queues[queueType][PlatformOutputQueue], 1, 0, (UINT32)mono_pkt);

	return size;
}

UINT32 platform_queue_push(PlatformQueueType queueType, void* data, int size){
	int res = 0;
	while(res != size){
		res += platform_queue_push_data(queueType, (void*)((UINT32)data+res), MIN(size-res, PACKET_SIZE));
	}
	return res;
}

UINT32 platform_queue_push_UINT32(PlatformQueueType queueType, UINT32 data){
	UINT32 res = platform_queue_push(queueType, &data, sizeof(unsigned int));
	return res;
}

UINT32 platform_queue_pop_data(PlatformQueueType queueType){
	MNAV_MonolithicPacketDescriptor *mono_pkt;

	do{
		mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(queues[queueType][PlatformInputQueue]);
		delay(100);
	}while(mono_pkt == 0);

	void* data_pkt = (void*)(((UINT32)mono_pkt) + mono_pkt->data_offset);
	UINT32 size = mono_pkt->packet_length-mono_pkt->data_offset;
	QBuffer_push(queueType, data_pkt, size);

	push_queue(EMPTY_CTRL, 1, 0, (UINT32)mono_pkt);

	return size;
}

UINT32 platform_queue_pop_UINT32(PlatformQueueType queueType){
	UINT32 data;
	platform_queue_pop(queueType, &data, sizeof(UINT32));
	return data;
}


UINT32 platform_queue_pop(PlatformQueueType queueType, void* data, int size){
	int res=0;

	if(size==0)
		return 0;

	int nbData = QBuffer_getNbData(queueType);
	if(nbData > 0){
		nbData = MIN(nbData, size);
		QBuffer_pop(queueType, (void*)((UINT32)data+res), nbData);
		res = nbData;
		if(size == res)
			return res;
	}

	do{
		platform_queue_pop_data(queueType);

		nbData = QBuffer_getNbData(queueType);
		nbData = MIN(nbData, size-res);
		QBuffer_pop(queueType, (void*)((UINT32)data+res), nbData);
		res += nbData;
	}while(size != res);

	return res;
}

BOOL platform_queue_NBPop_UINT32(PlatformQueueType queueType, UINT32* data){
	int nbData = QBuffer_getNbData(queueType);
	if(nbData >= sizeof(UINT32)){
		QBuffer_pop(queueType, (void*)data, sizeof(UINT32));
		return TRUE;
	}

	while(1){
		if(get_descriptor_count(queues[queueType][PlatformInputQueue])){
			platform_queue_pop_data(queueType);

			nbData = QBuffer_getNbData(queueType);
			if(nbData >= sizeof(UINT32)){
				QBuffer_pop(queueType, (void*)data, sizeof(UINT32));
				return TRUE;
			}
		}else
			return FALSE;
	}
}

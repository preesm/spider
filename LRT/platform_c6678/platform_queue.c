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

#include <ti/csl/csl_tsc.h>

#include <platform_types.h>
#include <platform_queue.h>
#include <platform.h>

#include "qmss_utils.h"
#include "memoryAlloc.h"
#include "cache.h"

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
static MNAV_MonolithicPacketDescriptor* cur_mono_pkt[PlatformNbQueueTypes][2];
static int cur_mono_pkt_size[PlatformNbQueueTypes][2];

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

	memset(cur_mono_pkt, 0, sizeof(cur_mono_pkt));
	memset(cur_mono_pkt_size, 0, sizeof(cur_mono_pkt_size));

	/* Wait on Core0 init*/
	MNAV_MonolithicPacketDescriptor *mono_pkt;
	do{
		mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(CTRL_IN);
		delay(100);
	}while(mono_pkt == 0);

	cache_invL1D(mono_pkt, CTRL_DESC_SIZE);

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
UINT32 platform_queue_push_data_internal(PlatformQueueType queueType, void* data, int size){
	while(cur_mono_pkt[queueType][0] == 0){
		cur_mono_pkt[queueType][0] = (MNAV_MonolithicPacketDescriptor*)pop_queue(EMPTY_CTRL);
		if(cur_mono_pkt[queueType][0] !=0){
			/* Initialize header */
			cache_invL1D(cur_mono_pkt[queueType][0], CTRL_DESC_SIZE);

			cur_mono_pkt[queueType][0]->type_id = 0x2;
			cur_mono_pkt[queueType][0]->packet_type = NORMAL;
			cur_mono_pkt[queueType][0]->data_offset = 12;
			cur_mono_pkt[queueType][0]->epib = 0;
			cur_mono_pkt[queueType][0]->pkt_return_qnum = EMPTY_CTRL;
			cur_mono_pkt[queueType][0]->src_tag_lo = 1; //copied to .flo_idx of streaming i/f
			break;
		}
		delay(100);
	}

	UINT32 toCopy = MIN(CTRL_DESC_SIZE - cur_mono_pkt_size[queueType][0] - 12, size);

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((UINT32)cur_mono_pkt[queueType][0]) + 12 + cur_mono_pkt_size[queueType][0]);
	memcpy(data_pkt, data, toCopy);

	cur_mono_pkt_size[queueType][0] += toCopy;
	return toCopy;
}

void platform_queue_push_finalize(PlatformQueueType queueType){
	if(cur_mono_pkt[queueType][0]){
		/* Send the descriptor */
		cur_mono_pkt[queueType][0]->packet_length = cur_mono_pkt_size[queueType][0]+12;

		cache_wbInvL1D(cur_mono_pkt[queueType][0], CTRL_DESC_SIZE);

		push_queue(queues[queueType][PlatformOutputQueue], 1, 0, (UINT32)cur_mono_pkt[queueType][0]);

//		printf("Send a descriptor of %d bytes\n", cur_mono_pkt_size[queueType][0]);

		cur_mono_pkt[queueType][0] = 0;
		cur_mono_pkt_size[queueType][0] = 0;

	}
}

UINT32 platform_queue_push(PlatformQueueType queueType, void* data, int size){
	UINT32 sended = 0;
	sended += platform_queue_push_data_internal(queueType, ((char*)data)+sended, size-sended);
	while(sended < size){
		platform_queue_push_finalize(queueType);
		sended += platform_queue_push_data_internal(queueType, ((char*)data)+sended, size-sended);
	}
	return sended;
}

UINT32 platform_queue_push_UINT32(PlatformQueueType queueType, UINT32 data){
	UINT32 res = platform_queue_push(queueType, &data, sizeof(unsigned int));
	return res;
}

UINT32 platform_queue_nbpop_data(PlatformQueueType queueType, void* data, int size){
	if(cur_mono_pkt[queueType][1] == 0){
		cur_mono_pkt[queueType][1] = (MNAV_MonolithicPacketDescriptor*)pop_queue(queues[queueType][PlatformInputQueue]);
		if(cur_mono_pkt[queueType][1] == 0){
			return 0;
		}
		cache_invL1D(cur_mono_pkt[queueType][1], CTRL_DESC_SIZE);
	}

	void* data_pkt = (void*)(((UINT32)cur_mono_pkt[queueType][1])
			+ cur_mono_pkt[queueType][1]->data_offset
			+ cur_mono_pkt_size[queueType][1]);
	UINT32 data_size = cur_mono_pkt[queueType][1]->packet_length
			- cur_mono_pkt[queueType][1]->data_offset
			- cur_mono_pkt_size[queueType][1];

	UINT32 toCopy = MIN(size, data_size);

	memcpy(data, data_pkt, toCopy);
	cur_mono_pkt_size[queueType][1] += toCopy;

	/* End of descriptor */
	if(toCopy == data_size){
		push_queue(EMPTY_CTRL, 1, 0, (UINT32)cur_mono_pkt[queueType][1]);
		cur_mono_pkt[queueType][1] = 0;
		cur_mono_pkt_size[queueType][1] = 0;
	}

	return toCopy;
}

UINT32 platform_queue_pop_UINT32(PlatformQueueType queueType){
	UINT32 data;
	platform_queue_pop(queueType, &data, sizeof(UINT32));
	return data;
}


UINT32 platform_queue_pop(PlatformQueueType queueType, void* data, int size){
	UINT32 recved = 0;
	while(recved < size){
		recved += platform_queue_nbpop_data(queueType, (char*)data+recved, size-recved);
	}
	return recved;
}

BOOL platform_queue_NBPop_UINT32(PlatformQueueType queueType, UINT32* data){
	return platform_queue_nbpop_data(queueType, data, sizeof(UINT32));
}

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

//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <fcntl.h>

#include <platform_types.h>
#include <platform_queue.h>
#include <grt_definitions.h>

#include <ti/csl/csl_tsc.h>
#include <memoryAlloc.h>
#include "cache.h"
#include "qmss_utils.h"

#define PACKET_SIZE 160-12
#define EMPTY_CTRL 	896
#define EMPTY_DATA 	940

#define BASE_DATA	941
#define NB_Q		8191 - BASE_ID


#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

#define CTRL_OUT(id)	(897   +id)
#define CTRL_IN(id)		(897+ 7+id)
#define INFO_OUT(id)	(897+14+id)
#define INFO_IN(id)		(897+21+id)
#define JOB_OUT(id)		(897+28+id)
#define JOB_IN(id)		(897+35+id)

static MNAV_MonolithicPacketDescriptor* cur_mono_pkt[7][3][2];
static int cur_mono_pkt_size[7][3][2];


typedef enum{
	INIT,
	NORMAL
}MSG_Types;


UINT8 *mono_ctrl_region = (UINT8 *)CTRL_DESC_BASE;
UINT8 *mono_data_region = (UINT8 *)DATA_DESC_BASE;

void delay(UINT32 cycles){
	UINT64      value;
    /* Get the current TSC  */
    value = CSL_tscRead ();
    while ((CSL_tscRead ()  - value) < cycles);
}

void __c_platform_queue_Init(UINT8 nbSlaves){
	MNAV_MonolithicPacketDescriptor * mono_pkt;
	UINT32 idx;

	memset(cur_mono_pkt, 0, sizeof(cur_mono_pkt));
	memset(cur_mono_pkt_size, 0, sizeof(cur_mono_pkt_size));

	CSL_tscEnable();

	/* Setup Memory Region 1 for 8 148B Monolithic descriptors. Our
	 * Mono descriptors will be 12 bytes plus 16 bytes of EPIB Info, plus
	 * 128 bytes of payload, but the next best size is 160 bytes times
	 * 32 descriptors. (dead space is possible) */

	printf("CTRL_REG %#x DATA_REG %#x\n", CTRL_REG, DATA_REG);
	set_memory_region(0, (Uint32) mono_ctrl_region, 0, CTRL_REG);
	set_memory_region(1, (Uint32) mono_data_region, CTRL_DESC_NB, DATA_REG);

	/*****************************************************************
	 * Configure Linking RAM 0 to use the 16k entry internal link ram.
	 */
	set_link_ram(0, 0x00080000, 0x3FFF);

	/* Initialize descriptor regions to zero */
	memset(mono_ctrl_region, 0, CTRL_DESC_SIZE*CTRL_DESC_NB);
	cache_wbInvL1D(mono_ctrl_region, CTRL_DESC_SIZE*CTRL_DESC_NB);
	memset(mono_data_region, 0, DATA_DESC_SIZE*DATA_DESC_NB);
	cache_wbInvL1D(mono_data_region, CTRL_DESC_SIZE*CTRL_DESC_NB);


	Uint32 n;
	for(idx = EMPTY_CTRL; idx < 8192; idx++)
		empty_queue(idx, (UINT32*)NULL, &n);

	for (idx = 0; idx < CTRL_DESC_NB; idx++) {
		mono_pkt = (MNAV_MonolithicPacketDescriptor *) (mono_ctrl_region
				+ (idx * CTRL_DESC_SIZE));

		cache_invL1D(mono_pkt, CTRL_DESC_SIZE);

		mono_pkt->pkt_return_qmgr = 1;
		mono_pkt->pkt_return_qnum = 1;

		cache_wbInvL1D(mono_pkt, CTRL_DESC_SIZE);

		push_queue(EMPTY_CTRL, 1, 0, (Uint32) (mono_pkt));
	}

	for (idx = 0; idx < DATA_DESC_NB; idx++) {
		mono_pkt = (MNAV_MonolithicPacketDescriptor *) (mono_data_region
				+ (idx * DATA_DESC_SIZE));

		cache_invL1D(mono_pkt, DATA_DESC_SIZE);

		mono_pkt->pkt_return_qmgr = 1;
		mono_pkt->pkt_return_qnum = 1;

		cache_wbInvL1D(mono_pkt, DATA_DESC_SIZE);

		push_queue(EMPTY_DATA, 1, 0, (Uint32) (mono_pkt));
	}

	for(idx = 0; idx<nbSlaves; idx++){
		mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(EMPTY_CTRL);

		cache_invL1D(mono_pkt, CTRL_DESC_SIZE);

		mono_pkt->type_id = 0x2;
		mono_pkt->packet_type = INIT;
		mono_pkt->data_offset = 12;
		mono_pkt->packet_length = 160;
		mono_pkt->epib = 0;
		mono_pkt->pkt_return_qnum = EMPTY_CTRL;
		mono_pkt->src_tag_lo = 1; //copied to .flo_idx of streaming i/f

		cache_wbInvL1D(mono_pkt, CTRL_DESC_SIZE);

		push_queue(CTRL_OUT(idx), 1, 0, (UINT32)mono_pkt);
	}
}

UINT32 __c_platform_QPush_data_internal(UINT8 slaveId, platformQType queueType, void* data, int size){
	while(cur_mono_pkt[slaveId][queueType][0] == 0){
		cur_mono_pkt[slaveId][queueType][0] = (MNAV_MonolithicPacketDescriptor*)pop_queue(EMPTY_CTRL);
		if(cur_mono_pkt[slaveId][queueType][0] !=0){
			/* Initialize header */
			cache_invL1D(cur_mono_pkt[slaveId][queueType][0], CTRL_DESC_SIZE);

			cur_mono_pkt[slaveId][queueType][0]->type_id = 0x2;
			cur_mono_pkt[slaveId][queueType][0]->packet_type = NORMAL;
			cur_mono_pkt[slaveId][queueType][0]->data_offset = 12;
			cur_mono_pkt[slaveId][queueType][0]->epib = 0;
			cur_mono_pkt[slaveId][queueType][0]->pkt_return_qnum = EMPTY_CTRL;
			cur_mono_pkt[slaveId][queueType][0]->src_tag_lo = 1; //copied to .flo_idx of streaming i/f
			break;
		}
		delay(100);
	}

	UINT32 toCopy = MIN(CTRL_DESC_SIZE - cur_mono_pkt_size[slaveId][queueType][0] - 12, size);

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((UINT32)cur_mono_pkt[slaveId][queueType][0]) + 12 + cur_mono_pkt_size[slaveId][queueType][0]);
	memcpy(data_pkt, data, toCopy);

	cur_mono_pkt_size[slaveId][queueType][0] += toCopy;
	return toCopy;
}

void __c_platform_QPush_finalize(UINT8 slaveId, platformQType queueType){
	if(cur_mono_pkt[slaveId][queueType][0]){
		/* Send the descriptor */
		cur_mono_pkt[slaveId][queueType][0]->packet_length = cur_mono_pkt_size[slaveId][queueType][0]+12;

		cache_wbInvL1D(cur_mono_pkt[slaveId][queueType][0], CTRL_DESC_SIZE);

		switch(queueType){
		case platformCtrlQ:
			push_queue(CTRL_OUT(slaveId), 1, 0, (UINT32)cur_mono_pkt[slaveId][queueType][0]);
			break;
		case platformInfoQ:
			push_queue(INFO_OUT(slaveId), 1, 0, (UINT32)cur_mono_pkt[slaveId][queueType][0]);
			break;
		case platformJobQ:
			push_queue(JOB_OUT(slaveId), 1, 0, (UINT32)cur_mono_pkt[slaveId][queueType][0]);
			break;
		}

//		printf("Send a descriptor of %d bytes\n", cur_mono_pkt_size[slaveId][queueType][0]);

		cur_mono_pkt[slaveId][queueType][0] = 0;
		cur_mono_pkt_size[slaveId][queueType][0] = 0;

	}
}

UINT32 __c_platform_QPush_data(UINT8 slaveId, platformQType queueType, void* data, int size){
	UINT32 sended = 0;
	sended += __c_platform_QPush_data_internal(slaveId, queueType, ((char*)data)+sended, size-sended);
	while(sended < size){
		__c_platform_QPush_finalize(slaveId, queueType);
		sended += __c_platform_QPush_data_internal(slaveId, queueType, ((char*)data)+sended, size-sended);
	}
	return sended;
}



UINT32 __c_platform_QPush(UINT8 slaveId, platformQType queueType, void* data, int size){
	int res = 0;
	while(res != size){
		res += __c_platform_QPush_data(slaveId, queueType, (void*)((UINT32)data+res), MIN(size-res, PACKET_SIZE));
	}
	return res;
}

UINT32 __c_platform_QPushUINT32(UINT8 slaveId, platformQType queueType, UINT32 data){
	UINT32 res = __c_platform_QPush_data(slaveId, queueType, &data, sizeof(unsigned int));
	return res;
}

UINT32 __c_platform_QNBPop_data(UINT8 slaveId, platformQType queueType, void* data, UINT32 size){
	UINT32 queue;
	switch(queueType){
	case platformCtrlQ:
		queue = CTRL_IN(slaveId);
		break;
	case platformInfoQ:
		queue = INFO_IN(slaveId);
		break;
	case platformJobQ:
		queue = JOB_IN(slaveId);
		break;
	}

	if(cur_mono_pkt[slaveId][queueType][1] == 0){
		cur_mono_pkt[slaveId][queueType][1] = (MNAV_MonolithicPacketDescriptor*)pop_queue(queue);
		if(cur_mono_pkt[slaveId][queueType][1] == 0){
			return 0;
		}
		cache_invL1D(cur_mono_pkt[slaveId][queueType][1], CTRL_DESC_SIZE);
	}

	void* data_pkt = (void*)(((UINT32)cur_mono_pkt[slaveId][queueType][1])
			+ cur_mono_pkt[slaveId][queueType][1]->data_offset
			+ cur_mono_pkt_size[slaveId][queueType][1]);
	UINT32 data_size = cur_mono_pkt[slaveId][queueType][1]->packet_length
			- cur_mono_pkt[slaveId][queueType][1]->data_offset
			- cur_mono_pkt_size[slaveId][queueType][1];

	UINT32 toCopy = MIN(size, data_size);

	memcpy(data, data_pkt, toCopy);
	cur_mono_pkt_size[slaveId][queueType][1] += toCopy;

	/* End of descriptor */
	if(toCopy == data_size){
		push_queue(EMPTY_CTRL, 1, 0, (UINT32)cur_mono_pkt[slaveId][queueType][1]);
		cur_mono_pkt[slaveId][queueType][1] = 0;
		cur_mono_pkt_size[slaveId][queueType][1] = 0;
	}

	return toCopy;
}


UINT32 __c_platform_QPop(UINT8 slaveId, platformQType queueType, void* data, int size){
	UINT32 recved = 0;
	while(recved < size){
		recved += __c_platform_QNBPop_data(slaveId, queueType, (char*)data+recved, size-recved);
	}
	return recved;
}

UINT32 __c_platform_QPopUINT32(UINT8 slaveId, platformQType queueType){
	UINT32 data;
	__c_platform_QPop(slaveId, queueType, &data, sizeof(UINT32));
	return data;
}

UINT32 __c_platform_QNonBlockingPop(UINT8 slaveId, platformQType queueType, void* data, int size){
	return __c_platform_QNBPop_data(slaveId, queueType, data, size);
}

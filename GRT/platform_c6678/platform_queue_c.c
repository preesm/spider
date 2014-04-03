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
#include "queue_buffer.h"

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

typedef enum{
	INIT,
	NORMAL
}MSG_Types;

#include "qmss_utils.h"

UINT8 *mono_region = (UINT8 *)CTRL_DESCRIPTOR;
UINT8 *mono_data_region = (UINT8 *)DATA_DESCRIPTOR;

void delay(UINT32 cycles){
	UINT64      value;
    /* Get the current TSC  */
    value = CSL_tscRead ();
    while ((CSL_tscRead ()  - value) < cycles);
}

void __c_platform_queue_Init(){
	MNAV_MonolithicPacketDescriptor * mono_pkt;
	UINT32 idx;

	CSL_tscEnable();

	/* Setup Memory Region 1 for 8 148B Monolithic descriptors. Our
	 * Mono descriptors will be 12 bytes plus 16 bytes of EPIB Info, plus
	 * 128 bytes of payload, but the next best size is 160 bytes times
	 * 32 descriptors. (dead space is possible) */
	set_memory_region(0, (Uint32) mono_region, 0, 0x00090000);

	/*****************************************************************
	 * Configure Linking RAM 0 to use the 16k entry internal link ram.
	 */
	set_link_ram(0, 0x00080000, 0x3FFF);

	/* Initialize descriptor regions to zero */
	memset(mono_region, 	 0, 32 * 160);
	memset(mono_data_region, 0, 128 * 16);


	Uint32 n;
	for(idx = EMPTY_CTRL; idx < 8192; idx++)
		empty_queue(idx, (UINT32*)NULL, &n);

	for (idx = 0; idx < 32; idx++) {
		mono_pkt = (MNAV_MonolithicPacketDescriptor *) (mono_region
				+ (idx * 160));

		mono_pkt->pkt_return_qmgr = 1;
		mono_pkt->pkt_return_qnum = 1;

		push_queue(EMPTY_CTRL, 1, 0, (Uint32) (mono_pkt));
	}

	for (idx = 0; idx < 128; idx++) {
		mono_pkt = (MNAV_MonolithicPacketDescriptor *) (mono_data_region
				+ (idx * 16));

		mono_pkt->pkt_return_qmgr = 1;
		mono_pkt->pkt_return_qnum = 1;

		push_queue(EMPTY_DATA, 1, 0, (Uint32) (mono_pkt));
	}

//	for(idx = 0; idx<7; idx++)
	mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(EMPTY_CTRL);

	mono_pkt->type_id = 0x2;
	mono_pkt->packet_type = INIT;
	mono_pkt->data_offset = 12;
	mono_pkt->packet_length = 160;
	mono_pkt->epib = 0;
	mono_pkt->pkt_return_qnum = EMPTY_CTRL;
	mono_pkt->src_tag_lo = 1; //copied to .flo_idx of streaming i/f

	push_queue(CTRL_OUT(0), 1, 0, (UINT32)mono_pkt);
}

UINT32 __c_platform_QPush_data(UINT8 slaveId, platformQType queueType, void* data, int size){
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

	switch(queueType){
	case platformCtrlQ:
		push_queue(CTRL_OUT(slaveId), 1, 0, (UINT32)mono_pkt);
		break;
	case platformInfoQ:
		push_queue(INFO_OUT(slaveId), 1, 0, (UINT32)mono_pkt);
		break;
	case platformJobQ:
		push_queue(JOB_OUT(slaveId), 1, 0, (UINT32)mono_pkt);
		break;
	}

	return size;
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

UINT32 __c_platform_QPop_data(UINT8 slaveId, platformQType queueType){
	MNAV_MonolithicPacketDescriptor *mono_pkt;
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

	do{
		mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(queue);
		delay(100);
	}while(mono_pkt == 0);

	void* data_pkt = (void*)(((UINT32)mono_pkt) + mono_pkt->data_offset);
	UINT32 size = mono_pkt->packet_length-mono_pkt->data_offset;
	QBuffer_push(slaveId, queueType, data_pkt, size);

	push_queue(EMPTY_CTRL, 1, 0, (UINT32)mono_pkt);

	return size;
}

UINT32 __c_platform_QPop(UINT8 slaveId, platformQType queueType, void* data, int size){
	int res=0;

	if(size==0)
		return 0;

	int nbData = QBuffer_getNbData(slaveId, queueType);
	if(nbData > 0){
		nbData = MIN(nbData, size);
		QBuffer_pop(slaveId, queueType, (void*)((UINT32)data+res), nbData);
		res = nbData;
		if(size == res)
			return res;
	}

	do{
		__c_platform_QPop_data(slaveId, queueType);

		nbData = QBuffer_getNbData(slaveId, queueType);
		nbData = MIN(nbData, size-res);
		QBuffer_pop(slaveId, queueType, (void*)((UINT32)data+res), nbData);
		res += nbData;
	}while(size != res);

	return res;
}

UINT32 __c_platform_QPopUINT32(UINT8 slaveId, platformQType queueType){
	UINT32 data;
	__c_platform_QPop(slaveId, queueType, &data, sizeof(UINT32));
	return data;
}

UINT32 __c_platform_QNonBlockingPop(UINT8 slaveId, platformQType queueType, void* data, int size){
	MNAV_MonolithicPacketDescriptor *mono_pkt;
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

	mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(queue);
	if(mono_pkt == 0){
		return 0;
	}

	void* data_pkt = (void*)(((UINT32)mono_pkt) + mono_pkt->data_offset);
	memcpy(data, data_pkt, size);

	push_queue(EMPTY_CTRL, 1, 0, (UINT32)mono_pkt);

	return size;
}

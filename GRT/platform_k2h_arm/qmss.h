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

#ifndef QMSS_CFG_H_
#define QMSS_CFG_H_

#include <stdint.h>

typedef struct {
	/* word 0 */
	uint32_t packet_length :16; //in bytes (65535 max)
	uint32_t data_offset :9;
	uint32_t packet_type :5;
	uint32_t type_id :2; //always 0x2 (Monolithic Packet ID)

	/* word 1 */
	uint32_t dest_tag_lo :8;
	uint32_t dest_tag_hi :8;
	uint32_t src_tag_lo :8;
	uint32_t src_tag_hi :8;

	/* word 2 */
	uint32_t pkt_return_qnum :12;
	uint32_t pkt_return_qmgr :2;
	uint32_t ret_push_policy :1; //0=return to queue tail, 1=queue head
	uint32_t reserved_w2b :1;
	uint32_t ps_flags :4;
	uint32_t err_flags :4;
	uint32_t psv_word_count :6; //number of 32-bit PS data words
	uint32_t reserved_w2 :1;
	uint32_t epib :1; //1=extended packet info block is present
} MonoPcktDesc;
#define MONO_PACKET_SIZE	sizeof(MonoPcktDesc)

#define CTRL_DESC_SIZE	256
#define CTRL_DESC_NB	512
#define CTRL_DESC_POOL_SIZE	CTRL_DESC_SIZE*CTRL_DESC_NB

#define DATA_DESC_SIZE	64
#define DATA_DESC_NB	1024
#define DATA_DESC_POOL_SIZE	DATA_DESC_SIZE*DATA_DESC_NB

#define QMSS_DESC_BASE	0x0C000000
#define QMSS_DESC_END	0x0C600000
#define DATA_BASE		0x90000000
#define DATA_END		0xA0000000

#define EMPTY_CTRL 	896
#define EMPTY_DATA 	897

#define BASE_CTRL_OUT	898
#define BASE_CTRL_IN	911
/* 898 -> 910 : GRT -> LRT# comm */
/* 911 -> 923 : LRT -> GRT# comm */

#define BASE_DATA 	924
#define MAX_QUEUES 	4000
#define NB_DATA		MAX_QUEUES-BASE_DATA

#define PACKET_HEADER 12

void platform_queue_Init();
void platform_queue_Exit();

#endif /* QMSS_CFG_H_ */

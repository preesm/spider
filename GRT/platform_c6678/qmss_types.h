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

#ifndef QMSS_TYPES_H_
#define QMSS_TYPES_H_

//Define the Accumulator Command Interface Structure
#ifdef _BIG_ENDIAN
typedef struct {
	uint32_t retrn_code :8; //0=idle, 1=success, 2-6=error
	uint32_t un1 :8;
	uint32_t command :8;//0x80=disable, 0x81=enable, 0=firmware response
	uint32_t channel :8;//0 to 47 or 0 to 15

	uint32_t queue_mask;//(multi-mode only) bit 0=qm_index queue

	uint32_t list_address;//address of Host ping-pong buffer

	uint32_t max_entries :16;//max entries per list
	uint32_t qm_index :16;//qnum to monitor (multiple of 32 for multimode)

	uint32_t un2 :8;
	uint32_t cfg_un :2;
	uint32_t cfg_multi_q :1;//0=single queue mode, 1=multi queue mode
	uint32_t cfg_list_mode :1;//0=NULL terminate, 1=entry count mode
	uint32_t cfg_list_size :2;//0="D" Reg, 1="C+D" regs, 2="A+B+C+D"
	uint32_t cfg_int_delay :2;//0=none, 1=last int, 2=1st new, 3=last new
	uint32_t timer_count :16;//number of 25us timer ticks to delay int
}Qmss_AccCmd;

#else
typedef struct {
	uint32_t channel :8;	//0 to 47 or 0 to 15
	uint32_t command :8;	//0x80=disable, 0x81=enable, 0=firmware response
	uint32_t un1 :8;
	uint32_t retrn_code :8; //0=idle, 1=success, 2-6=error

	uint32_t queue_mask;			//(multi-mode only) bit 0=qm_index queue

	uint32_t list_address;			//address of Host ping-pong buffer

	uint32_t qm_index :16;		//qnum to monitor (multiple of 32 for multimode)
	uint32_t max_entries :16;			//max entries per list

	uint32_t timer_count :16;		//number of 25us timer ticks to delay int
	uint32_t cfg_int_delay :2;	//0=none, 1=last int, 2=1st new, 3=last new
	uint32_t cfg_list_size :2;	//0="D" Reg, 1="C+D" regs, 2="A+B+C+D"
	uint32_t cfg_list_mode :1;	//0=NULL terminate, 1=entry count mode
	uint32_t cfg_multi_q :1; //0=single queue mode, 1=multi queue mode
	uint32_t cfg_un :2;
	uint32_t un2 :8;
} Qmss_AccCmd;
#endif

/*******************************************************************/
/* Define the bit and word layouts for the Host Packet Descriptor. */
/* For a Host Packet, this is used for the first descriptor only. */
/*******************************************************************/
#ifdef _BIG_ENDIAN
typedef struct {
	/* word 0 */
	uint32_t type_id : 2;	//always 0x0 (Host Packet ID)
	uint32_t packet_type : 5;
	uint32_t reserved_w0 : 2;
	uint32_t ps_reg_loc : 1;//0=PS words in desc, 1=PS words in SOP buff
	uint32_t packet_length : 22;//in bytes (4M - 1 max)

	/* word 1 */
	uint32_t src_tag_hi : 8;
	uint32_t src_tag_lo : 8;
	uint32_t dest_tag_hi : 8;
	uint32_t dest_tag_lo : 8;

	/* word 2 */
	uint32_t epib : 1; 	//1=extended packet info block is present
	uint32_t reserved_w2 : 1;
	uint32_t psv_word_count : 6;//number of 32-bit PS data words
	uint32_t err_flags : 4;
	uint32_t ps_flags : 4;
	uint32_t return_policy : 1;//0=linked packet goes to pkt_return_qnum,
							   //1=each descriptor goes to pkt_return_qnum
	uint32_t ret_push_policy : 1;//0=return to queue tail, 1=queue head
	uint32_t pkt_return_qmgr : 2;
	uint32_t pkt_return_qnum : 12;

	/* word 3 */
	uint32_t reserved_w3 : 10;
	uint32_t buffer_len : 22;

	/* word 4 */
	uint32_t buffer_ptr;

	/* word 5 */
	uint32_t next_desc_ptr;

	/* word 6 */
	uint32_t orig_buff0_pool : 4;
	uint32_t orig_buff0_refc : 6;
	uint32_t orig_buff0_len : 22;

	/* word 7 */
	uint32_t orig_buff0_ptr;
}MNAV_HostPacketDescriptor;

#else

typedef struct {
	/* word 0 */
	uint32_t packet_length :22; 	//in bytes (4M - 1 max)
	uint32_t ps_reg_loc :1; 	//0=PS words in desc, 1=PS words in SOP buff
	uint32_t reserved_w0 :2;
	uint32_t packet_type :5;
	uint32_t type_id :2; 	//always 0x0 (Host Packet ID)

	/* word 1 */
	uint32_t dest_tag_lo :8;
	uint32_t dest_tag_hi :8;
	uint32_t src_tag_lo :8;
	uint32_t src_tag_hi :8;

	/* word 2 */
	uint32_t pkt_return_qnum :12;
	uint32_t pkt_return_qmgr :2;
	uint32_t ret_push_policy :1; 	//0=return to queue tail, 1=queue head
	uint32_t return_policy :1; 	//0=linked packet goes to pkt_return_qnum,
	//1=each descriptor goes to pkt_return_qnum
	uint32_t ps_flags :4;
	uint32_t err_flags :4;
	uint32_t psv_word_count :6; //number of 32-bit PS data words
	uint32_t reserved_w2 :1;
	uint32_t epib :1; //1=extended packet info block is present

	/* word 3 */
	uint32_t buffer_len :22;
	uint32_t reserved_w3 :10;

	/* word 4 */
	uint32_t buffer_ptr;

	/* word 5 */
	uint32_t next_desc_ptr;

	/* word 6 */
	uint32_t orig_buff0_len :22;
	uint32_t orig_buff0_refc :6;
	uint32_t orig_buff0_pool :4;

	/* word 7 */
	uint32_t orig_buff0_ptr;

} MNAV_HostPacketDescriptor;
#endif

#define MNAV_HOST_PACKET_SIZE	sizeof(MNAV_HostPacketDescriptor)

/*******************************************************************/
/* Define the bit and word layouts for the Host Buffer Descriptor. */
/* For a Host Packet, this will used for secondary descriptors.
 */
/*******************************************************************/
#ifdef _BIG_ENDIAN
typedef struct {
	/* word 0 */
	uint32_t reserved_w0;

	/* word 1 */
	uint32_t reserved_w1;

	/* word 2 */
	uint32_t reserved_w2 : 17;
	uint32_t ret_push_policy : 1;	//0=return to queue tail, 1=queue ead
	uint32_t pkt_return_qmgr : 2;
	uint32_t pkt_return_qnum : 12;

	/* word 3 */
	uint32_t reserved_w3 : 10;
	uint32_t buffer_len : 22;

	/* word 4 */
	uint32_t buffer_ptr;

	/* word 5 */
	uint32_t next_desc_ptr;

	/* word 6 */
	uint32_t orig_buff0_pool : 4;
	uint32_t orig_buff0_refc : 6;
	uint32_t orig_buff0_len : 22;

	/* word 7 */
	uint32_t orig_buff0_ptr;

}MNAV_HostBufferDescriptor;

#else

typedef struct {
	/* word 0 */
	uint32_t reserved_w0;

	/* word 1 */
	uint32_t reserved_w1;

	/* word 2 */
	uint32_t pkt_return_qnum :12;
	uint32_t pkt_return_qmgr :2;
	uint32_t ret_push_policy :1; 	//0=return to queue tail, 1=queue head
	uint32_t reserved_w2 :17;

	/* word 3 */
	uint32_t buffer_len :22;
	uint32_t reserved_w3 :10;

	/* word 4 */
	uint32_t buffer_ptr;

	/* word 5 */
	uint32_t next_desc_ptr;

	/* word 6 */
	uint32_t orig_buff0_len :22;
	uint32_t orig_buff0_refc :6;
	uint32_t orig_buff0_pool :4;

	/* word 7 */
	uint32_t orig_buff0_ptr;

} MNAV_HostBufferDescriptor;
#endif

// Host Buffer packet size is always the same as Host Packet size
/*********************************************************************/
/* Define the bit and word layouts for the Monolithic Pkt Descriptor.*/
/*********************************************************************/
#ifdef _BIG_ENDIAN
typedef struct {
	/* word 0 */
	uint32_t type_id : 2;  //always 0x2 (Monolithic Packet ID)
	uint32_t packet_type : 5;
	uint32_t data_offset : 9;
	uint32_t packet_length : 16;//in bytes (65535 max)

	/* word 1 */
	uint32_t src_tag_hi : 8;
	uint32_t src_tag_lo : 8;
	uint32_t dest_tag_hi : 8;
	uint32_t dest_tag_lo : 8;

	/* word 2 */
	uint32_t epib : 1; //1=extended packet info block is present
	uint32_t reserved_w2 : 1;
	uint32_t psv_word_count : 6;//number of 32-bit PS data words
	uint32_t err_flags : 4;
	uint32_t ps_flags : 4;
	uint32_t reserved_w2b : 1;
	uint32_t ret_push_policy: 1;//0=return to queue tail, 1=queue head
	uint32_t pkt_return_qmgr: 2;
	uint32_t pkt_return_qnum: 12;

}MNAV_MonolithicPacketDescriptor;

#else

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
} MNAV_MonolithicPacketDescriptor;
#endif

#define MNAV_MONO_PACKET_SIZE	sizeof(MNAV_MonolithicPacketDescriptor)

/*********************************************************************/
/* Define the word layout of the Extended Packet Info Block. It      */
/* is optional and may follow Host Packet and Monolithic descriptors.*/
/*********************************************************************/

typedef struct {
	/* word 0 */
	uint32_t timestamp;

	/* word 1 */
	uint32_t sw_info0;

	/* word 2 */
	uint32_t sw_info1;

	/* word 3 */
	uint32_t sw_info2;
} MNAV_ExtendedPacketInfoBlock;

/* Define Accumulator Command constants. */
#define QMSS_ACC_CMD_TIMER         0x82
#define QMSS_ACC_CMD_ENABLE        0x81
#define QMSS_ACC_CMD_DISABLE       0x80

#endif /* QMSS_TYPES_H_ */

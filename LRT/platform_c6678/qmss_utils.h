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

#ifndef QMSS_UTILS_H_
#define QMSS_UTILS_H_

/*
 * qmss.c
 *
 *  Created on: Feb 27, 2014
 *      Author: jheulot
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ti/csl/tistdtypes.h>
#include "qmss_addr.h"
#include "qmss_types.h"

/* This function programs a QM memory region. */
void set_memory_region(Uint16 regn, Uint32 addr, Uint32 indx, Uint32 setup);

/* This function programs a QM Link RAM. */
void set_link_ram(Uint16 ram, Uint32 addr, Uint32 count) ;

/* This function pushes descriptor info to a queue.
 * mode parameter: 1 = write reg D only.
 *
 2 = write regs C and D.
 *
 * It turns out the VBUSM is more efficient to push to than VBUSP,
 * and also allows for atomic c+d pushes.
 */
void push_queue(Uint16 qn, Uint8 mode, Uint32 c_val, Uint32 d_val);

/* This function pops a descriptor address from a queue. */
Uint32 pop_queue(Uint16 qn);

/* This function moves a source queue to a destination queue. If
 * headtail = 0, the source queue is appended to the tail of the
 * dest queue. If 1, it is appended at the head. */
void divert_queue(Uint16 src_qn, Uint16 dest_qn, Uint8 headtail);

/* This function pops a queue until it is empty. If *list is not NULL,
 * it will return the list of descriptor addresses and the count. */
void empty_queue(Uint16 qn, Uint32 *list, Uint32 *listCount);

/* This function returns the byte count of a queue. */
Uint32 get_byte_count(Uint16 qn);

/* This function returns the descriptor count of a queue. */
Uint32 get_descriptor_count(Uint16 qn);

/* This function sets a queue threshold for queue monitoring purposes. */
void set_queue_threshold(Uint16 qn, Uint32 value);

/* This function programs a Hi or Lo Accumulator channel. */
void program_accumulator(Uint16 pdsp, Qmss_AccCmd *cmd);

/* This function disables a Hi or Lo Accumulator program. */
void disable_accumulator(Uint16 pdsp, Uint16 channel);

/* This function writes a new value to a PDSP's firmware
 * Time is specified in usec, then converted to the hardware
 * expect value assuming a 350Mhz QMSS sub-system clock. */
void set_firmware_timer(Uint16 pdsp, Uint16 time);

/* This function programs base addresses for the four logical
 * queue managers that a PKTDMA may setup. Use a value of 0xffff
 * if you don't want to set value into QM base addr reg. N. */
void config_pktdma_qm(Uint32 base, Uint16 *physical_qnum);

/* This function enables/disables internal loopback mode for a pktDMA.
 * By default, it should be enabled for QMSS, disabled for all others. */
void config_pktdma_loopback(Uint32 base, Uint8 enable);

/* This function sets the packet retry timeout.
 * A value of 0 disables the retry feature. */
void config_pktdma_retry_timeout(Uint32 base, Uint16 timeout) ;

/* This function disables a TX DMA channel, then configures it. */
void config_tx_chan(Uint32 base, Uint16 chan, Uint32 return_q);

/* This function configures priority of a TX DMA channel */
void config_tx_sched(Uint32 base, Uint16 chan, Uint32 priority);

/* This function configures an RX DMA channel flow. */
void config_rx_flow(Uint32 base, Uint16 flow, Uint32 a, Uint32 b, Uint32 c,
		Uint32 d, Uint32 e, Uint32 f, Uint32 g, Uint32 h);

/* This function writes an RX DMA channel's enable register. */
void enable_rx_chan(Uint32 base, Uint16 chan, Uint32 value) ;

/* This function writes a TX DMA channel's enable register. */
void enable_tx_chan(Uint32 base, Uint16 chan, Uint32 value);

/* This function reads a QMSS INTD Status Register.
 * group parameter: 0 = high priority interrupts.
 *
 1 = low priority interrupts.
 *
 4 = PKTDMA starvation interrupts.
 *
 * If the chan parameter = 0xffffffff, the entire register contents
 * are returned. Otherwise, chan is expected to be a channel number,
 * and the return value will be a 0 or 1 for that channel's status.
 */
Uint32 read_status(Uint16 group, Uint32 chan);

/* This function writes a QMSS INTD Status Register.
 * group parameter: 0 = high priority interrupts.
 *
 1 = low priority interrupts.
 *
 4 = PKTDMA starvation interrupts.
 */
void set_status(Uint16 group, Uint32 chan);

/* This function writes a QMSS INTD Status Clear Register.
 * group parameter: 0 = high priority interrupts.
 *
 1 = low priority interrupts.
 *
 4 = PKTDMA starvation interrupts.
 */
void clear_status(Uint16 group, Uint32 chan);

/* This function reads a QMSS INTD Int Count Register.
 * Reading has no effect on the register.
 * "intnum" is: 0..31 for High Pri interrupts
 *
 32..47 for Low Pri interrupts
 *
 48..49 for PKTDMA Starvation interrupts
 */
Uint32 read_intcount(Uint16 intnum);

/* This function reads a QMSS INTD Int Count Register.
 * Writing will cause the written value to be subtracted from the register.
 * "intnum" is:
 * 0..31 for High Pri interrupts
 * 32..47 for Low Pri interrupts
 * 48..49 for PKTDMA Starvation interrupts
 */
void write_intcount(Uint16 intnum, Uint32 val);

/* This function writes a QMSS INTD EOI Register. Values to write are:
 0 or 1: PKTDMA starvation interrupts,
 2 to 33: High Pri interrupts,
 34 to 49: Low Pri interrupts.
 * Writing one of these values will clear the corresponding interrupt.
 */
void write_eoi(Uint32 val);

/* This function writes a QMSS PDSP Control Register. */
void pdsp_control(Uint16 pdsp, Uint32 val);

/* This function enables QMSS PDSP n. */
void pdsp_enable(Uint16 pdsp);

/* This function disables QMSS PDSP n. */
void pdsp_disable(Uint16 pdsp);

/* This function returns true if QMSS PDSP n is running. */
Uint8 pdsp_running(Uint16 pdsp);

/* This function controls the PDSP to load firmware to it. */
void pdsp_download_firmware(Uint16 pdsp, Uint8 *code, Uint32 size);


#endif /* QMSS_UTILS_H_ */

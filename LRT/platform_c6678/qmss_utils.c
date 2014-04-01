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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ti/csl/tistdtypes.h>
#include "qmss_addr.h"
#include "qmss_types.h"
#include "qmss_utils.h"

/* This function programs a QM memory region. */
void set_memory_region(Uint16 regn, Uint32 addr, Uint32 indx, Uint32 setup) {
	Uint32 *reg;
	reg = (Uint32 *) (QM_DESC_REGION + QM_REG_MEM_REGION_BASE + (regn * 16));
	*reg = addr;
	reg = (Uint32 *) (QM_DESC_REGION + QM_REG_MEM_REGION_INDEX + (regn * 16));
	*reg = indx;
	/* See register description for programming values. */
	reg = (Uint32 *) (QM_DESC_REGION + QM_REG_MEM_REGION_SETUP + (regn * 16));
	*reg = setup;
}


/* This function programs a QM Link RAM. */
void set_link_ram(Uint16 ram, Uint32 addr, Uint32 count) {
	Uint32 *reg;
	reg = (Uint32 *) (QM_CTRL_REGION + QM_REG_LINKRAM_0_BASE + (ram * 8));
	*reg = addr;
	if (ram == 0) {
		reg = (Uint32 *) (QM_CTRL_REGION + QM_REG_LINKRAM_0_SIZE);
		*reg = count;
	}
}


/* This function pushes descriptor info to a queue.
 * mode parameter: 1 = write reg D only.
 *
 2 = write regs C and D.
 *
 * It turns out the VBUSM is more efficient to push to than VBUSP,
 * and also allows for atomic c+d pushes.
 */
void push_queue(Uint16 qn, Uint8 mode, Uint32 c_val, Uint32 d_val) {
#ifdef USE_VBUSM
	if (mode == 2)
	{
		uint64_t *reg;
		reg = (uint64_t *)(QM_QMAN_VBUSM_REGION + QM_REG_QUE_REG_C + (qn * 16));
#ifdef _BIG_ENDIAN
		*reg = ((uint64_t)c_val << 32) | d_val;
#else
		*reg = ((uint64_t)d_val << 32) | c_val;
#endif
	}
	else
	{
		Uint32 *reg;
		reg = (Uint32 *)(QM_QMAN_VBUSM_REGION + QM_REG_QUE_REG_D + (qn * 16));
		*reg = d_val;
	}
#else
	Uint32 *reg;
	if (mode == 2) {
		reg = (Uint32 *) (QM_QMAN_REGION + QM_REG_QUE_REG_C + (qn * 16));
		*reg = c_val;
	}
	reg = (Uint32 *) (QM_QMAN_REGION + QM_REG_QUE_REG_D + (qn * 16));
	*reg = d_val;
#endif
}


/* This function pops a descriptor address from a queue. */
Uint32 pop_queue(Uint16 qn) {
	Uint32 *reg;
	Uint32 value;
	reg = (Uint32 *) (QM_QMAN_REGION + QM_REG_QUE_REG_D + (qn * 16));
	value = *reg;
	return (value);
}


/* This function moves a source queue to a destination queue. If
 * headtail = 0, the source queue is appended to the tail of the
 * dest queue. If 1, it is appended at the head. */
void divert_queue(Uint16 src_qn, Uint16 dest_qn, Uint8 headtail) {
	Uint32 *reg;
	Uint32 value;
	reg = (Uint32 *) (QM_CTRL_REGION + QM_REG_QUE_DIVERSION);
	value = (headtail << 31) + (dest_qn << 16) + src_qn;
	*reg = value;
	return;
}


/* This function pops a queue until it is empty. If *list is not NULL,
 * it will return the list of descriptor addresses and the count. */
void empty_queue(Uint16 qn, Uint32 *list, Uint32 *listCount) {
	Uint32 *reg;
	Uint32 value;
	Uint16 idx;
	Uint32 count;
	reg = (Uint32 *) (QM_PEEK_REGION + QM_REG_QUE_REG_A + (qn * 16));
	count = *reg; //read the descriptor count
	*listCount = count;
	reg = (Uint32 *) (QM_QMAN_REGION + QM_REG_QUE_REG_D + (qn * 16));
	for (idx = 0; idx < count; idx++) {
		value = *reg;
		if (list != NULL) {
			list[idx] = value;
		}
	}
}


/* This function returns the byte count of a queue. */
Uint32 get_byte_count(Uint16 qn) {
	Uint32 *reg;
	Uint32 count;
	reg = (Uint32 *) (QM_PEEK_REGION + QM_REG_QUE_REG_B + (qn * 16));
	count = *reg;
	return (count);
}


/* This function returns the descriptor count of a queue. */
Uint32 get_descriptor_count(Uint16 qn) {
	Uint32 *reg;
	Uint32 count;
	reg = (Uint32 *) (QM_PEEK_REGION + QM_REG_QUE_REG_A + (qn * 16));
	count = *reg;
	return (count);
}


/* This function sets a queue threshold for queue monitoring purposes. */
void set_queue_threshold(Uint16 qn, Uint32 value) {
	Uint32 *reg;
	reg = (Uint32 *) (QM_PEEK_REGION + QM_REG_QUE_STATUS_REG_D + (qn * 16));
	*reg = value;
}


/* This function programs a Hi or Lo Accumulator channel. */
void program_accumulator(Uint16 pdsp, Qmss_AccCmd *cmd) {
	Uint16 idx;
	Uint32 *tmplist;
	Uint32 *reg, result;
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_CMD_REGION + 4 * 4); //point to last word
	else
		reg = (Uint32 *) (PDSP2_CMD_REGION + 4 * 4); //point to last word
	tmplist = ((uint32_t *) cmd) + 4; //write first word last
	for (idx = 0; idx < 5; idx++) {
		*reg-- = *tmplist--;
	}
	/* wait for the command byte to clear. */
	reg++;
	do {
		result = (*reg & 0x0000ff00) >> 8;
	} while (result != 0);
}


/* This function disables a Hi or Lo Accumulator program. */
void disable_accumulator(Uint16 pdsp, Uint16 channel) {
	Uint16 idx;
	Uint32 *tmplist;
	Uint32 *reg;
	Qmss_AccCmd cmd;
	memset(&cmd, 0, sizeof(Qmss_AccCmd));
	cmd.channel = channel;
	cmd.command = QMSS_ACC_CMD_DISABLE;
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_CMD_REGION + 4 * 4); //point to last word
	else
		reg = (Uint32 *) (PDSP2_CMD_REGION + 4 * 4); //point to last word
	tmplist = ((uint32_t *) &cmd) + 4; //write first word last
	for (idx = 0; idx < 5; idx++) {
		*reg-- = *tmplist--;
	}
}


/* This function writes a new value to a PDSP's firmware
 * Time is specified in usec, then converted to the hardware
 * expect value assuming a 350Mhz QMSS sub-system clock. */
void set_firmware_timer(Uint16 pdsp, Uint16 time) {
	Uint32 *tmplist;
	Uint32 *reg;
	Qmss_AccCmd cmd;
	memset(&cmd, 0, sizeof(Qmss_AccCmd));
	cmd.queue_mask = (time * 175); //convert usec to hw val
	cmd.command = QMSS_ACC_CMD_TIMER;
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_CMD_REGION + 4); //point to 2nd word
	else
		reg = (Uint32 *) (PDSP2_CMD_REGION + 4); //point to 2nd word
	tmplist = ((uint32_t *) &cmd) + 1; //write 2nd word last
	*reg-- = *tmplist--;
	*reg = *tmplist;
}


/* This function programs base addresses for the four logical
 * queue managers that a PKTDMA may setup. Use a value of 0xffff
 * if you don't want to set value into QM base addr reg. N. */
void config_pktdma_qm(Uint32 base, Uint16 *physical_qnum) {
	Uint16 idx;
	Uint32 qm_base;
	Uint32 *reg;
	for (idx = 0; idx < 4; idx++) {
		if (physical_qnum[idx] != 0xffff) {
			qm_base = QM_QMAN_VBUSM_REGION + (physical_qnum[idx] * 16);
			reg = (Uint32 *) (base + PKTDMA_REG_QM0_BASE_ADDR + (idx * 4));
			*reg = qm_base;
		}
	}
}


/* This function enables/disables internal loopback mode for a pktDMA.
 * By default, it should be enabled for QMSS, disabled for all others. */
void config_pktdma_loopback(Uint32 base, Uint8 enable) {
	Uint32 *reg;
	reg = (Uint32 *) (base + PKTDMA_REG_EMULATION_CTRL);
	if (enable)
		*reg = 0x80000000;
	else
		*reg = 0x0;
}


/* This function sets the packet retry timeout.
 * A value of 0 disables the retry feature. */
void config_pktdma_retry_timeout(Uint32 base, Uint16 timeout) {
	Uint32 *reg;
	Uint32 val;
	reg = (Uint32 *) (base + PKTDMA_REG_PERFORMANCE_CTRL);
	val = *reg & 0xFFFF0000;
	*reg = val | timeout;
}


/* This function disables a TX DMA channel, then configures it. */
void config_tx_chan(Uint32 base, Uint16 chan, Uint32 return_q) {
	Uint32 *reg;
	reg = (Uint32 *) (base + PKTDMA_REG_TX_CHAN_CFG_A + (chan * 32));
	*reg = 0; //disable the channel
	reg = (Uint32 *) (base + PKTDMA_REG_TX_CHAN_CFG_B + (chan * 32));
	*reg = return_q;
}


/* This function configures priority of a TX DMA channel */
void config_tx_sched(Uint32 base, Uint16 chan, Uint32 priority) {
	Uint32 *reg;
	reg = (Uint32 *) (base + PKTDMA_REG_TX_SCHED_CHAN_CFG + (chan * 4));
	*reg = priority;
}


/* This function configures an RX DMA channel flow. */
void config_rx_flow(Uint32 base, Uint16 flow, Uint32 a, Uint32 b, Uint32 c,
		Uint32 d, Uint32 e, Uint32 f, Uint32 g, Uint32 h) {
	Uint32 *reg;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_FLOW_CFG_A + (flow * 32));
	*reg = a;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_FLOW_CFG_B + (flow * 32));
	*reg = b;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_FLOW_CFG_C + (flow * 32));
	*reg = c;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_FLOW_CFG_D + (flow * 32));
	*reg = d;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_FLOW_CFG_E + (flow * 32));
	*reg = e;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_FLOW_CFG_F + (flow * 32));
	*reg = f;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_FLOW_CFG_G + (flow * 32));
	*reg = g;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_FLOW_CFG_H + (flow * 32));
	*reg = h;
}


/* This function writes an RX DMA channel's enable register. */
void enable_rx_chan(Uint32 base, Uint16 chan, Uint32 value) {
	Uint32 *reg;
	reg = (Uint32 *) (base + PKTDMA_REG_RX_CHAN_CFG_A + (chan * 32));
	*reg = value;
}


/* This function writes a TX DMA channel's enable register. */
void enable_tx_chan(Uint32 base, Uint16 chan, Uint32 value) {
	Uint32 *reg;
	reg = (Uint32 *) (base + PKTDMA_REG_TX_CHAN_CFG_A + (chan * 32));
	*reg = value;
}


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
Uint32 read_status(Uint16 group, Uint32 chan) {
	Uint32 *reg;
	Uint32 value;
	Uint32 mask;
	reg = (Uint32 *) (QM_INTD_REGION + QM_REG_INTD_STATUS + (group * 4));
	value = *reg;
	if (chan != 0xffffffff) {
		mask = 1 << (chan & 0x001f);
		if ((value & mask) == 0)
			value = 0;
		else
			value = 1;
	}
	return (value);
}


/* This function writes a QMSS INTD Status Register.
 * group parameter: 0 = high priority interrupts.
 *
 1 = low priority interrupts.
 *
 4 = PKTDMA starvation interrupts.
 */
void set_status(Uint16 group, Uint32 chan) {
	Uint32 *reg;
	Uint32 value;
	Uint32 mask;
	reg = (Uint32 *) (QM_INTD_REGION + QM_REG_INTD_STATUS + (group * 4));
	value = *reg;
	mask = 1 << (chan & 0x001f);
	value |= mask;
	*reg = value;
}


/* This function writes a QMSS INTD Status Clear Register.
 * group parameter: 0 = high priority interrupts.
 *
 1 = low priority interrupts.
 *
 4 = PKTDMA starvation interrupts.
 */
void clear_status(Uint16 group, Uint32 chan) {
	Uint32 *reg;
	Uint32 value;
	Uint32 mask;
	reg = (Uint32 *) (QM_INTD_REGION + QM_REG_INTD_STATUS_CLEAR + (group * 4));
	value = *reg;
	mask = 1 << (chan & 0x001f);
	value |= mask;
	*reg = value;
}


/* This function reads a QMSS INTD Int Count Register.
 * Reading has no effect on the register.
 * "intnum" is: 0..31 for High Pri interrupts
 *
 32..47 for Low Pri interrupts
 *
 48..49 for PKTDMA Starvation interrupts
 */
Uint32 read_intcount(Uint16 intnum) {
	Uint32 *reg;
	Uint32 value;
	reg = (Uint32 *) (QM_INTD_REGION + QM_REG_INTD_INT_COUNT + (intnum * 4));
	value = *reg;
	return (value);
}


/* This function reads a QMSS INTD Int Count Register.
 * Writing will cause the written value to be subtracted from the register.
 * "intnum" is: 0..31 for High Pri interrupts
 *
 32..47 for Low Pri interrupts
 *
 48..49 for PKTDMA Starvation interrupts
 */
void write_intcount(Uint16 intnum, Uint32 val) {
	Uint32 *reg;
	reg = (Uint32 *) (QM_INTD_REGION + QM_REG_INTD_INT_COUNT + (intnum * 4));
	*reg = val;
}


/* This function writes a QMSS INTD EOI Register. Values to write are:
 0 or 1: PKTDMA starvation interrupts,
 2 to 33: High Pri interrupts,
 34 to 49: Low Pri interrupts.
 * Writing one of these values will clear the corresponding interrupt.
 */
void write_eoi(Uint32 val) {
	Uint32 *reg;
	reg = (Uint32 *) (QM_INTD_REGION + QM_REG_INTD_EOI);
	*reg = val;
}

/* This function writes a QMSS PDSP Control Register. */
void pdsp_control(Uint16 pdsp, Uint32 val) {
	Uint32 *reg;
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_REG_REGION + QM_REG_PDSP_CONTROL);
	else
		reg = (Uint32 *) (PDSP2_REG_REGION + QM_REG_PDSP_CONTROL);
	*reg = val;
}


/* This function enables QMSS PDSP n. */
void pdsp_enable(Uint16 pdsp) {
	Uint32 *reg;
	Uint32 tmp;
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_REG_REGION + QM_REG_PDSP_CONTROL);
	else
		reg = (Uint32 *) (PDSP2_REG_REGION + QM_REG_PDSP_CONTROL);
	tmp = *reg;
	tmp |= 0x02;
	*reg = tmp;
}


/* This function disables QMSS PDSP n. */
void pdsp_disable(Uint16 pdsp) {
	Uint32 *reg;
	Uint32 tmp;
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_REG_REGION + QM_REG_PDSP_CONTROL);
	else
		reg = (Uint32 *) (PDSP2_REG_REGION + QM_REG_PDSP_CONTROL);
	tmp = *reg;
	tmp &= 0xfffffffd;
	*reg = tmp;
}


/* This function returns true if QMSS PDSP n is running. */
Uint8 pdsp_running(Uint16 pdsp) {
	Uint32 *reg;
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_REG_REGION + QM_REG_PDSP_CONTROL);
	else
		reg = (Uint32 *) (PDSP2_REG_REGION + QM_REG_PDSP_CONTROL);
	return (*reg & 0x00008000);
}


/* This function controls the PDSP to load firmware to it. */
void pdsp_download_firmware(Uint16 pdsp, Uint8 *code, Uint32 size) {
	Uint16 idx;
	Uint32 value;
	Uint32 *reg;
	/* Reset PDSP 1 */
	pdsp_disable(pdsp);
	/* Confirm PDSP has halted */
	do {
		value = pdsp_running(pdsp);
	} while (value == 1);
	/* Download the firmware */
	if (pdsp == 1)
		memcpy((void *) PDSP1_IRAM_REGION, code, size);
	else
		memcpy((void *) PDSP2_IRAM_REGION, code, size);
	/* Use the command register to sync the PDSP */
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_CMD_REGION);
	else
		reg = (Uint32 *) (PDSP2_CMD_REGION);
	*reg = 0xffffffff;
	/* Wait to the memory write to land */
	for (idx = 0; idx < 20000; idx++) {
		value = *reg;
		if (value == 0xffffffff)
			break;
	}
	/* Reset program counter to zero, and clear Soft Reset bit. */
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_REG_REGION + QM_REG_PDSP_CONTROL);
	else
		reg = (Uint32 *) (PDSP2_REG_REGION + QM_REG_PDSP_CONTROL);
	value = *reg;
	*reg = value & 0x0000fffe; //PC reset is in upper 16 bits, soft reset in bit 0
			/* Enable the PDSP */
	pdsp_enable(pdsp);
	/* Wait for the command register to clear */
	if (pdsp == 1)
		reg = (Uint32 *) (PDSP1_CMD_REGION);
	else
		reg = (Uint32 *) (PDSP2_CMD_REGION);
	do {
		value = *reg;
	} while (value != 0);
}

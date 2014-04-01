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

#ifndef QMSS_ADDR_H_
#define QMSS_ADDR_H_

#define QMSS_CFG_BASE	(0x02a00000u)
#define QMSS_VBUSM_BASE	(0x34000000u)
#define SRIO_CFG_BASE	(0x02900000u)
#define PASS_CFG_BASE	(0x02000000u)
#define FFTCA_CFG_BASE	(0x021f0000u)
#define FFTCB_CFG_BASE	(0x021f4000u)
#define AIF_CFG_BASE	(0x01f00000u)

/* Define QMSS Register block regions. */
#define QM_CTRL_REGION 			(QMSS_CFG_BASE 	+ 0x00068000u)
#define QM_DESC_REGION			(QMSS_CFG_BASE 	+ 0x0006a000u)
#define QM_QMAN_REGION 			(QMSS_CFG_BASE 	+ 0x00020000u)
#define QM_QMAN_VBUSM_REGION 	(QMSS_VBUSM_BASE + 0x00020000u)
#define QM_PEEK_REGION			(QMSS_CFG_BASE 	+ 0x00000000u)
#define QM_LRAM_REGION			(				+ 0x00080000u)
#define QM_INTD_REGION			(QMSS_CFG_BASE 	+ 0x000a0000u)
#define QM_PROXY_REGION			(QMSS_CFG_BASE + 0x00040000u)
#define PDSP1_CMD_REGION		(QMSS_CFG_BASE + 0x000b8000u)
#define PDSP2_CMD_REGION		(QMSS_CFG_BASE + 0x000bc000u)
#define PDSP1_REG_REGION		(QMSS_CFG_BASE + 0x0006E000u)
#define PDSP2_REG_REGION		(QMSS_CFG_BASE + 0x0006F000u)
#define PDSP1_IRAM_REGION		(QMSS_CFG_BASE + 0x00060000u)
#define PDSP2_IRAM_REGION		(QMSS_CFG_BASE + 0x00061000u)

/* Define QMSS PKTDMA Register block regions. */
#define QMSS_PKTDMA_GBL_CFG_REGION	(QMSS_CFG_BASE	+ 0x0006c000u)
#define QMSS_PKTDMA_TX_CHAN_REGION	(QMSS_CFG_BASE 	+ 0x0006c400u)
#define QMSS_PKTDMA_RX_CHAN_REGION	(QMSS_CFG_BASE 	+ 0x0006c800u)
#define QMSS_PKTDMA_TX_SCHD_REGION	(QMSS_CFG_BASE 	+ 0x0006cc00u)
#define QMSS_PKTDMA_RX_FLOW_REGION	(QMSS_CFG_BASE  + 0x0006d000u)

/* Define PASS PKTDMA Register block regions. */
#define PASS_PKTDMA_GBL_CFG_REGION	(PASS_CFG_BASE  + 0x00004000u)
#define PASS_PKTDMA_TX_CHAN_REGION	(PASS_CFG_BASE  + 0x00004400u)
#define PASS_PKTDMA_RX_CHAN_REGION	(PASS_CFG_BASE  + 0x00004800u)
#define PASS_PKTDMA_TX_SCHD_REGION	(PASS_CFG_BASE  + 0x00004c00u)
#define PASS_PKTDMA_RX_FLOW_REGION	(PASS_CFG_BASE  + 0x00005000u)

/* Define SRIO PKTDMA Register block regions. */
#define SRIO_PKTDMA_GBL_CFG_REGION	(SRIO_CFG_BASE 	+ 0x00001000u)
#define SRIO_PKTDMA_TX_CHAN_REGION	(SRIO_CFG_BASE 	+ 0x00001400u)
#define SRIO_PKTDMA_RX_CHAN_REGION	(SRIO_CFG_BASE 	+ 0x00001800u)
#define SRIO_PKTDMA_TX_SCHD_REGION	(SRIO_CFG_BASE 	+ 0x00001c00u)
#define SRIO_PKTDMA_RX_FLOW_REGION	(SRIO_CFG_BASE 	+ 0x00002000u)

/* Define FFTC A PKTDMA Register block regions. */
#define FFTCA_PKTDMA_GBL_CFG_REGION	(FFTCA_CFG_BASE + 0x00000200u)
#define FFTCA_PKTDMA_TX_CHAN_REGION	(FFTCA_CFG_BASE + 0x00000400u)
#define FFTCA_PKTDMA_RX_CHAN_REGION	(FFTCA_CFG_BASE + 0x00000500u)
#define FFTCA_PKTDMA_TX_SCHD_REGION	(FFTCA_CFG_BASE + 0x00000300u)
#define FFTCA_PKTDMA_RX_FLOW_REGION	(FFTCA_CFG_BASE + 0x00000600u)

/* Define FFTC B PKTDMA Register block regions. */
#define FFTCB_PKTDMA_GBL_CFG_REGION	(FFTCB_CFG_BASE + 0x00000200u)
#define FFTCB_PKTDMA_TX_CHAN_REGION	(FFTCB_CFG_BASE + 0x00000400u)
#define FFTCB_PKTDMA_RX_CHAN_REGION	(FFTCB_CFG_BASE + 0x00000500u)
#define FFTCB_PKTDMA_TX_SCHD_REGION	(FFTCB_CFG_BASE + 0x00000300u)
#define FFTCB_PKTDMA_RX_FLOW_REGION	(FFTCB_CFG_BASE + 0x00000600u)

/* Define AIF PKTDMA Register block regions. */
#define AIF_PKTDMA_GBL_CFG_REGION	(AIF_CFG_BASE 	+ 0x00014000u)
#define AIF_PKTDMA_TX_CHAN_REGION	(AIF_CFG_BASE 	+ 0x00016000u)
#define AIF_PKTDMA_RX_CHAN_REGION	(AIF_CFG_BASE 	+ 0x00018000u)
//#define AIF_PKTDMA_TX_SCHD_REGION	(AIF_CFG_BASE 	+ 0x00000000u)
#define AIF_PKTDMA_RX_FLOW_REGION	(AIF_CFG_BASE 	+ 0x0001a000u)

/**********************************************************************
* Define offsets to individual QM registers within an address region.
***********************************************************************/

/* Queue Manager Region */
#define QM_REG_QUE_REVISION		0x000
#define QM_REG_QUE_DIVERSION	0x008
#define QM_REG_STARVATION_CNT	0x020
#define QM_REG_LINKRAM_0_BASE	0x00c
#define QM_REG_LINKRAM_0_SIZE	0x010
#define QM_REG_LINKRAM_1_BASE	0x014

/* Descriptor Memory Region */
#define QM_REG_MEM_REGION_BASE	0x000
#define QM_REG_MEM_REGION_INDEX	0x004
#define QM_REG_MEM_REGION_SETUP 0x008

/* Queue Management Region */
#define QM_REG_QUE_REG_A		0x000
#define QM_REG_QUE_REG_B		0x004
#define QM_REG_QUE_REG_C		0x008
#define QM_REG_QUE_REG_D 		0x00c

/* Queue Status Region */
#define QM_REG_QUE_STATUS_REG_A		0x000
#define QM_REG_QUE_STATUS_REG_B		0x004
#define QM_REG_QUE_STATUS_REG_C		0x008
#define QM_REG_QUE_STATUS_REG_D 	0x00c

/* Interrupt Distributor (INTD) Region */
#define QM_REG_INTD_REVISION		0x000
#define QM_REG_INTD_EOI				0x010
#define QM_REG_INTD_STATUS 			0x200
#define QM_REG_INTD_STATUS_CLEAR 	0x280
#define QM_REG_INTD_INT_COUNT		0x300

/* PDSP(n) Reg Region */
#define QM_REG_PDSP_CONTROL			0x000
#define QM_REG_PDSP_STATUS			0x004
#define QM_REG_PDSP_CYCLE_COUNT		0x00c
#define QM_REG_PDSP_STALL_COUNT		0x010

/**********************************************************************
* Define offsets to individual PKTDMA registers within an address region.
***********************************************************************/

/* Global Cfg Register Block */
#define PKTDMA_REG_REVISION			0x000
#define PKTDMA_REG_PERFORMANCE_CTRL 0x004
#define PKTDMA_REG_EMULATION_CTRL	0x008
#define PKTDMA_REG_PRIORITY_CTRL	0x00c
#define PKTDMA_REG_QM0_BASE_ADDR	0x010
#define PKTDMA_REG_QM1_BASE_ADDR	0x014
#define PKTDMA_REG_QM2_BASE_ADDR	0x018
#define PKTDMA_REG_QM3_BASE_ADDR	0x01c

/* Tx Chan Cfg Register Block */
#define PKTDMA_REG_TX_CHAN_CFG_A	0x000
#define PKTDMA_REG_TX_CHAN_CFG_B	0x004

/* Rx Chan Cfg Register Block */
#define PKTDMA_REG_RX_CHAN_CFG_A 	0x000

/* Rx Flow Cfg Register Block */
#define PKTDMA_REG_RX_FLOW_CFG_A	0x000
#define PKTDMA_REG_RX_FLOW_CFG_B	0x004
#define PKTDMA_REG_RX_FLOW_CFG_C	0x008
#define PKTDMA_REG_RX_FLOW_CFG_D	0x00c
#define PKTDMA_REG_RX_FLOW_CFG_E	0x010
#define PKTDMA_REG_RX_FLOW_CFG_F	0x014
#define PKTDMA_REG_RX_FLOW_CFG_G	0x018
#define PKTDMA_REG_RX_FLOW_CFG_H 	0x01c

/* Tx Sched Cfg Register Block */
#define PKTDMA_REG_TX_SCHED_CHAN_CFG 0x000

#endif /* QMSS_ADDR_H_ */

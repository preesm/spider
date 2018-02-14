/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2016) :
 *
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include <ti/csl/cslr_tmr.h>
#include <ti/csl/cslr_fftc.h>
#include <ti/drv/qmss/qmss_drv.h>
#include <ti/drv/cppi/cppi_drv.h>
#include <ti/drv/cppi/cppi_desc.h>
#include <ti/csl/csl_qm_queue.h>

#define PACKET_HEADER 12
#define CACHE_LINESZ 64
#define align(x)   ((x + CACHE_LINESZ) & (~CACHE_LINESZ))

#define TranslateAddress(address, offset, type) address = (type)(((uint8_t*)address) + (uint32_t)offset)


#ifdef DEVICE_K2H
#define MSMC_SIZE 		0x00600000
#define QMSS_CFG_SIZE 	0x00200000
#define QMSS_DATA_SIZE 	0x00100000
#define TMR_REGS_SIZE 	0x00010000

#define DDR_BASE 		0xD0000000
#define DDR_SIZE 		0x10000000

#define CPPI_BASE_REG	0x01F00000
#define CPPI_SIZE_REG	0x01100000

#else
#error Please define sizes in platformK2Arm/init_hw.c
#endif

typedef enum{
	DATA_REG_NUM  = Qmss_MemRegion_MEMORY_REGION0,
	CTRL_REG_NUM  = Qmss_MemRegion_MEMORY_REGION1,
	TRACE_REG_NUM = Qmss_MemRegion_MEMORY_REGION2,
	FFTC_REG_NUM  = Qmss_MemRegion_MEMORY_REGION3
}RegionNumber;

#define DATA_DESC_SIZE	64
#define DATA_DESC_NUM	512

#define CTRL_DESC_SIZE	512
#define CTRL_DESC_NUM	512

#define TRACE_DESC_SIZE	64
#define TRACE_DESC_NUM	512

#define FFTC_DESC_SIZE	64
#define FFTC_DESC_NUM	32

/* QUEUES */
typedef enum{
	/* Special Queues */
	QUEUE_TX_FFTC_A		= QMSS_FFTC_A_QUEUE_BASE,
	QUEUE_TX_FFTC_B		= QMSS_FFTC_B_QUEUE_BASE,

	/* Bounds */
	QUEUE_FIRST			= QMSS_GENERAL_PURPOSE_QUEUE_BASE,
	QUEUE_LAST			= 4000,//QMSS_MAX_GENERAL_PURPOSE_QUEUE

	/* General Purpose Queues */
	QUEUE_FREE_DATA		= QMSS_GENERAL_PURPOSE_QUEUE_BASE, /*896*/
	QUEUE_FREE_CTRL,
	QUEUE_FREE_TRACE,

	QUEUE_FREE_FFTC,
	QUEUE_FREE_RX_FFTC_A,
	QUEUE_RX_FFTC_A,
	QUEUE_FREE_RX_FFTC_B,
	QUEUE_RX_FFTC_B,

	QUEUE_TRACE,

	/* GRT -> LRT */
	QUEUE_CTRL_DOWN_0,
	QUEUE_CTRL_DOWN_1,
	QUEUE_CTRL_DOWN_2,
	QUEUE_CTRL_DOWN_3,
	QUEUE_CTRL_DOWN_4,
	QUEUE_CTRL_DOWN_5,
	QUEUE_CTRL_DOWN_6,
	QUEUE_CTRL_DOWN_7,
	QUEUE_CTRL_DOWN_8,
	QUEUE_CTRL_DOWN_9,
	QUEUE_CTRL_DOWN_10,
	QUEUE_CTRL_DOWN_11,

	/* LRT -> GRT */
	QUEUE_CTRL_UP_0,
	QUEUE_CTRL_UP_1,
	QUEUE_CTRL_UP_2,
	QUEUE_CTRL_UP_3,
	QUEUE_CTRL_UP_4,
	QUEUE_CTRL_UP_5,
	QUEUE_CTRL_UP_6,
	QUEUE_CTRL_UP_7,
	QUEUE_CTRL_UP_8,
	QUEUE_CTRL_UP_9,
	QUEUE_CTRL_UP_10,
	QUEUE_CTRL_UP_11,

	QUEUE_FFTC_RES,
	QUEUE_DATA_BASE
} QUEUE_ID;

extern int qmss_cfg_regs;
extern int msmc_mem_base;
extern int ddr_mem_base;
extern int data_mem_base;
extern CSL_FftcRegs* fftc_a_cfg_regs;
extern CSL_FftcRegs* fftc_b_cfg_regs;
extern void* qm_regs;
extern CSL_TmrRegsOvly tmr_regs;

#define CPPI_BASE_REG	0x01F00000
#define CPPI_SIZE_REG	0x01100000
extern void* cppi_regs;

void init_hw();
void init_qmss(int useMsmc);
void init_cppi();
void init_fftc();

void printQueueState();

void clean_fftc();
void clean_cppi();
void clean_qmss();
void clean_hw();

#endif//INIT_H

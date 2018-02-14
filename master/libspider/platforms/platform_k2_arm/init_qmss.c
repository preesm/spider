/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2015 - 2016) :
 *
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2015 - 2016)
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
#include "init.h"
#include "osal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ti/drv/qmss/qmss_drv.h>
#include <ti/drv/qmss/qmss_firmware.h>
#include <ti/drv/cppi/cppi_desc.h>

#include <ti/csl/cslr_device.h>
#include <ti/drv/cppi/cppi_drv.h>
#include <ti/drv/qmss/qmss_drv.h>

static Cppi_Handle hCppi[2];
static Cppi_ChHnd hCppiRxChan[2];
static Cppi_ChHnd hCppiTxChan[2];
static Cppi_FlowHnd hCppiRxFlow[2];

int data_mem_base;

/* Default QMSS Driver global config */
extern Qmss_GlobalConfigParams qmssGblCfgParams;
/* CPPI device specific configuration */
extern Cppi_GlobalConfigParams cppiGblCfgParams;

static inline int setup_region(
			uint32_t* 		descBase,
			int 			descSize,
			int				descNum,
			int				startIndex,
			RegionNumber 	descRegion
		){
    Qmss_MemRegInfo qmss_regionCfg;
	Qmss_Result 	qmss_result;

	memset(&qmss_regionCfg, 0, sizeof(qmss_regionCfg));
	memset(descBase, 0, descSize*descNum);

	qmss_regionCfg.descBase 		= descBase;
	qmss_regionCfg.descSize 		= descSize;
	qmss_regionCfg.descNum 			= descNum;
	qmss_regionCfg.manageDescFlag 	= Qmss_ManageDesc_UNMANAGED_DESCRIPTOR;
	qmss_regionCfg.memRegion 		= (Qmss_MemRegion)descRegion;
	qmss_regionCfg.startIndex 		= startIndex;

	qmss_result = Qmss_insertMemoryRegion (&qmss_regionCfg);
	if (qmss_result != descRegion){
		printf ("Error : Inserting Data memory region %d error code : %d\n", qmss_regionCfg.memRegion, qmss_result);
		abort();
	}
	printf ("Data Memory region %d inserted\n", qmss_result);
	return 0;
}

void printQueueState(){
	int Queue;

	printf("\nQueue State:\n");
	/* Special Queues */
	if(Qmss_getQueueEntryCount(QUEUE_TX_FFTC_A) > 0){
		printf("%4d: %d\n", QUEUE_TX_FFTC_A, Qmss_getQueueEntryCount(QUEUE_TX_FFTC_A));
	}
	if(Qmss_getQueueEntryCount(QUEUE_TX_FFTC_B) > 0){
		printf("%4d: %d\n", QUEUE_TX_FFTC_B, Qmss_getQueueEntryCount(QUEUE_TX_FFTC_B));
	}

	/* General Purpose Queues */
	for(Queue = 0; Queue < QUEUE_LAST; Queue++){
		if(Qmss_getQueueEntryCount(Queue) > 0){
			printf("%4d: %d\n", Queue, Qmss_getQueueEntryCount(Queue));
		}
	}
	printf("\n");
}

static void configureRxFlow(int fftc_ix){
	uint8_t isAllocated;
	int rxQueue, freeRxQueue;
	Cppi_RxFlowCfg rxFlowCfg;

	switch(fftc_ix){
	case 0:
		rxQueue = QUEUE_RX_FFTC_A;
		freeRxQueue = QUEUE_FREE_RX_FFTC_A;
		break;
	case 1:
		rxQueue = QUEUE_RX_FFTC_B;
		freeRxQueue = QUEUE_FREE_RX_FFTC_B;
		break;
	}

	/* Initialize the flow configuration */
	memset (&rxFlowCfg, 0, sizeof(Cppi_RxFlowCfg));

	/* Setup a Rx Flow for this Rx object */
	rxFlowCfg.flowIdNum             =   0;
	rxFlowCfg.rx_dest_qmgr          =   0;
	rxFlowCfg.rx_dest_qnum          =   rxQueue;
	rxFlowCfg.rx_desc_type          =   Cppi_DescType_HOST;
	rxFlowCfg.rx_ps_location    	=   Cppi_PSLoc_PS_IN_DESC;
	rxFlowCfg.rx_psinfo_present     =   0;

	rxFlowCfg.rx_error_handling     =   0;    /* Drop the packet, do not retry on starvation by default */
	rxFlowCfg.rx_einfo_present      =   0;    /* By default no EPIB info */

	rxFlowCfg.rx_dest_tag_lo_sel    =   4;    /* Always pick the dest tag 7:0 bits from the PD dest_tag */
	rxFlowCfg.rx_dest_tag_hi_sel    =   5;    /* Always pick the dest tag 15:8 bits from the PD dest_tag */

	rxFlowCfg.rx_src_tag_lo_sel     =   2;    /* Always pick the src tag 7:0 bits from the PD flow_id 7:0 */
	rxFlowCfg.rx_src_tag_hi_sel     =   4;    /* Always pick the src tag 15:8 bits from the PD src_tag 7:0 */

	/* By default, we disable Rx Thresholds */
	rxFlowCfg.rx_size_thresh0_en    =   0;
	rxFlowCfg.rx_size_thresh1_en    =   0;
	rxFlowCfg.rx_size_thresh2_en    =   0;

	rxFlowCfg.rx_size_thresh0       =   0x0;
	rxFlowCfg.rx_size_thresh1       =   0x0;
	rxFlowCfg.rx_size_thresh2       =   0x0;

	rxFlowCfg.rx_fdq0_sz0_qmgr      =   0; /* Setup the Receive free queue for the flow */
	rxFlowCfg.rx_fdq0_sz0_qnum      =   freeRxQueue;
	rxFlowCfg.rx_fdq0_sz1_qnum      =   0x0;
	rxFlowCfg.rx_fdq0_sz1_qmgr      =   0x0;
	rxFlowCfg.rx_fdq0_sz2_qnum      =   0x0;
	rxFlowCfg.rx_fdq0_sz2_qmgr      =   0x0;
	rxFlowCfg.rx_fdq0_sz3_qnum      =   0x0;
	rxFlowCfg.rx_fdq0_sz3_qmgr      =   0x0;

	rxFlowCfg.rx_fdq1_qnum          =   freeRxQueue;  /* Use the Rx Queue to pick descriptors */
	rxFlowCfg.rx_fdq1_qmgr          =   0;
	rxFlowCfg.rx_fdq2_qnum          =   freeRxQueue;  /* Use the Rx Queue to pick descriptors */
	rxFlowCfg.rx_fdq2_qmgr          =   0;
	rxFlowCfg.rx_fdq3_qnum          =   freeRxQueue;  /* Use the Rx Queue to pick descriptors */
	rxFlowCfg.rx_fdq3_qmgr          =   0;

	/* Configure the Rx flow */
	if ((hCppiRxFlow[fftc_ix] = Cppi_configureRxFlow (hCppi[fftc_ix], &rxFlowCfg, &isAllocated)) == NULL){
		printf ("Error configuring Rx flow \n");
	}
}


static void configureTxChan(int fftc_ix){
	uint8_t isAllocated;

	/* If this is the first time we are initializing this the FFTC transmit queue,
	 * setup the configuration, otherwise just increment the reference count
	 * and return the queue handle if its being opened in "shared" mode.
	 */
	Cppi_TxChInitCfg txChCfg;
	txChCfg.channelNum      =   0;       /* CPPI channels are mapped one-one to the FFTC Tx queues */
	txChCfg.txEnable        =   Cppi_ChState_CHANNEL_DISABLE;  /* Disable the channel for now. */
	txChCfg.filterEPIB      =   0;
	txChCfg.filterPS        =   0;
	txChCfg.aifMonoMode     =   0;

	/* Get the FFTC Control register contents */
	txChCfg.priority        =   0;//CSL_FEXT (fftc_cfg_regs->CONFIG, FFTC_CONFIG_QUEUE_0_PRIORITY);

	/* Open a CPPI Tx Channel corresponding to the FFTC queue specified.
	 * In the FFTC H/w:     CPPI Channel 0 mapped to FFTC Tx Queue 0
	 *                      CPPI Channel 1 mapped to FFTC Tx Queue 1
	 *                      CPPI Channel 2 mapped to FFTC Tx Queue 2
	 *                      CPPI Channel 3 mapped to FFTC Tx Queue 3
	 */
	if ((hCppiTxChan[fftc_ix] = Cppi_txChannelOpen (hCppi[fftc_ix], &txChCfg, &isAllocated)) == NULL)	{
		printf ("Error opening Tx channel corresponding to FFTC queue : %d\n", 0);
	}
}

static void configureRxChan(int fftc_ix){
	uint8_t isAllocated;

	/* The Rx channel is mapped one to one with Tx channel.
	 * We need to open and enable this one too.
	 */
	Cppi_RxChInitCfg rxChInitCfg;
	rxChInitCfg.channelNum          = 0;
	rxChInitCfg.rxEnable            = Cppi_ChState_CHANNEL_DISABLE;
	if ((hCppiRxChan[fftc_ix] = Cppi_rxChannelOpen (hCppi[fftc_ix], &rxChInitCfg, &isAllocated)) == NULL)	{
		printf ("Error opening Rx channel: %d \n", 0);
	}
}

void setFFTSize(int id, int size){

	static int init = 0;

	if(!init){
		init = 1;

	CSL_FftcRegs* regs;

	switch(id){
	case 0:
		regs = fftc_a_cfg_regs;
		break;
	case 1:
		regs = fftc_b_cfg_regs;
		break;
	default:
		printf("Unhandled case in setFFTSize\n");
	}

	//int logSize = log2(size)-2;
	int logSize = 16;

	regs->Q0_SCALE_SHIFT =
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_DYNAMIC_SCALING_ENABLE, 	0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_OUTPUT_SCALING, 			16)  |
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_OUT_SCALING, 		0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_6_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_5_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_4_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_3_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_2_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_1_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_0_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_LTE_SHIFT_SCALING, 	0);

	/* Step 4. Configure Queue 'n' Control Register */
	regs->Q0_CONTROL =
		CSL_FMK (FFTC_Q0_CONTROL_SUPPRESSED_SIDE_INFO, 	1) |
		CSL_FMK (FFTC_Q0_CONTROL_DFT_IDFT_SELECT, 		1) |  /* DFT */
		CSL_FMK (FFTC_Q0_CONTROL_ZERO_PAD_MODE, 		0) |
		CSL_FMK (FFTC_Q0_CONTROL_ZERO_PAD_VAL, 			0) |
		CSL_FMK (FFTC_Q0_CONTROL_DFT_SIZE, 				logSize);

	}
}

void configureFFTRegs(CSL_FftcRegs* regs){
	/* The FFTC Local Configuration consists of configuration of the following
	 * queue specific registers:
	 *  1. FFTC Queue x Destination Queue Register
	 *  2. FFTC Queue x Scaling & Shifting Register
	 *  3. FFTC Queue x Cyclic Prefix Register
	 *  4. FFTC Queue x Control Register
	 *  5. FFTC Queue x LTE Frequency Shift Register
	 */
	/* Step 1. Configure the FFTC Queue 'n' Destination Queue Register  */
	regs->Q0_DEST =
			CSL_FMK (FFTC_Q0_DEST_FFTC_SHIFT_LEFT_RIGHT_OUTPUT, 0) |
			CSL_FMK (FFTC_Q0_DEST_FFTC_SHIFT_LEFT_RIGHT_INPUT, 	0) |
			CSL_FMK (FFTC_Q0_DEST_FFTC_VARIABLE_SHIFT_INPUT, 	0) |
			CSL_FMK (FFTC_Q0_DEST_DEFAULT_DEST, 				0x3FFF);

	/* Step 2. Configure the Queue 'n' Scaling and Shift Register configuration. */
	regs->Q0_SCALE_SHIFT =
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_DYNAMIC_SCALING_ENABLE, 	0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_OUTPUT_SCALING, 			2)  |
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_OUT_SCALING, 		0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_6_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_5_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_4_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_3_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_2_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_1_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_0_SCALING, 			0) 	|
			CSL_FMK (FFTC_Q0_SCALE_SHIFT_STAGE_LTE_SHIFT_SCALING, 	0);

	/* Step 3. Configure Queue 'n' Cyclic Prefix Register */
	regs->Q0_CYCLIC_PREFIX =
			CSL_FMK (FFTC_Q0_CYCLIC_PREFIX_CYCLIC_PREFIX_REMOVE_EN, 	0) |
			CSL_FMK (FFTC_Q0_CYCLIC_PREFIX_CYCLIC_PREFIX_REMOVE_OFFSET, 0) |
			CSL_FMK (FFTC_Q0_CYCLIC_PREFIX_CYCLIC_PREFIX_ADDITION, 		0);

	/* Step 4. Configure Queue 'n' Control Register */
	regs->Q0_CONTROL =
			CSL_FMK (FFTC_Q0_CONTROL_SUPPRESSED_SIDE_INFO, 	1) |
			CSL_FMK (FFTC_Q0_CONTROL_DFT_IDFT_SELECT, 		1) |  /* DFT */
			CSL_FMK (FFTC_Q0_CONTROL_ZERO_PAD_MODE, 		0) |
			CSL_FMK (FFTC_Q0_CONTROL_ZERO_PAD_VAL, 			0) |
			CSL_FMK (FFTC_Q0_CONTROL_DFT_SIZE, 				6); /* 6 -> 256 */

	/* Step 5. Finally configure the Queue 'n' LTE Frequency Shift Register */
	regs->Q0_LTE_FREQ =
			CSL_FMK (FFTC_Q0_LTE_FREQ_LTE_FREQ_SHIFT_EN, 		0) |
			CSL_FMK (FFTC_Q0_LTE_FREQ_LTE_FREQ_SHIFT_DIR, 		0) |
			CSL_FMK (FFTC_Q0_LTE_FREQ_LTE_FREQ_SHIFT_FACTOR, 	0) |
			CSL_FMK (FFTC_Q0_LTE_FREQ_LTE_FREQ_SHIFT_PHASE, 	0) |
			CSL_FMK (FFTC_Q0_LTE_FREQ_LTE_FREQ_SHIFT_INDEX, 	0);
}

/**
 * Init Queue Manager SUbSystem (QMSS)
 *  - Configure QMSS Driver
 * 	- Define Memory regions
 * 	-
 */
void init_qmss(int useMsmc){
	int i, result;
    Qmss_InitCfg 			qmss_initCfg;
	Cppi_CpDmaInitCfg 		cpdmaCfg;
	Qmss_GlobalConfigParams qmss_globalCfg;

    /* Descriptor base addresses */
    void* data_desc_base  = (void*)align((int)msmc_mem_base);
    void* ctrl_desc_base  = (void*)align((int)data_desc_base + DATA_DESC_NUM*DATA_DESC_SIZE);
    void* trace_desc_base = (void*)align((int)ctrl_desc_base + CTRL_DESC_NUM*CTRL_DESC_SIZE);
    void* fftc_desc_base  = (void*)align((int)trace_desc_base + TRACE_DESC_NUM*TRACE_DESC_SIZE);

    if(useMsmc){
    	data_mem_base = align((int)fftc_desc_base + FFTC_DESC_NUM*FFTC_DESC_SIZE);
    }else{
    	data_mem_base = align((int)ddr_mem_base);
    }

    /* Initialize QMSS Driver */
    memset (&qmss_initCfg, 0, sizeof (Qmss_InitCfg));

    /* Use internal linking RAM */
	qmss_initCfg.linkingRAM0Base  = 0;
	qmss_initCfg.linkingRAM0Size  = 0;
	qmss_initCfg.linkingRAM1Base  = 0;
	qmss_initCfg.maxDescNum       = DATA_DESC_NUM + CTRL_DESC_NUM + TRACE_DESC_NUM + FFTC_DESC_NUM;

	qmss_initCfg.pdspFirmware[0].pdspId   = Qmss_PdspId_PDSP1;
	qmss_initCfg.pdspFirmware[0].firmware = &acc48_le;
	qmss_initCfg.pdspFirmware[0].size     = sizeof (acc48_le);

    /* Bypass hardware initialization as it is done within Kernel */
    qmss_initCfg.qmssHwStatus     =   QMSS_HW_INIT_COMPLETE;

    qmss_globalCfg = qmssGblCfgParams;
    /* Convert address to Virtual address */
	for(i=0;i < (int)qmss_globalCfg.maxQueMgrGroups;i++){
		TranslateAddress(qmss_globalCfg.groupRegs[i].qmConfigReg,       qmss_cfg_regs-CSL_QMSS_CFG_BASE,    CSL_Qm_configRegs*);
		TranslateAddress(qmss_globalCfg.groupRegs[i].qmDescReg,         qmss_cfg_regs-CSL_QMSS_CFG_BASE,    CSL_Qm_descriptor_region_configRegs*);
		TranslateAddress(qmss_globalCfg.groupRegs[i].qmQueMgmtReg,      qmss_cfg_regs-CSL_QMSS_CFG_BASE,    CSL_Qm_queue_managementRegs*);
		TranslateAddress(qmss_globalCfg.groupRegs[i].qmQueMgmtProxyReg, qmss_cfg_regs-CSL_QMSS_CFG_BASE,    CSL_Qm_queue_managementRegs*);
		TranslateAddress(qmss_globalCfg.groupRegs[i].qmQueStatReg,      qmss_cfg_regs-CSL_QMSS_CFG_BASE,    CSL_Qm_queue_status_configRegs*);
		TranslateAddress(qmss_globalCfg.groupRegs[i].qmStatusRAM,       qmss_cfg_regs-CSL_QMSS_CFG_BASE,    CSL_Qm_Queue_Status*);
		TranslateAddress(qmss_globalCfg.groupRegs[i].qmQueMgmtDataReg,  qmss_cfg_regs-CSL_QMSS_DATA_BASE,   CSL_Qm_queue_managementRegs*);

		/* not supported on k2 hardware, and not used by lld */
		qmss_globalCfg.groupRegs[i].qmQueMgmtProxyDataReg = 0;
	}

	for(i=0;i < QMSS_MAX_INTD;i++)
		TranslateAddress(qmss_globalCfg.regs.qmQueIntdReg[i], qmss_cfg_regs-CSL_QMSS_CFG_BASE, CSL_Qm_intdRegs*);


	for(i=0;i < QMSS_MAX_PDSP;i++){
		TranslateAddress(qmss_globalCfg.regs.qmPdspCmdReg[i],  qmss_cfg_regs-CSL_QMSS_CFG_BASE, volatile uint32_t*);
		TranslateAddress(qmss_globalCfg.regs.qmPdspCtrlReg[i], qmss_cfg_regs-CSL_QMSS_CFG_BASE, CSL_PdspRegs*);
		TranslateAddress(qmss_globalCfg.regs.qmPdspIRamReg[i], qmss_cfg_regs-CSL_QMSS_CFG_BASE, volatile uint32_t*);
	}

	TranslateAddress(qmss_globalCfg.regs.qmLinkingRAMReg, 	qmss_cfg_regs-CSL_QMSS_CFG_BASE, volatile uint32_t*);
	TranslateAddress(qmss_globalCfg.regs.qmBaseAddr, 		qmss_cfg_regs-CSL_QMSS_CFG_BASE, void*);

	if ((result = Qmss_init (&qmss_initCfg, &qmss_globalCfg)) != QMSS_SOK){
		printf ("initQmss: Error initializing Queue Manager SubSystem, Error code : %d\n", result);
		abort();
	}

	if ((result = Qmss_start ()) != QMSS_SOK){
		printf ("initQmss: Error starting Queue Manager SubSystem, Error code : %d\n", result);
		abort();
	}

	Cppi_GlobalCPDMAConfigParams translatedCppiGblCpdmaCfgParams[Cppi_CpDma_LAST+1];
	Cppi_GlobalConfigParams translatedCppiGblCfgParams = cppiGblCfgParams;

	translatedCppiGblCfgParams.cpDmaCfgs = translatedCppiGblCpdmaCfgParams;

#define translateCpdma(reg, type) (translatedCppiGblCfgParams.reg = (type)(((int) cppiGblCfgParams.reg ) +  cppi_regs - CPPI_BASE_REG))

	Cppi_CpDma cpdma;
	for(cpdma = Cppi_CpDma_SRIO_CPDMA; cpdma <= Cppi_CpDma_LAST; cpdma++){
		translatedCppiGblCfgParams.cpDmaCfgs[cpdma] = cppiGblCfgParams.cpDmaCfgs[cpdma];
		translateCpdma(cpDmaCfgs[cpdma].gblCfgRegs,	CSL_Cppidma_global_configRegs*);
		translateCpdma(cpDmaCfgs[cpdma].txChRegs, 	CSL_Cppidma_tx_channel_configRegs*);
		translateCpdma(cpDmaCfgs[cpdma].rxChRegs, 	CSL_Cppidma_rx_channel_configRegs*);
		translateCpdma(cpDmaCfgs[cpdma].txSchedRegs,CSL_Cppidma_tx_scheduler_configRegs*);
		translateCpdma(cpDmaCfgs[cpdma].rxFlowRegs, CSL_Cppidma_rx_flow_configRegs*);
	}

    if ((result = Cppi_init (&translatedCppiGblCfgParams)) != CPPI_SOK){
        printf ("Error initializing CPPI LLD, Error code : %d\n", result);
        abort();
    }

	/* Setup memory regions */
	/* Setup DATA region */
	result = setup_region(
			data_desc_base,
			DATA_DESC_SIZE, DATA_DESC_NUM,
			0,
			DATA_REG_NUM);
	if (result) abort();

	/* Setup CTRL region */
	result = setup_region(
			ctrl_desc_base,
			CTRL_DESC_SIZE, CTRL_DESC_NUM,
			DATA_DESC_NUM,
			CTRL_REG_NUM);
	if (result) abort();

	/* Setup TRACE region */
	result = setup_region(
			trace_desc_base,
			TRACE_DESC_SIZE, TRACE_DESC_NUM,
			DATA_DESC_NUM+CTRL_DESC_NUM,
			TRACE_REG_NUM);
	if (result) abort();

	/* Setup FFTC region */
	result = setup_region(
			fftc_desc_base,
			FFTC_DESC_SIZE, FFTC_DESC_NUM,
			DATA_DESC_NUM+CTRL_DESC_NUM+TRACE_DESC_NUM,
			FFTC_REG_NUM);
	if (result) abort();

    /* Setup the driver for this FFTC peripheral instance number. */
	/* Set up the FFTC CPDMA configuration */
	memset (&cpdmaCfg, 0, sizeof (Cppi_CpDmaInitCfg));
	cpdmaCfg.dmaNum = Cppi_CpDma_FFTC_A_CPDMA;

	/* Initialize FFTC CPDMA */
	if ((hCppi[0] = Cppi_open (&cpdmaCfg)) == NULL){
		printf ("Error initializing CPPI for FFTC CPDMA %d\n", cpdmaCfg.dmaNum);
		abort();
	}

	/* Disable FFTC CDMA loopback */
	if (Cppi_setCpdmaLoopback (hCppi[0], 0) != CPPI_SOK){
		printf ("Error disabling loopback for FFTC CPDMA %d\n", cpdmaCfg.dmaNum);
		abort();
	}

	memset (&cpdmaCfg, 0, sizeof (Cppi_CpDmaInitCfg));
	cpdmaCfg.dmaNum = Cppi_CpDma_FFTC_B_CPDMA;
	if ((hCppi[1] = Cppi_open (&cpdmaCfg)) == NULL){
		printf ("Error initializing CPPI for FFTC CPDMA %d\n", cpdmaCfg.dmaNum);
		abort();
	}

	/* Disable FFTC CDMA loopback */
	if (Cppi_setCpdmaLoopback (hCppi[1], 0) != CPPI_SOK){
		printf ("Error disabling loopback for FFTC CPDMA %d\n", cpdmaCfg.dmaNum);
		abort();
	}

	fftc_a_cfg_regs->CONFIG = 0;
	fftc_b_cfg_regs->CONFIG = 0;
//						CSL_FMK (FFTC_CONFIG_Q3_FLOWID_OVERWRITE, 0) |
//						CSL_FMK (FFTC_CONFIG_Q2_FLOWID_OVERWRITE, 0) |
//						CSL_FMK (FFTC_CONFIG_Q1_FLOWID_OVERWRITE, 0) |
//						CSL_FMK (FFTC_CONFIG_Q0_FLOWID_OVERWRITE, 0) |
//						CSL_FMK (FFTC_CONFIG_STARVATION_PERIOD, 0) |
//						CSL_FMK (FFTC_CONFIG_QUEUE_3_PRIORITY, 0) |
//						CSL_FMK (FFTC_CONFIG_QUEUE_2_PRIORITY, 0) |
//						CSL_FMK (FFTC_CONFIG_QUEUE_1_PRIORITY, 0) |
//						CSL_FMK (FFTC_CONFIG_QUEUE_0_PRIORITY, 0) |
//						CSL_FMK (FFTC_CONFIG_FFT_DISABLE, 0);

	/* Emptying Queues */
	/* Tx FFTC */
    Qmss_queueEmpty(QMSS_FFTC_A_QUEUE_BASE);
    for(i=QUEUE_FIRST; i<=QUEUE_LAST; i++){
        Qmss_queueEmpty(i);
    }

    /* Populate free queues */
    for(i=0; i<DATA_DESC_NUM; i++){
    	Cppi_Desc* mono_pkt = (Cppi_Desc *) ((int)data_desc_base + i*DATA_DESC_SIZE);

    	Osal_DescBeginMemAccess(mono_pkt, DATA_DESC_SIZE);

    	Qmss_Queue freeQueue = {0, QUEUE_FREE_DATA};
    	Cppi_setDescType(							  mono_pkt, Cppi_DescType_MONOLITHIC);
    	Cppi_setDataOffset(	Cppi_DescType_MONOLITHIC,  mono_pkt, PACKET_HEADER);
    	Cppi_setPacketLen(	Cppi_DescType_MONOLITHIC, mono_pkt, DATA_DESC_SIZE);
    	Cppi_setReturnQueue(Cppi_DescType_MONOLITHIC, mono_pkt, freeQueue);

    	/* Sync Descriptor */
    	Osal_DescEndMemAccess(mono_pkt, DATA_DESC_SIZE);

		Qmss_queuePushDescSize(QUEUE_FREE_DATA, mono_pkt, DATA_DESC_SIZE);
    }

    for(i=0; i<CTRL_DESC_NUM; i++){
    	Cppi_Desc* mono_pkt = (Cppi_Desc *) ((int)ctrl_desc_base + i*CTRL_DESC_SIZE);

    	Osal_DescBeginMemAccess(mono_pkt, CTRL_DESC_SIZE);

    	Qmss_Queue freeQueue = {0, QUEUE_FREE_DATA};
    	Cppi_setDescType(							  mono_pkt, Cppi_DescType_MONOLITHIC);
    	Cppi_setDataOffset(	Cppi_DescType_MONOLITHIC,  mono_pkt, PACKET_HEADER);
    	Cppi_setPacketLen(	Cppi_DescType_MONOLITHIC, mono_pkt, CTRL_DESC_SIZE);
    	Cppi_setReturnQueue(Cppi_DescType_MONOLITHIC, mono_pkt, freeQueue);

    	/* Sync Descriptor */
    	Osal_DescEndMemAccess(mono_pkt, CTRL_DESC_SIZE);

		Qmss_queuePushDescSize(QUEUE_FREE_CTRL, mono_pkt, CTRL_DESC_SIZE);
    }

    for(i=0; i<TRACE_DESC_NUM; i++){
    	Cppi_Desc* mono_pkt = (Cppi_Desc *) ((int)trace_desc_base + i*TRACE_DESC_SIZE);

    	Osal_DescBeginMemAccess(mono_pkt, TRACE_DESC_SIZE);

    	Qmss_Queue freeQueue = {0, QUEUE_FREE_DATA};
    	Cppi_setDescType(							  mono_pkt, Cppi_DescType_MONOLITHIC);
    	Cppi_setDataOffset(	Cppi_DescType_MONOLITHIC, mono_pkt, PACKET_HEADER);
    	Cppi_setPacketLen(	Cppi_DescType_MONOLITHIC, mono_pkt, TRACE_DESC_SIZE);
    	Cppi_setReturnQueue(Cppi_DescType_MONOLITHIC, mono_pkt, freeQueue);

    	/* Sync Descriptor */
    	Osal_DescEndMemAccess(mono_pkt, TRACE_DESC_SIZE);

		Qmss_queuePushDescSize(QUEUE_FREE_TRACE, mono_pkt, TRACE_DESC_SIZE);
    }

    for(i=0; i<FFTC_DESC_NUM; i++){
    	Cppi_Desc * host_pkt = (Cppi_Desc *) ((int)fftc_desc_base + i*FFTC_DESC_SIZE);

    	Osal_DescBeginMemAccess(host_pkt, FFTC_DESC_SIZE);
    	memset(host_pkt, 0, FFTC_DESC_SIZE);

    	Qmss_Queue queue = {0, QUEUE_FREE_FFTC};
    	Cppi_setDescType(								host_pkt, Cppi_DescType_HOST);
    	Cppi_setReturnPolicy(		Cppi_DescType_HOST, host_pkt, Cppi_ReturnPolicy_RETURN_BUFFER);
    	Cppi_setReturnPushPolicy(	Cppi_DescType_HOST, host_pkt, Qmss_Location_TAIL);
    	Cppi_setPSLocation(			Cppi_DescType_HOST, host_pkt, Cppi_PSLoc_PS_IN_DESC);
    	Cppi_setReturnQueue(		Cppi_DescType_HOST, host_pkt, queue);
    	((Cppi_HostDesc*)host_pkt)->nextBDPtr = 0;

    	/* Sync Descriptor */
    	Osal_DescEndMemAccess(host_pkt, FFTC_DESC_SIZE);

		Qmss_queuePushDescSize(QUEUE_FREE_FFTC,host_pkt,FFTC_DESC_SIZE);
    }

	configureRxFlow(0);
	configureRxFlow(1);

	configureTxChan(0);
	configureTxChan(1);

	configureRxChan(0);
	configureRxChan(1);

	/* Finally, enable the Tx channel so that we can start sending
	 * data blocks to FFTC engine.
	 */
	Cppi_channelEnable (hCppiTxChan[0]);
	Cppi_channelEnable (hCppiTxChan[1]);
	Cppi_channelEnable (hCppiRxChan[0]);
	Cppi_channelEnable (hCppiRxChan[1]);

	configureFFTRegs(fftc_a_cfg_regs);
	configureFFTRegs(fftc_b_cfg_regs);
}

void clean_qmss(){
	int i;
	Qmss_Result result;

	/* Emptying all queues */
    Qmss_queueEmpty(QUEUE_TX_FFTC_A);
    Qmss_queueEmpty(QUEUE_TX_FFTC_B);
    for(i=QUEUE_FIRST; i<=QUEUE_LAST; i++){
        Qmss_queueEmpty(i);
    }

    /* Close Flow */
	Cppi_closeRxFlow (hCppiRxFlow[0]);
	Cppi_closeRxFlow (hCppiRxFlow[1]);

	/* Disable the Tx channel */
	Cppi_channelDisable (hCppiTxChan[0]);
	Cppi_channelDisable (hCppiTxChan[1]);

	/* Disable the Rx channel */
	Cppi_channelDisable (hCppiRxChan[0]);
	Cppi_channelDisable (hCppiRxChan[1]);

	/* Close the CPPI/QMSS channels and queues associated with
	 * the transmit queue specified.
	 */
	Cppi_channelClose (hCppiTxChan[0]);
	Cppi_channelClose (hCppiRxChan[0]);
	Cppi_channelClose (hCppiTxChan[1]);
	Cppi_channelClose (hCppiRxChan[1]);

	/* Close Cppi */
	Cppi_close (hCppi[0]);
	Cppi_close (hCppi[1]);

	/* Reset FFTC */
	CSL_FINS (fftc_a_cfg_regs->CONTROL, FFTC_CONTROL_RESTART_BIT, 1);
	CSL_FINS (fftc_b_cfg_regs->CONTROL, FFTC_CONTROL_RESTART_BIT, 1);

	/* Exit Cppi */
	Cppi_exit ();

	/* Remove my regions which were only intstalled from core 0 */
	if ((result = Qmss_removeMemoryRegion (DATA_REG_NUM, 0)) != 0){
		printf ("Error : remove Data region\n");
		abort();
	}
	if ((result = Qmss_removeMemoryRegion (CTRL_REG_NUM, 0)) != 0){
		printf ("Error : remove Ctrl region\n");
		abort();
	}
	if ((result = Qmss_removeMemoryRegion (TRACE_REG_NUM, 0)) != 0){
		printf ("Error : remove Trace region\n");
		abort();
	}
	if ((result = Qmss_removeMemoryRegion (FFTC_REG_NUM, 0)) != 0){
		printf ("Error : remove FFTC region\n");
		abort();
	}

	/* Exit Qmss */
	if ((result = Qmss_exit ()) != 0){
		printf ("Error : Qmss exit error %d\n", result);
		abort();
	}
}

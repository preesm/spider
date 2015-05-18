/**
 *   @file  k2l/src/qmss_device.c
 *
 *   @brief   
 *      This file contains the device specific configuration and initialization routines
 *      for QMSS Low Level Driver.
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2011-2014, Texas Instruments, Inc.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  \par
*/

#ifdef DEVICE_K2L
/* QMSS Types includes */
#include <stdint.h>
#include <stdlib.h>

/* QMSS includes */
#include <ti/drv/qmss/qmss_qm.h>
#include <ti/drv/qmss/qmss_osal.h>

/* CSL RL includes */
#include <ti/csl/cslr_device.h>
#include <ti/csl/cslr_qm_config.h>
#include <ti/csl/cslr_qm_descriptor_region_config.h>
#include <ti/csl/cslr_qm_queue_management.h>
#include <ti/csl/cslr_qm_queue_status_config.h>
#include <ti/csl/cslr_qm_intd.h>
#include <ti/csl/cslr_pdsp.h>
#include <ti/csl/csl_qm_queue.h>

/** @addtogroup QMSS_LLD_DATASTRUCT
@{ 
*/

Qmss_QueueNumRange GlobalQmGroup0[] =
{
    /** Base queue number and Maximum supported low priority queues */
    {
        Qmss_QueueType_LOW_PRIORITY_QUEUE,
        QMSS_LOW_PRIORITY_QUEUE_BASE, 
        QMSS_MAX_LOW_PRIORITY_QUEUE,
        "LOW_PRIORITY_QUEUE-qm1"       
    },
        
    /** Base queue number and Maximum supported PASS 0 queues */
    {
        Qmss_QueueType_PASS_QUEUE,
        QMSS_PASS_QUEUE_BASE,
        QMSS_MAX_PASS_QUEUE,
        "PASS_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported Intc Pend queues */
    {
        Qmss_QueueType_INTC_QUEUE,
        QMSS_CIC0_QUEUE_BASE,
        QMSS_MAX_CIC0_QUEUE,
        "INTC_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported Intc Pend queues in set 2 */
    {
        Qmss_QueueType_INTC_SET2_QUEUE,
        QMSS_CIC_SET_0_QUEUE_BASE,
        QMSS_MAX_CIC_SET_0_QUEUE,
        "INTC_SET2_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported Intc Pend queues in set 3 */
    {
        Qmss_QueueType_INTC_SET3_QUEUE,
        QMSS_CIC_SET_1_QUEUE_BASE,
        QMSS_MAX_CIC_SET_1_QUEUE,
        "INTC_SET3_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported Intc Pend queues in set 4 */
    {
        Qmss_QueueType_INTC_SET4_QUEUE,
        QMSS_CIC_SET_2_QUEUE_BASE,
        QMSS_MAX_CIC_SET_2_QUEUE,
        "INTC_SET4_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported Intc Pend queues in set 5 */
    {
        Qmss_QueueType_INTC_SET5_QUEUE,
        QMSS_CIC_SET_3_QUEUE_BASE,
        QMSS_MAX_CIC_SET_3_QUEUE,
        "INTC_SET5_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported Intc & EDMA Pend queues in set 0 */
    {
        Qmss_QueueType_INTC_EDMA_SET0_QUEUE,
        QMSS_CIC_EDMA_SET_0_QUEUE_BASE,
        QMSS_MAX_CIC_EDMA_SET_0_QUEUE,
        "INTC_EDMA_SET0_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported Intc & EDMA Pend queues in set 1 */
    {
        Qmss_QueueType_INTC_EDMA_SET1_QUEUE,
        QMSS_CIC_EDMA_SET_1_QUEUE_BASE,
        QMSS_MAX_CIC_EDMA_SET_1_QUEUE,
        "INTC_EDMA_SET1_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported Intc & EDMA Pend queues in set 2 */
    {
        Qmss_QueueType_INTC_EDMA_SET2_QUEUE,
        QMSS_CIC_EDMA_SET_2_QUEUE_BASE,
        QMSS_MAX_CIC_EDMA_SET_2_QUEUE,
        "INTC_EDMA_SET2_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported Intc & EDMA Pend & GIC queues in set 0 */
    {
        Qmss_QueueType_SOC_SET0_QUEUE,
        QMSS_SOC_SET_0_QUEUE_BASE,
        QMSS_MAX_SOC_SET_0_QUEUE,
        "SOC_SET0_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported Intc & EDMA Pend & GIC qeuees in set 1 */
    {
        Qmss_QueueType_SOC_SET1_QUEUE,
        QMSS_SOC_SET_1_QUEUE_BASE,
        QMSS_MAX_SOC_SET_1_QUEUE,
        "SOC_SET1_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported FFTC A queues */
    {
        Qmss_QueueType_FFTC_A_QUEUE,
        QMSS_FFTC_A_QUEUE_BASE,
        QMSS_MAX_FFTC_A_QUEUE,
        "FFTC_A_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported FFTC B queues */
    {
        Qmss_QueueType_FFTC_B_QUEUE, 
        QMSS_FFTC_B_QUEUE_BASE,
        QMSS_MAX_FFTC_B_QUEUE,
        "FFTC_B_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported BCP queues */
    {
        Qmss_QueueType_BCP_QUEUE,
        QMSS_BCP_QUEUE_BASE,
        QMSS_MAX_BCP_QUEUE,
        "BCP_QUEUE-qm1" 
    },
           
    /** Base queue number and Maximum supported high priority queues */
    {
        Qmss_QueueType_HIGH_PRIORITY_QUEUE,
        QMSS_HIGH_PRIORITY_QUEUE_BASE,
        QMSS_MAX_HIGH_PRIORITY_QUEUE,
        "HIGH_PRIORITY_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported starvation counter queues */
    {
        Qmss_QueueType_STARVATION_COUNTER_QUEUE,
        QMSS_STARVATION_COUNTER_QUEUE_BASE,
        QMSS_MAX_STARVATION_COUNTER_QUEUE,
        "STARVATION_COUNTER_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported infrastructure queues on first QM */
    {
        Qmss_QueueType_INFRASTRUCTURE_QUEUE,
        QMSS_INFRASTRUCTURE_QUEUE_BASE,
        QMSS_MAX_INFRASTRUCTURE_QUEUE,
        "INFRASTRUCTURE_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported GIC400 queues */
    {
        Qmss_QueueType_GIC400_QUEUE,
        QMSS_ARM_GIC_QUEUE_BASE,
        QMSS_MAX_ARM_GIC_QUEUE,
        "GIC400_QUEUE-qm1" 
    },
     
    /** Base queue number and Maximum supported IQNET queues */
    {
        Qmss_QueueType_IQNET_QUEUE,
        QMSS_IQNET_QUEUE_BASE,
        QMSS_MAX_IQNET_QUEUE,
        "IQNET_QUEUE-qm1"
    },
 
    /** Base queue number and Maximum supported EDMA0 queues */
    {
        Qmss_QueueType_EDMA_0_QUEUE,
        QMSS_EDMA0_QUEUE_BASE,
        QMSS_MAX_EDMA0_QUEUE,
        "EDMA_0_QUEUE-qm1"
    },
 
    /** Base queue number and Maximum supported general purpose queues */
    {
        Qmss_QueueType_GENERAL_PURPOSE_QUEUE,
        QMSS_GENERAL_PURPOSE_QUEUE_BASE, 
        QMSS_MAX_GENERAL_PURPOSE_QUEUE,
        "GENERAL_PURPOSE_QUEUE-qm1" 
    }
}; /* GlobalQmGroup0 */


Qmss_QueueNumRange NetssQmGroup0[] =
{
    /** Base queue number and Maximum supported PASS 0 queues */
    {
        Qmss_QueueType_PASS_QUEUE,
        QMSS_NETSS_PASS_QUEUE_BASE,
        QMSS_MAX_NETSS_PASS_QUEUE,
        "NETSS_PASS_QUEUE-qm1" 
    },
    {
        Qmss_QueueType_GENERAL_PURPOSE_QUEUE,
        QMSS_NETSS_GENERAL_PURPOSE_QUEUE_BASE, 
        QMSS_MAX_NETSS_GENERAL_PURPOSE_QUEUE,
        "NETSS_GENERAL_PURPOSE_QUEUE-qm1" 
    }
}; /* NetssQmGroup0 */

Qmss_QueueNumRange NetssQmGroup1[] =
{
    {
        Qmss_QueueType_GENERAL_PURPOSE_QUEUE,
        QMSS_NETSS_GENERAL_PURPOSE_QUEUE_BASE2, 
        QMSS_MAX_NETSS_GENERAL_PURPOSE_QUEUE2,
        "NETSS_GENERAL_PURPOSE_QUEUE-qm2" 
    }
}; /* NetssQmGroup0 */


/** @brief QMSS LLD initialization parameters */
Qmss_GlobalConfigParams qmssGblCfgParams =
{
    /** Maximum number of queue manager groups */
    1u,
    /** Maximum number of queue Managers */
    2u,
    /** Maximum number of queues */
    8192u,
    /** Maximum number of descriptor regions */
    64,
    /** Maximum number of PDSP */
    8,
    /** Size of internal linkram */
    16384u,
    /** Does not require ordered memory regions */
    0,
    {
        /** queue types for group 0 */
        sizeof(GlobalQmGroup0)/sizeof(Qmss_QueueNumRange),
        /** queue types for group 1 */
        0
    },
    {   /* numQueueNum[] */
        /** Queue groups for group 1 of global QM */
        GlobalQmGroup0,
        /** No second group on global QM on this device */
        NULL
    },
    { 
        { /* Registers for first QM */
            /** QM Global Config registers */
            (CSL_Qm_configRegs *) CSL_QMSS_CFG_QM_1_CFG_REGS,
            /** QM Descriptor Config registers */
            (CSL_Qm_descriptor_region_configRegs *) CSL_QMSS_CFG_QM_1_DESCRIPTOR_REGS, 
            /** QM queue Management registers */
            (CSL_Qm_queue_managementRegs *) CSL_QMSS_CFG_QM_1_QUEUE_MANAGEMENT_REGS,
            /** QM queue Management Proxy registers */
            (CSL_Qm_queue_managementRegs *) CSL_QMSS_CFG_QM_1_QUEUE_PROXY_REGS,
            /** QM queue status registers */
            (CSL_Qm_queue_status_configRegs *) CSL_QMSS_CFG_QM_1_QUEUE_PEEK_REGS,
            /** QM Status RAM */
            (CSL_Qm_Queue_Status *) CSL_QMSS_CFG_QM_1_STATUS_RAM,
            /** QM queue Management registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) CSL_QMSS_DATA_QM1_QUEUE_MANAGEMENT_REGS,
            /** QM queue Management Proxy registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) NULL
        },
        { /* Registers for second QM */
            /** QM Global Config registers */
            (CSL_Qm_configRegs *) NULL,
            /** QM Descriptor Config registers */
            (CSL_Qm_descriptor_region_configRegs *) NULL, 
            /** QM queue Management registers */
            (CSL_Qm_queue_managementRegs *) NULL,
            /** QM queue Management Proxy registers */
            (CSL_Qm_queue_managementRegs *) NULL,
            /** QM queue status registers */
            (CSL_Qm_queue_status_configRegs *) NULL,
            /** QM Status RAM */
            (CSL_Qm_Queue_Status *) NULL,
            /** QM queue Management registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) NULL,
            /** QM queue Management Proxy registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) NULL
        }
    },
    { /* QMSS global registers */
        /** QM INTD N registers */
        {
            /** QM INTD 1 registers */
            (CSL_Qm_intdRegs *) CSL_QMSS_CFG_INTD_1_REGS,
            /** QM INTD 2 registers */
            (CSL_Qm_intdRegs *) CSL_QMSS_CFG_INTD_2_REGS
        },
        /** QM PDSP N command register */
        {
            /** QM PDSP 1 command register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP1_SRAM,
            /** QM PDSP 2 command register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP2_SRAM,
            /** QM PDSP 3 command register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP3_SRAM,
            /** QM PDSP 4 command register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP4_SRAM,
            /** QM PDSP 5 command register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP5_SRAM,
            /** QM PDSP 6 command register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP6_SRAM,
            /** QM PDSP 7 command register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP7_SRAM,
            /** QM PDSP 8 command register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP8_SRAM
        },
        /** QM PDSP N control register */
        {
            /** QM PDSP 1 control register */
            (CSL_PdspRegs *) CSL_QMSS_CFG_PDSP1_REGS,
            /** QM PDSP 2 control register */
            (CSL_PdspRegs *) CSL_QMSS_CFG_PDSP2_REGS,
            /** QM PDSP 3 control register */
            (CSL_PdspRegs *) CSL_QMSS_CFG_PDSP3_REGS,
            /** QM PDSP 4 control register */
            (CSL_PdspRegs *) CSL_QMSS_CFG_PDSP4_REGS,
            /** QM PDSP 5 control register */
            (CSL_PdspRegs *) CSL_QMSS_CFG_PDSP5_REGS,
            /** QM PDSP 6 control register */
            (CSL_PdspRegs *) CSL_QMSS_CFG_PDSP6_REGS,
            /** QM PDSP 7 control register */
            (CSL_PdspRegs *) CSL_QMSS_CFG_PDSP7_REGS,
            /** QM PDSP 8 control register */
            (CSL_PdspRegs *) CSL_QMSS_CFG_PDSP8_REGS
        },
        /** QM PDSP N IRAM register */
        {
            /** QM PDSP 1 IRAM register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP1_IRAM,
            /** QM PDSP 2 IRAM register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP2_IRAM,
            /** QM PDSP 3 IRAM register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP3_IRAM,
            /** QM PDSP 4 IRAM register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP4_IRAM,
            /** QM PDSP 5 IRAM register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP5_IRAM,
            /** QM PDSP 6 IRAM register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP6_IRAM,
            /** QM PDSP 7 IRAM register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP7_IRAM,
            /** QM PDSP 8 IRAM register */
            (volatile uint32_t *) CSL_QMSS_CFG_PDSP8_IRAM
        },
        /** QM Linking RAM register */
        (volatile uint32_t *) CSL_QMSS_LINKING_RAM,
        /** QM peripheral base address used to calculate internal addresses */
        (void *) CSL_QMSS_CFG_BASE
    },
#ifdef _VIRTUAL_ADDR_SUPPORT
    /** virt2Phy */
    Osal_qmssVirtToPhy,
    /** phy2Virt */
    Osal_qmssPhyToVirt,
    /** virt2PhyDesc */
    Osal_qmssConvertDescVirtToPhy,
    /** phy2VirtDesc */
    Osal_qmssConvertDescPhyToVirt,
#else
    /** virt2Phy */
    NULL,
    /** phy2Virt */
    NULL,
    /** virt2PhyDesc */
    NULL,
    /** phy2VirtDesc */
    NULL,
#endif
    /** Resource Manager Handle should be NULL by default */
    (Qmss_RmServiceHnd)NULL,
    { 
        { /* Resource names for first QM */
            /** RM DTS resource name for first QM overall QM control */
            "control-qm1",
            /** RM DTS resource name for first QM allowing Link RAM configurability */
            "linkram-control-qm1",
            /** RM DTS resource name for first QM Link RAM indices (internal/region 0) */
            "linkram-int-qm1",
            /** RM DTS resource name for first QM Link RAM indices (external/region 1) */
            "linkram-ext-qm1",
            /** RM DTS resource name for first QM Memory regions */
            "memory-regions-qm1",
        },
        { /* Resource names for second QM */
            /** RM DTS resource name for second QM overall QM control */
            "control-qm2",         
            /** RM DTS resource name for second QM allowing Link RAM configurability */
            "linkram-control-qm2",
            /** RM DTS resource name for second QM Link RAM indices (internal/region 0) */
            "linkram-int-qm2",
            /** RM DTS resource name for second QM Link RAM indices (external/region 1) */
            "linkram-ext-qm2",
            /** RM DTS resource name for second QM Memory regions */
            "memory-regions-qm2",
        }
    },    
    /** RM DTS resource name for PDSP Firmware download */
    "firmware-pdsp",
    {
        /** RM DST resource name for accumulator channels on accumulator 0 (INTD0) */
        "accumulator0-ch",
        /** RM DST resource name for accumulator channels on accumulator 1 (INTD1) */
        "accumulator1-ch"
    },
    /** RM nameserver pattern for per region queues */
    "qm-reg-q-ss%d-gr%d-r%d",
    /** Map of PDSP # to its associated intd (which defines which set of logical channels/interrupts are used */
    {
        0, 0, 1, 1, 0, 0, 1, 1
    }
};

/** @brief QMSS LLD initialization parameters */
Qmss_GlobalConfigParams qmssNetssGblCfgParams =
{
    /** Maximum number of queue manager groups */
    2u,
    /** Maximum number of queue Managers */
    2u,
    /** Maximum number of queues */
    128u,
    /** Maximum number of descriptor regions */
    16,
    /** Maximum number of PDSP */
    0,
    /** Size of internal linkram */
    16384u,
    /** Does not require ordered memory regions (0) */
    0,
    {
        /** queue types for group 0 */
        sizeof(NetssQmGroup0)/sizeof(Qmss_QueueNumRange),
        /** queue types for group 1 */
        sizeof(NetssQmGroup1)/sizeof(Qmss_QueueNumRange),
    },
    {   /* numQueueNum[] */
        /** Queue groups for group 1 of global QM */
        NetssQmGroup0,
        /** No second group on global QM on this device */
        NetssQmGroup1
    },
    /* Tables defining queue numbers for each queue manager group */
    {  /* maxQueueNum[] */
        { /* Registers for first QM */
            /** QM Global Config registers */
            (CSL_Qm_configRegs *) (CSL_NETCP_CFG_REGS + 0x001B0000),
            /** QM Descriptor Config registers */
            (CSL_Qm_descriptor_region_configRegs *) (CSL_NETCP_CFG_REGS + 0x001B1000), 
            /** QM queue Management registers */
            (CSL_Qm_queue_managementRegs *) (CSL_NETCP_CFG_REGS + 0x001B8000),
            /** QM queue Management Proxy registers */
            (CSL_Qm_queue_managementRegs *) NULL,
            /** QM queue status registers */
            (CSL_Qm_queue_status_configRegs *) (CSL_NETCP_CFG_REGS + 0x001BC000),
            /** QM Status RAM */
            (CSL_Qm_Queue_Status *) NULL,
            /** QM queue Management registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) (CSL_NETCP_CFG_REGS + 0x001B8000),
            /** QM queue Management Proxy registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) NULL
        },
        { /* Registers for second QM */
            /** QM Global Config registers */
            (CSL_Qm_configRegs *) (CSL_NETCP_CFG_REGS + 0x001B0800),
            /** QM Descriptor Config registers */
            (CSL_Qm_descriptor_region_configRegs *) (CSL_NETCP_CFG_REGS + 0x001B1800), 
            /** QM queue Management registers */
            (CSL_Qm_queue_managementRegs *) (CSL_NETCP_CFG_REGS + 0x001B8400),
            /** QM queue Management Proxy registers */
            (CSL_Qm_queue_managementRegs *) NULL,
            /** QM queue status registers */
            (CSL_Qm_queue_status_configRegs *) (CSL_NETCP_CFG_REGS + 0x001BC400),
            /** QM Status RAM */
            (CSL_Qm_Queue_Status *) NULL,
            /** QM queue Management registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) (CSL_NETCP_CFG_REGS + 0x001B8400),
            /** QM queue Management Proxy registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) NULL
        }
    },
    { /* QMSS global registers */
        /** QM INTD N registers */
        {
            /** QM INTD 1 registers */
            (CSL_Qm_intdRegs *) NULL,
            /** QM INTD 2 registers */
            (CSL_Qm_intdRegs *) NULL
        },
        /** QM PDSP N command register */
        {
            /** QM PDSP 1 command register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 2 command register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 3 command register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 4 command register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 5 command register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 6 command register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 7 command register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 8 command register */
            (volatile uint32_t *) NULL
        },
        /** QM PDSP N control register */
        {
            /** QM PDSP 1 control register */
            (CSL_PdspRegs *) NULL,
            /** QM PDSP 2 control register */
            (CSL_PdspRegs *) NULL,
            /** QM PDSP 3 control register */
            (CSL_PdspRegs *) NULL,
            /** QM PDSP 4 control register */
            (CSL_PdspRegs *) NULL,
            /** QM PDSP 5 control register */
            (CSL_PdspRegs *) NULL,
            /** QM PDSP 6 control register */
            (CSL_PdspRegs *) NULL,
            /** QM PDSP 7 control register */
            (CSL_PdspRegs *) NULL,
            /** QM PDSP 8 control register */
            (CSL_PdspRegs *) NULL
        },
        /** QM PDSP N IRAM register */
        {
            /** QM PDSP 1 IRAM register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 2 IRAM register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 3 IRAM register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 4 IRAM register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 5 IRAM register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 6 IRAM register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 7 IRAM register */
            (volatile uint32_t *) NULL,
            /** QM PDSP 8 IRAM register */
            (volatile uint32_t *) NULL
        },
        /** QM Linking RAM register */
        (volatile uint32_t *) (CSL_NETCP_CFG_REGS + 0x001a0000),
        /** QM peripheral base address used to calculate internal addresses */
        (void *) (CSL_NETCP_CFG_REGS + 0x001a0000)
    },
    /** virt2Phy */
    NULL,
    /** phy2Virt */
    NULL,
    /** virt2PhyDesc */
    NULL,
    /** phy2VirtDesc */
    NULL,
    /** Resource Manager Handle should be NULL by default */
    (Qmss_RmServiceHnd)NULL,
    { 
        { /* Resource names for first QM */
            /** RM DTS resource name for first QM overall QM control */
            "netss-control-qm1",
            /** RM DTS resource name for first QM allowing Link RAM configurability */
            "netss-linkram-control-qm1",
            /** RM DTS resource name for first QM Link RAM indices (internal/region 0) */
            "netss-linkram-qm1",
            /** RM DTS resource name for first QM Link RAM indices (external not supported) */
            "netss-linkram-qm1-ns",
            /** RM DTS resource name for first QM Memory regions */
            "netss-memory-regions-qm1",
        },
        { /* Resource names for second QM */
            /** RM DTS resource name for second QM overall QM control */
            "netss-control-qm2",         
            /** RM DTS resource name for second QM allowing Link RAM configurability */
            "netss-linkram-control-qm2",
            /** RM DTS resource name for second QM Link RAM indices (internal/region 0) */
            "netss-linkram-qm2",
            /** RM DTS resource name for second QM Link RAM indices (external not supported) */
            "netss-linkram-qm2-ns",
            /** RM DTS resource name for second QM Memory regions */
            "netss-memory-regions-qm2",
        }
    },    
    /** RM DTS resource name for PDSP Firmware download */
    "netss-firmware-pdsp",
    {
        /** RM DST resource name for accumulator channels on accumulator 0 (INTD0) */
        "netss-accumulator0-ch",
        /** RM DST resource name for accumulator channels on accumulator 1 (INTD1) */
        "netss-accumulator1-ch"
    },
    /** RM nameserver pattern for per region queues */
    "qm-reg-q-ss%d-gr%d-r%d",
    /** Map of PDSP # to its associated intd (which defines which set of logical channels/interrupts are used */
    {
        0, 0, 0, 0, 0, 0, 0, 0
    }
};
#endif

/**
@}
*/



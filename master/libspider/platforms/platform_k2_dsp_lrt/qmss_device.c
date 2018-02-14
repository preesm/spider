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
        
    /** Base queue number and Maximum supported AIF queues */
    {
        Qmss_QueueType_AIF_QUEUE,
        QMSS_AIF_QUEUE_BASE,
        QMSS_MAX_AIF_QUEUE,
        "AIF_QUEUE-qm1"
    },
        
    /** Base queue number and Maximum supported PASS 0 queues */
    {
        Qmss_QueueType_PASS_QUEUE,
        QMSS_PASS_QUEUE_BASE,
        QMSS_MAX_PASS_QUEUE,
        "PASS_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported Intc Pend queues in set 1 */
    {
        Qmss_QueueType_INTC_QUEUE,
        QMSS_INTC_QUEUE_BASE,
        QMSS_MAX_INTC_QUEUE,
        "INTC_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported Intc Pend queues in set 2 */
    {
        Qmss_QueueType_INTC_SET2_QUEUE,
        QMSS_INTC_SET2_QUEUE_BASE,
        QMSS_MAX_INTC_SET2_QUEUE,
        "INTC_SET2_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported Intc Pend queues in set 3 */
    {
        Qmss_QueueType_INTC_SET3_QUEUE,
        QMSS_INTC_SET3_QUEUE_BASE,
        QMSS_MAX_INTC_SET3_QUEUE,
        "INTC_SET3_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported SRIO queues */
    {
        Qmss_QueueType_SRIO_QUEUE, 
        QMSS_SRIO_QUEUE_BASE, 
        QMSS_MAX_SRIO_QUEUE,
        "SRIO_QUEUE-qm1" 
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
        
    /** Base queue number and Maximum supported FFTC C queues */
    {
        Qmss_QueueType_FFTC_C_QUEUE,
        QMSS_FFTC_C_QUEUE_BASE,
        QMSS_MAX_FFTC_C_QUEUE,
        "FFTC_C_QUEUE-qm1" 
    },

    /** Base queue number and Maximum supported FFTC D queues */
    {
        Qmss_QueueType_FFTC_D_QUEUE, 
        QMSS_FFTC_D_QUEUE_BASE,
        QMSS_MAX_FFTC_D_QUEUE,
        "FFTC_D_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported FFTC E queues */
    {
        Qmss_QueueType_FFTC_E_QUEUE, 
        QMSS_FFTC_E_QUEUE_BASE,
        QMSS_MAX_FFTC_E_QUEUE,
        "FFTC_E_QUEUE-qm1" 
    },
        
    /** Base queue number and Maximum supported FFTC F queues */
    {
        Qmss_QueueType_FFTC_F_QUEUE, 
        QMSS_FFTC_F_QUEUE_BASE,
        QMSS_MAX_FFTC_F_QUEUE,
        "FFTC_F_QUEUE-qm1" 
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
        
    /** Base queue number and Maximum supported traffic shaping queues */
    {
        Qmss_QueueType_TRAFFIC_SHAPING_QUEUE,
        QMSS_TRAFFIC_SHAPING_QUEUE_BASE,
        QMSS_MAX_TRAFFIC_SHAPING_QUEUE,
        "TRAFFIC_SHAPING_QUEUE-qm1" 
    },
       
    /** Base queue number and Maximum supported general purpose queues */
    {
        Qmss_QueueType_GENERAL_PURPOSE_QUEUE,
        QMSS_GENERAL_PURPOSE_QUEUE_BASE, 
        QMSS_MAX_GENERAL_PURPOSE_QUEUE,
        "GENERAL_PURPOSE_QUEUE-qm1" 
    }
}; /* GlobalQmGroup0 */

Qmss_QueueNumRange GlobalQmGroup1[] =
{
    /** Base queue number and Maximum supported low priority queues */
    {
        Qmss_QueueType_LOW_PRIORITY_QUEUE, 
        QMSS_QM2_LOW_PRIORITY_QUEUE_BASE, 
        QMSS_MAX_QM2_LOW_PRIORITY_QUEUE,
        "LOW_PRIORITY_QUEUE-qm2"  
    },

    /** Base queue number and Maximum supported Intc Pend queues in set 4 */
    {
        Qmss_QueueType_INTC_SET4_QUEUE,
        QMSS_INTC_SET4_QUEUE_BASE,
        QMSS_MAX_INTC_SET4_QUEUE,
        "INTC_SET4_QUEUE-qm2" 
    },
    
    /** Base queue number and Maximum supported high priority queues */
    {
        Qmss_QueueType_HIGH_PRIORITY_QUEUE,
        QMSS_QM2_HIGH_PRIORITY_QUEUE_BASE,
        QMSS_MAX_QM2_HIGH_PRIORITY_QUEUE,
        "HIGH_PRIORITY_QUEUE-qm2"
    },
        
    /** Base queue number and Maximum supported starvation counter queues */
    {
        Qmss_QueueType_STARVATION_COUNTER_QUEUE,
        QMSS_QM2_STARVATION_COUNTER_QUEUE_BASE,
        QMSS_MAX_QM2_STARVATION_COUNTER_QUEUE,
        "STARVATION_COUNTER_QUEUE-qm2"
    },

    /** Base queue number and Maximum supported infrastructure queues on second QM */
    {
        Qmss_QueueType_QM2_INFRASTRUCTURE_QUEUE,
        QMSS_QM2_INFRASTRUCTURE_DMA_QUEUE_BASE,
        QMSS_MAX_QM2_INFRASTRUCTURE_DMA_QUEUE,
        "QM2_INFRASTRUCTURE_QUEUE-qm2"
    },
        
    /** Base queue number and Maximum supported GIC400 queues */
    {
        Qmss_QueueType_GIC400_QUEUE,
        QMSS_GIC400_QUEUE_BASE,
        QMSS_MAX_GIC400_QUEUE,
        "GIC400_QUEUE-qm2"
    },
     
    /** Base queue number and Maximum supported EDMA 4 queues */
    {
        Qmss_QueueType_EDMA_4_QUEUE,
        QMSS_EDMA4_QUEUE_BASE,
        QMSS_MAX_EDMA4_QUEUE,
        "EDMA_4_QUEUE-qm2"
    },
     
    /** Base queue number and Maximum supported Hyperlink Broadcast queues */
    {
        Qmss_QueueType_HLINK_BROADCAST_QUEUE,
        QMSS_BCAST_HYPERLINK_0_1_QUEUE_BASE,
        QMSS_MAX_BCAST_HYPERLINK_0_1_QUEUE,
        "HLINK_BROADCAST_QUEUE-qm2"
    },

    /** Base queue number and Maximum supported Hyperlink 0 queues */
    {
        Qmss_QueueType_HLINK_0_QUEUE,
        QMSS_HYPERLINK_0_QUEUE_BASE,
        QMSS_MAX_HYPERLINK_0_QUEUE,
        "HLINK_0_QUEUE-qm2"
    },

    /** Base queue number and Maximum supported Hyperlink 1 queues */
    {
        Qmss_QueueType_HLINK_1_QUEUE,
        QMSS_HYPERLINK_1_QUEUE_BASE,
        QMSS_MAX_HYPERLINK_1_QUEUE,
        "HLINK_1_QUEUE-qm2"
    },

    /** Base queue number and Maximum supported XGE (10 gigabit ethernet) queues */
    {
        Qmss_QueueType_XGE_QUEUE,
        QMSS_XGE_QUEUE_BASE,
        QMSS_MAX_XGE_QUEUE,
        "XGE_QUEUE-qm2"
    },

    /** Base queue number and Maximum supported DXB queues */
    {
        Qmss_QueueType_DXB_QUEUE,
        QMSS_DXB_QUEUE_BASE,
        QMSS_MAX_DXB_QUEUE,
        "DXB_QUEUE-qm2"
    },

    /** Base queue number and Maximum supported general purpose queues */
    {
        Qmss_QueueType_GENERAL_PURPOSE_QUEUE,
        QMSS_QM2_GENERAL_PURPOSE_QUEUE_BASE, 
        QMSS_MAX_QM2_GENERAL_PURPOSE_QUEUE,
        "GENERAL_PURPOSE_QUEUE-qm2"
    }
}; /* GlobalQmGroup1 */

/** @brief QMSS LLD initialization parameters */
Qmss_GlobalConfigParams qmssGblCfgParams =
{
    /** Maximum number of queue manager groups */
    2u,
    /** Maximum number of queue Managers */
    4u,
    /** Maximum number of queues */
    16384u,
    /** Maximum number of descriptor regions */
    64,
    /** Maximum number of PDSP */
    8,
    /** Size of internal linkram */
    32768u,
    /** Does not require ordered memory regions */
    0,
    {
        /** queue types for group 0 */
        sizeof(GlobalQmGroup0)/sizeof(Qmss_QueueNumRange),
        /** queue types for group 1 */
        sizeof(GlobalQmGroup1)/sizeof(Qmss_QueueNumRange)
    },
    {   /* numQueueNum[] */
        /** Queue groups for group 1 of global QM */
        GlobalQmGroup0,
        /** No second group on global QM on this device */
        GlobalQmGroup1
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
            (CSL_Qm_configRegs *) CSL_QMSS_CFG_QM_2_CFG_REGS,
            /** QM Descriptor Config registers */
            (CSL_Qm_descriptor_region_configRegs *) CSL_QMSS_CFG_QM_2_DESCRIPTOR_REGS, 
            /** QM queue Management registers */
            (CSL_Qm_queue_managementRegs *) CSL_QMSS_CFG_QM_2_QUEUE_MANAGEMENT_REGS,
            /** QM queue Management Proxy registers */
            (CSL_Qm_queue_managementRegs *) CSL_QMSS_CFG_QM_2_QUEUE_PROXY_REGS,
            /** QM queue status registers */
            (CSL_Qm_queue_status_configRegs *) CSL_QMSS_CFG_QM_2_QUEUE_PEEK_REGS,
            /** QM Status RAM */
            (CSL_Qm_Queue_Status *) CSL_QMSS_CFG_QM_2_STATUS_RAM,
            /** QM queue Management registers, accessed via DMA port */
            (CSL_Qm_queue_managementRegs *) CSL_QMSS_DATA_QM2_QUEUE_MANAGEMENT_REGS,
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
    /** Map of PDSP # to its associated intd (which defines which set of logical channels/interrupts are used */
    {
        0, 0, 1, 1, 0, 0, 1, 1
    }
};

/**
@}
*/


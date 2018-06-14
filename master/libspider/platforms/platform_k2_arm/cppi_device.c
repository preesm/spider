/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2015 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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
/* CPPI Types includes */
#include <stdint.h>
#include <stdlib.h>

/* CPPI includes */
#include <ti/drv/cppi/cppi_drv.h>

/* CSL RL includes */
#include <ti/csl/cslr_device.h>
#include <ti/csl/cslr_cppidma_global_config.h>
#include <ti/csl/cslr_cppidma_rx_channel_config.h>
#include <ti/csl/cslr_cppidma_rx_flow_config.h>
#include <ti/csl/cslr_cppidma_tx_channel_config.h>
#include <ti/csl/cslr_cppidma_tx_scheduler_config.h>
#include <ti/csl/csl_cppi.h>

/** @addtogroup CPPI_LLD_DATASTRUCT
@{ 
*/
/** @brief CPPI LLD initialization parameters for each CPDMA */
Cppi_GlobalCPDMAConfigParams cppiGblCPDMACfgParams[CPPI_MAX_CPDMA] =
        {
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_SRIO_CPDMA,
                        /** Maximum supported Rx Channels */
                        16u,
                        /** Maximum supported Tx Channels */
                        16u,
                        /** Maximum supported Rx Flows */
                        20u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_SRIO_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_SRIO_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_SRIO_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_SRIO_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_SRIO_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "srio-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "srio-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "srio-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_AIF_CPDMA,
                        /** Maximum supported Rx Channels */
                        129u,
                        /** Maximum supported Tx Channels */
                        129u,
                        /** Maximum supported Rx Flows */
                        129u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_AIF_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_AIF_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_AIF_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_AIF_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_AIF_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "aif-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "aif-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "aif-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_FFTC_A_CPDMA,
                        /** Maximum supported Rx Channels */
                        4u,
                        /** Maximum supported Tx Channels */
                        4u,
                        /** Maximum supported Rx Flows */
                        8u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_FFTC_0_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_FFTC_0_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_FFTC_0_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_FFTC_0_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_FFTC_0_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "fftc-a-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "fftc-a-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "fftc-a-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_FFTC_B_CPDMA,
                        /** Maximum supported Rx Channels */
                        4u,
                        /** Maximum supported Tx Channels */
                        4u,
                        /** Maximum supported Rx Flows */
                        8u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_FFTC_1_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_FFTC_1_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_FFTC_1_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_FFTC_1_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_FFTC_1_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "fftc-b-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "fftc-b-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "fftc-b-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_FFTC_C_CPDMA,
                        /** Maximum supported Rx Channels */
                        4u,
                        /** Maximum supported Tx Channels */
                        4u,
                        /** Maximum supported Rx Flows */
                        8u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_FFTC_2_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_FFTC_2_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_FFTC_2_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_FFTC_2_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_FFTC_2_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "fftc-c-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "fftc-c-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "fftc-c-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_FFTC_D_CPDMA,
                        /** Maximum supported Rx Channels */
                        4u,
                        /** Maximum supported Tx Channels */
                        4u,
                        /** Maximum supported Rx Flows */
                        8u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_FFTC_3_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_FFTC_3_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_FFTC_3_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_FFTC_3_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_FFTC_3_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "fftc-d-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "fftc-d-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "fftc-d-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_FFTC_E_CPDMA,
                        /** Maximum supported Rx Channels */
                        4u,
                        /** Maximum supported Tx Channels */
                        4u,
                        /** Maximum supported Rx Flows */
                        8u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_FFTC_4_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_FFTC_4_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_FFTC_4_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_FFTC_4_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_FFTC_4_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "fftc-e-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "fftc-e-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "fftc-e-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_FFTC_F_CPDMA,
                        /** Maximum supported Rx Channels */
                        4u,
                        /** Maximum supported Tx Channels */
                        4u,
                        /** Maximum supported Rx Flows */
                        8u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_FFTC_5_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_FFTC_5_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_FFTC_5_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_FFTC_5_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_FFTC_5_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "fftc-f-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "fftc-f-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "fftc-f-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_PASS_CPDMA,
                        /** Maximum supported Rx Channels */
                        24u,
                        /** Maximum supported Tx Channels */
                        9u,
                        /** Maximum supported Rx Flows */
                        32u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_NETCP_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_NETCP_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_NETCP_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_NETCP_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_NETCP_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "pass-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "pass-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "pass-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_QMSS_CPDMA,
                        /** Maximum supported Rx Channels */
                        32u,
                        /** Maximum supported Tx Channels */
                        32u,
                        /** Maximum supported Rx Flows */
                        64u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_QMSS_CFG_PKTDMA_1_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_QMSS_CFG_PKTDMA_1_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_QMSS_CFG_PKTDMA_1_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_QMSS_CFG_PKTDMA_1_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_QMSS_CFG_PKTDMA_1_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "qmss-qm1-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "qmss-qm1-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "qmss-qm1-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_QMSS_QM2_CPDMA,
                        /** Maximum supported Rx Channels */
                        32u,
                        /** Maximum supported Tx Channels */
                        32u,
                        /** Maximum supported Rx Flows */
                        64u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_QMSS_CFG_PKTDMA_2_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_QMSS_CFG_PKTDMA_2_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_QMSS_CFG_PKTDMA_2_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_QMSS_CFG_PKTDMA_2_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_QMSS_CFG_PKTDMA_2_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "qmss-qm2-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "qmss-qm2-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "qmss-qm2-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_BCP_CPDMA,
                        /** Maximum supported Rx Channels */
                        (uint32_t) 8u,
                        /** Maximum supported Tx Channels */
                        (uint32_t) 8u,
                        /** Maximum supported Rx Flows */
                        (uint32_t) 64u,
                        /** Priority for all Rx transactions of this CPDMA */
                        (uint8_t) 0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        (uint8_t) 0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) CSL_BCP_CFG_PKTDMA_GLOBAL_CFG_REGS,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) CSL_BCP_CFG_PKTDMA_TX_CFG_REGS,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) CSL_BCP_CFG_PKTDMA_RX_CFG_REGS,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) CSL_BCP_CFG_PKTDMA_TX_SCHEDULER_CFG_REGS,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) CSL_BCP_CFG_PKTDMA_RX_FLOW_CFG_REGS,
                        /** RM DTS resource name for CPDMA rx channels */
                        "bcp-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "bcp-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "bcp-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_XGE_CPDMA,
                        /** Maximum supported Rx Channels */
                        0u,
                        /** Maximum supported Tx Channels */
                        0u,
                        /** Maximum supported Rx Flows */
                        0u,
                        /** Priority for all Rx transactions of this CPDMA */
                        0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) NULL,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) NULL,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) NULL,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) NULL,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) NULL,

                        /** RM DTS resource name for CPDMA rx channels */
                        "xge-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "xge-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "xge-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_NETCP_LOCAL_CPDMA,
                        /** Maximum supported Rx Channels */
                        (uint32_t) 0u,
                        /** Maximum supported Tx Channels */
                        (uint32_t) 0u,
                        /** Maximum supported Rx Flows */
                        (uint32_t) 0u,
                        /** Priority for all Rx transactions of this CPDMA */
                        (uint8_t) 0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        (uint8_t) 0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) NULL,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) NULL,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) NULL,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) NULL,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) NULL,
                        /** RM DTS resource name for CPDMA rx channels */
                        "netcp-local-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "netcp-local-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "netcp-local-rx-flow-id"
                },
                {
                        /** CPDMA this configuration belongs to */
                        Cppi_CpDma_IQN_CPDMA,
                        /** Maximum supported Rx Channels */
                        (uint32_t) 0u,
                        /** Maximum supported Tx Channels */
                        (uint32_t) 0u,
                        /** Maximum supported Rx Flows */
                        (uint32_t) 0u,
                        /** Priority for all Rx transactions of this CPDMA */
                        (uint8_t) 0u,
                        /** Priority for all Tx transactions of this CPDMA */
                        (uint8_t) 0u,

                        /** Base address for the CPDMA overlay registers */

                        /** Global Config registers */
                        (CSL_Cppidma_global_configRegs *) NULL,
                        /** Tx Channel Config registers */
                        (CSL_Cppidma_tx_channel_configRegs *) NULL,
                        /** Rx Channel Config registers */
                        (CSL_Cppidma_rx_channel_configRegs *) NULL,
                        /** Tx Channel Scheduler registers */
                        (CSL_Cppidma_tx_scheduler_configRegs *) NULL,
                        /** Rx Flow Config registers */
                        (CSL_Cppidma_rx_flow_configRegs *) NULL,

                        /** RM DTS resource name for CPDMA rx channels */
                        "iqn-rx-ch",
                        /** RM DTS resource name for CPDMA tx channels */
                        "iqn-tx-ch",
                        /** RM DTS resource name for CPDMA rx flows */
                        "iqn-rx-flow-id"
                }
        };

/** @brief CPPI LLD initialization parameters for system */
Cppi_GlobalConfigParams cppiGblCfgParams =
        {
                /** Configurations of each CPDMA */
                cppiGblCPDMACfgParams,
                /** Base address for first 4K queues */
                CSL_QMSS_DATA_QM1_QUEUE_MANAGEMENT_REGS,
                /** Base address for second 4K queues */
                CSL_QMSS_DATA_QM1_QUEUE_MANAGEMENT_REGS + 0x10000,
                /** Base address for third 4K queues */
                CSL_QMSS_DATA_QM2_QUEUE_MANAGEMENT_REGS,
                /** Base address for fourth 4K queues */
                CSL_QMSS_DATA_QM2_QUEUE_MANAGEMENT_REGS + 0x10000
        };

/**
@}
*/


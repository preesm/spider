/**
 * *****************************************************************************
 * Copyright or © or Copr. IETR/INSA: Maxime Pelcat, Jean-François Nezan,
 * Karol Desnos, Julien Heulot, Clément Guy, Yaset Oliva Venegas
 *
 * [mpelcat,jnezan,kdesnos,jheulot,cguy,yoliva]@insa-rennes.fr
 *
 * This software is a computer program whose purpose is to prototype
 * parallel applications.
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL-C
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
 * knowledge of the CeCILL-C license and that you accept its terms.
 * ****************************************************************************
 */
#include <stdio.h>
#include <stdint.h>

#include "edma.h"

#include <ti/csl/csl_edma3.h>
#include <ti/csl/csl_edma3Aux.h>
#include <ti/csl/csl_cacheAux.h>

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/

/* Global Variables which are used to dump the TPCC register overlay in the 
 * debugger... */
CSL_TpccRegs*  gEDMACC0Regs  = (CSL_TpccRegs*)CSL_EDMACC_0_REGS;
CSL_TpccRegs*  gEDMACC1Regs  = (CSL_TpccRegs*)CSL_EDMACC_1_REGS;
CSL_TpccRegs*  gEDMACC2Regs  = (CSL_TpccRegs*)CSL_EDMACC_2_REGS;

static CSL_Edma3Handle       	hEdma0, hEdma2;
static CSL_Status               status;
static CSL_Edma3Context         context;
static CSL_Edma3Obj             edmaObj0, edmaObj2;
static CSL_Edma3ChannelObj      ch0Obj, ch1Obj, ch2Obj, ch3Obj;
static CSL_Edma3ChannelAttr     chAttr;
static CSL_Edma3ChannelHandle   hChannel0, hChannel1, hChannel2, hChannel3;
static CSL_Edma3ParamHandle     hParam0, hParam1, hParam2, hParam3;

int edma_init(){
    /* Module initialization */
    if (CSL_edma3Init(&context) != CSL_SOK){
        printf ("Error: EDMA module initialization failed\n");
        return -1;
    }

    /* Open the EDMA Module using the provided instance number */
    hEdma0 = CSL_edma3Open(&edmaObj0, 0, NULL, &status);
    if ( (hEdma0 == NULL) || (status != CSL_SOK))
    {
        printf ("Error: EDMA module open failed\n");
        return -1;
    }
    hEdma2 = CSL_edma3Open(&edmaObj2, 2, NULL, &status);
    if ( (hEdma0 == NULL) || (status != CSL_SOK))
    {
        printf ("Error: EDMA module open failed\n");
        return -1;
    }

    /* Channel open */
    chAttr.regionNum = CSL_EDMA3_REGION_GLOBAL;
    chAttr.chaNum    = 0;
    hChannel0 = CSL_edma3ChannelOpen(&ch0Obj, 0, &chAttr, &status);
    if ((hChannel0 == NULL) || (status != CSL_SOK))
    {
        printf ("Error: Unable to open EDMA Channel:0\n");
        return -1;
    }
    chAttr.chaNum    = 1;
    hChannel1 = CSL_edma3ChannelOpen(&ch1Obj, 0, &chAttr, &status);
    if ((hChannel1 == NULL) || (status != CSL_SOK))
    {
        printf ("Error: Unable to open EDMA Channel:1\n");
        return -1;
    }
    chAttr.regionNum = CSL_EDMA3_REGION_GLOBAL;
    chAttr.chaNum    = 0;
    hChannel2 = CSL_edma3ChannelOpen(&ch2Obj, 2, &chAttr, &status);
    if ((hChannel2 == NULL) || (status != CSL_SOK))
    {
        printf ("Error: Unable to open EDMA Channel:2\n");
        return -1;
    }
    chAttr.chaNum    = 1;
    hChannel3 = CSL_edma3ChannelOpen(&ch3Obj, 2, &chAttr, &status);
    if ((hChannel3 == NULL) || (status != CSL_SOK))
    {
        printf ("Error: Unable to open EDMA Channel:3\n");
        return -1;
    }

    /* Map the DMA Channel to PARAM Block 0. */
    CSL_edma3MapDMAChannelToParamBlock (hEdma0, 0, 0);
    CSL_edma3MapDMAChannelToParamBlock (hEdma0, 1, 1);
    CSL_edma3MapDMAChannelToParamBlock (hEdma2, 0, 0);
    CSL_edma3MapDMAChannelToParamBlock (hEdma2, 1, 1);

    status = CSL_edma3HwChannelSetupQue(hChannel0, CSL_EDMA3_QUE_0);
    status = CSL_edma3HwChannelSetupQue(hChannel1, CSL_EDMA3_QUE_1);
    status = CSL_edma3HwChannelSetupQue(hChannel2, CSL_EDMA3_QUE_0);
    status = CSL_edma3HwChannelSetupQue(hChannel3, CSL_EDMA3_QUE_1);

    /* Obtain a handle to parameter set 0 */
    hParam0 = CSL_edma3GetParamHandle(hChannel0, 0, &status);
    if (hParam0 == NULL){
        printf ("Error: EDMA Get Parameter Entry failed for 2.\n");
        return -1;
    }
    hParam1 = CSL_edma3GetParamHandle(hChannel1, 1, &status);
    if (hParam1 == NULL){
        printf ("Error: EDMA Get Parameter Entry failed for 2.\n");
        return -1;
    }
    hParam2 = CSL_edma3GetParamHandle(hChannel2, 0, &status);
    if (hParam2 == NULL){
        printf ("Error: EDMA Get Parameter Entry failed for 2.\n");
        return -1;
    }
    hParam3 = CSL_edma3GetParamHandle(hChannel3, 1, &status);
    if (hParam3 == NULL){
        printf ("Error: EDMA Get Parameter Entry failed for 2.\n");
        return -1;
    }

	return 0;
}

int edma_clean(){
	/* Close channel */
	if (CSL_edma3ChannelClose(hChannel0) != CSL_SOK)
	{
		printf("Error: EDMA Channel Close failed\n");
		return -1;
	}

    if (CSL_edma3ChannelClose(hChannel1) != CSL_SOK)
    {
        printf("Error: EDMA Channel Close failed\n");
        return -1;
    }

    if (CSL_edma3ChannelClose(hChannel2) != CSL_SOK)
    {
        printf("Error: EDMA Channel Close failed\n");
        return -1;
    }

    if (CSL_edma3ChannelClose(hChannel3) != CSL_SOK)
    {
        printf("Error: EDMA Channel Close failed\n");
        return -1;
    }

	/* Close EDMA module */
	if (CSL_edma3Close(hEdma0) != CSL_SOK)
	{
		printf("Error: EDMA Module Close failed\n");
		return -1;
	}
    if (CSL_edma3Close(hEdma2) != CSL_SOK)
    {
        printf("Error: EDMA Module Close failed\n");
        return -1;
    }

	return 0;
}

int edma_cpy (
		void* src, void* dst,
		int aCnt, int bCnt, int cCnt,
		int srcBIdx, int dstBIdx,
		int srcCIdx, int dstCIdx)
{
    CSL_Edma3CmdIntr                regionIntr;
    CSL_Edma3ParamSetup             myParamSetup;

    /* Setup the parameter entry parameters (Ping buffer) */
    myParamSetup.option = CSL_EDMA3_OPT_MAKE(CSL_EDMA3_ITCCH_EN,
                                             CSL_EDMA3_TCCH_DIS,
                                             CSL_EDMA3_ITCINT_DIS,
                                             CSL_EDMA3_TCINT_EN,
                                             0,
                                             CSL_EDMA3_TCC_NORMAL,
                                             CSL_EDMA3_FIFOWIDTH_NONE,
                                             CSL_EDMA3_STATIC_DIS,
                                             CSL_EDMA3_SYNC_AB,
                                             CSL_EDMA3_ADDRMODE_INCR,
                                             CSL_EDMA3_ADDRMODE_INCR );
    myParamSetup.srcAddr    = (Uint32)src;
    myParamSetup.dstAddr    = (Uint32)dst;
    myParamSetup.aCntbCnt   = CSL_EDMA3_CNT_MAKE(aCnt,bCnt);
    myParamSetup.cCnt 		= cCnt;
    myParamSetup.srcDstBidx = CSL_EDMA3_BIDX_MAKE(srcBIdx,dstBIdx);
    myParamSetup.srcDstCidx = CSL_EDMA3_CIDX_MAKE(srcCIdx,dstCIdx);
    myParamSetup.linkBcntrld= 0xFFFF;

    /* Ping setup */
    if (CSL_edma3ParamSetup(hParam0,&myParamSetup) != CSL_SOK)
    {
        printf ("Error: EDMA Parameter Entry Setup failed\n");    
        return -1;
    }

    /* Interrupt enable (Bits 0-1)  for the global region interrupts */
    regionIntr.region = CSL_EDMA3_REGION_GLOBAL;
    regionIntr.intr   = 0x1;
    regionIntr.intrh  = 0x0000;
    CSL_edma3HwControl(hEdma0,CSL_EDMA3_CMD_INTR_ENABLE,&regionIntr);

    /* Trigger channel */
    CSL_edma3HwChannelControl(hChannel0,CSL_EDMA3_CMD_CHANNEL_SET,NULL);

    regionIntr.region = CSL_EDMA3_REGION_GLOBAL;
    regionIntr.intr   = 0;
    regionIntr.intrh  = 0;

    CSL_Edma3CtrlErrStat errStat;

    /* Poll on IPR bits 0 and 1 */
    do {
        CSL_edma3GetHwStatus(hEdma0,CSL_EDMA3_QUERY_INTRPEND,&regionIntr);
        CSL_edma3GetHwStatus(hEdma0,CSL_EDMA3_QUERY_CTRLERROR, &errStat);
    } while ((regionIntr.intr & 0x1) != 0x1);

    /* Clear the pending bit */
    CSL_edma3HwControl(hEdma0,CSL_EDMA3_CMD_INTRPEND_CLEAR,&regionIntr);


    /* The test passed. */
    return 0;
}

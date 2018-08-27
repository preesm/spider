#include <stdio.h>
#include <stdint.h>

#include "edma.h"

#include <ti/csl/device/k2h/src/cslr_device.h>
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

static CSL_Edma3Handle       	hModule;
static CSL_Status               status;
static CSL_Edma3Context         context;
static CSL_Edma3Obj             edmaObj;
static CSL_Edma3ChannelObj      chObj;
static CSL_Edma3ChannelAttr     chAttr;
static CSL_Edma3ChannelHandle   hChannel;
static CSL_Edma3ParamHandle     hParam;

int edma_init(){
    /* Module initialization */
    if (CSL_edma3Init(&context) != CSL_SOK){
        printf ("Error: EDMA module initialization failed\n");
        return -1;
    }

    /* Open the EDMA Module using the provided instance number */
    hModule = CSL_edma3Open(&edmaObj, 0, NULL, &status);
    if ( (hModule == NULL) || (status != CSL_SOK))
    {
        printf ("Error: EDMA module open failed\n");
        return -1;
    }

    /* Channel open */
    chAttr.regionNum = CSL_EDMA3_REGION_GLOBAL;
    chAttr.chaNum    = 0;
    hChannel = CSL_edma3ChannelOpen(&chObj, 0, &chAttr, &status);
    if ((hChannel == NULL) || (status != CSL_SOK)) 
    {
        printf ("Error: Unable to open EDMA Channel:0\n");
        return -1;
    }

    /* Map the DMA Channel to PARAM Block 0. */
    CSL_edma3MapDMAChannelToParamBlock (hModule, 0, 0);

    /* Obtain a handle to parameter set 0 */
    hParam = CSL_edma3GetParamHandle(hChannel, 0, &status);
    if (hParam == NULL){
        printf ("Error: EDMA Get Parameter Entry failed for 2.\n");
        return -1;
    }

	return 0;
}

int edma_clean(){
	/* Close channel */
	if (CSL_edma3ChannelClose(hChannel) != CSL_SOK)
	{
		printf("Error: EDMA Channel Close failed\n");
		return -1;
	}

	/* Close EDMA module */
	if (CSL_edma3Close(hModule) != CSL_SOK)
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
    if (CSL_edma3ParamSetup(hParam,&myParamSetup) != CSL_SOK)
    {
        printf ("Error: EDMA Parameter Entry Setup failed\n");    
        return -1;
    }
    
    /* Interrupt enable (Bits 0-1)  for the global region interrupts */
    regionIntr.region = CSL_EDMA3_REGION_GLOBAL;
    regionIntr.intr   = 0x1;
    regionIntr.intrh  = 0x0000;
    CSL_edma3HwControl(hModule,CSL_EDMA3_CMD_INTR_ENABLE,&regionIntr);

    /* Trigger channel */
    CSL_edma3HwChannelControl(hChannel,CSL_EDMA3_CMD_CHANNEL_SET,NULL); 

    regionIntr.region = CSL_EDMA3_REGION_GLOBAL;
    regionIntr.intr   = 0;
    regionIntr.intrh  = 0;

    /* Poll on IPR bit 0 */
    do {
        CSL_edma3GetHwStatus(hModule,CSL_EDMA3_QUERY_INTRPEND,&regionIntr);
    } while (!(regionIntr.intr & 0x1));

    /* Clear the pending bit */
    CSL_edma3HwControl(hModule,CSL_EDMA3_CMD_INTRPEND_CLEAR,&regionIntr);   


    /* The test passed. */
    return 0;
}

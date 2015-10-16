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

#include <ti/csl/csl_edma3.h>
#include <ti/csl/csl_edma3Aux.h>
#include <ti/csl/csl_cacheAux.h>

#include <math.h>

int bitreverse_LUT[] = {
		0, 8,	4, 12,
		2, 10,	6, 14,
		1, 9,	5, 13,
		3, 11,	7, 15 };

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/

/* Global Variables which are used to dump the TPCC register overlay in the
 * debugger... */
CSL_TpccRegs*  gEDMACC0Regs  = (CSL_TpccRegs*)CSL_EDMACC_0_REGS;
CSL_TpccRegs*  gEDMACC1Regs  = (CSL_TpccRegs*)CSL_EDMACC_1_REGS;
CSL_TpccRegs*  gEDMACC2Regs  = (CSL_TpccRegs*)CSL_EDMACC_2_REGS;

/* 64k application specific EDMA transfer */
#define FFTSIZE 65536
#define N1 16
#define N2 4096

/* Platform config*/
#define EDMA_EVTQUEUE_NB 4

static CSL_Edma3Handle			 hEdma1, hEdma2;
static CSL_Status               status;
static CSL_Edma3Context         context;
static CSL_Edma3Obj             edmaObj1, edmaObj2;
static CSL_Edma3ChannelObj      chObj[N1];
static CSL_Edma3ChannelHandle   hChannel[N1];
static CSL_Edma3ParamHandle     hParam[N1];

int edmabr_init(void* src, void* dst,
		unsigned short aCnt, unsigned short bCnt, unsigned short cCnt,
		short srcBIdx, short dstBIdx,
		short srcCIdx, short dstCIdx){
	CSL_Edma3ChannelAttr     chAttr;
	CSL_Edma3ParamSetup      myParamSetup;
	int i;


    /* Module initialization */
    if (CSL_edma3Init(&context) != CSL_SOK){
        printf ("Error: EDMA module initialization failed\n");
        return -1;
    }

    /* Open the EDMA Module using the provided instance number.
     * Instance number correspond to the EDMA channel controller? */
    hEdma1 = CSL_edma3Open(&edmaObj1, 1, NULL, &status);
	if ( (hEdma1 == NULL) || (status != CSL_SOK))
	{
		printf ("Error: EDMA module open failed\n");
		return -1;
	}

	hEdma2 = CSL_edma3Open(&edmaObj2, 2, NULL, &status);
	if ( (hEdma2 == NULL) || (status != CSL_SOK))
	{
		printf ("Error: EDMA module open failed\n");
		return -1;
	}


    /* Channel open. Open N1 channel on the channel controller no 0 */
    //second parameter correspond to the channel controller?
    //each edma controller have 64 channels?
	chAttr.regionNum = CSL_EDMA3_REGION_GLOBAL;
	int edmaCC;
	for (i=0; i < N1; i++) {
		chAttr.chaNum = i%8; //correspond to the channel to open
		edmaCC = i/8+1;		 //compute the edma on which the channel will be associated. (1 or 2)

		hChannel[i] = CSL_edma3ChannelOpen(&chObj[i], edmaCC, &chAttr, &status);
		if ((hChannel[i] == NULL) || (status != CSL_SOK))
		{
			printf ("Error: Unable to open EDMA %d with Channel:%d\n", edmaCC,i);
			return -1;
		}
	}

    /* Map the DMA Channel to PARAM Block 0.
     * fst param corresponds to the channel, 2nd correspond to the param in the set to map to.
     * The following loop map the first N1 params with the first N1 channels. It is needed (cf doc)? */
	for (i=0; i < N1; i++) {
		if(i/8 == 0)
			CSL_edma3MapDMAChannelToParamBlock (hEdma1, i%8, i%8);
		else
			CSL_edma3MapDMAChannelToParamBlock (hEdma2, i%8, i%8);
	}

	/* Specify on which queue each channel should be associated with. */
	for (i=0; i < N1; i+=4) {
		status = CSL_edma3HwChannelSetupQue(hChannel[i], CSL_EDMA3_QUE_0);
		status = CSL_edma3HwChannelSetupQue(hChannel[i+1], CSL_EDMA3_QUE_1);
		status = CSL_edma3HwChannelSetupQue(hChannel[i+2], CSL_EDMA3_QUE_2);
		status = CSL_edma3HwChannelSetupQue(hChannel[i+3], CSL_EDMA3_QUE_3);
	}

    /* Obtain a handle to each PaRAM*/
	for (i=0; i < N1; i++) {
		hParam[i] = CSL_edma3GetParamHandle(hChannel[i], i%8, &status);
		if (hParam[i] == NULL){
			printf ("Error: EDMA Get Parameter Entry failed for 2.\n");
			return -1;
		}
	}

	for(i=0; i < N1; i++) {
		//Configure OPT
		if((i+1)%8) {
			myParamSetup.option =
				CSL_EDMA3_OPT_MAKE(
					CSL_EDMA3_ITCCH_DIS, 	//disable intermediate completion chaining. cf page 48
					CSL_EDMA3_TCCH_EN, 		//enable chaining cf page 48
					CSL_EDMA3_ITCINT_DIS,
					CSL_EDMA3_TCINT_EN,
					i%8+1, 					//chain this param with the next channel
					CSL_EDMA3_TCC_EARLY, 	//trigger channel chaining before the transfer
					CSL_EDMA3_FIFOWIDTH_NONE,
					CSL_EDMA3_STATIC_DIS,
					CSL_EDMA3_SYNC_AB,
					CSL_EDMA3_ADDRMODE_INCR,
					CSL_EDMA3_ADDRMODE_INCR );

			//Configure addresses and counts
			myParamSetup.srcAddr    = (Uint32)src + aCnt*bitreverse_LUT[i]; //TODO 4 is size of the structure. make it configurable
			myParamSetup.dstAddr    = (Uint32)dst + aCnt*N2*i; 				//4096 * i
			myParamSetup.aCntbCnt   = CSL_EDMA3_CNT_MAKE(aCnt,bCnt);
			myParamSetup.cCnt 	    = cCnt;
			myParamSetup.srcDstBidx = CSL_EDMA3_BIDX_MAKE(srcBIdx,dstBIdx);
			myParamSetup.srcDstCidx = CSL_EDMA3_CIDX_MAKE(srcCIdx,dstCIdx);
			myParamSetup.linkBcntrld= 0xFFFF;
		} else {
			//Don't chain the last param!
			myParamSetup.option =
				CSL_EDMA3_OPT_MAKE(
					CSL_EDMA3_ITCCH_DIS,
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

			myParamSetup.srcAddr    = ((Uint32)src) + aCnt*bitreverse_LUT[N1-1] ;
			myParamSetup.dstAddr    = ((Uint32)dst) + aCnt*N2*(N1-1);
			myParamSetup.aCntbCnt   = CSL_EDMA3_CNT_MAKE(aCnt,bCnt);
			myParamSetup.cCnt 	    = cCnt;
			myParamSetup.srcDstBidx = CSL_EDMA3_BIDX_MAKE(srcBIdx,dstBIdx);
			myParamSetup.srcDstCidx = CSL_EDMA3_CIDX_MAKE(srcCIdx,dstCIdx);
			myParamSetup.linkBcntrld= 0xFFFF;
		}


		//apply the PaRAM structure on the handle retrieved during the init process
		if (CSL_edma3ParamSetup(hParam[i],&myParamSetup) != CSL_SOK)
		{
			printf ("Error: EDMA Parameter Entry Setup failed\n");
			return -1;
		}
	}

	return 0;
}

int edmabr_clean(){
	/* Close channel */
	int i;

	for (i=0; i < N1; i++) {
		if (CSL_edma3ChannelClose(hChannel[i]) != CSL_SOK)
		{
			printf("Error: EDMA Channel %d Close failed\n", i);
			return -1;
		}
	}

	/* Close EDMA module */
	if (CSL_edma3Close(hEdma1) != CSL_SOK)
	{
		printf("Error: EDMA Module Close failed\n");
		return -1;
	}

	return 0;
}

int edmabr_cpy_jumpaddr_chained_bitreverse (
		)
{
	CSL_Edma3CmdIntr                regionIntr;


	/* Setup the parameter entry parameters (Ping buffer) */
	/* FIRST TRANSFER */

	/* Create N1 params that are chained. */



	/* Interrupt enable (Bits 0-1)  for the global region interrupts */
	regionIntr.region = CSL_EDMA3_REGION_GLOBAL;
	regionIntr.intr   = 0x00FF; //wait for the 8 transfers to be done. (IPR)
	regionIntr.intrh  = 0x0000;
	CSL_edma3HwControl(hEdma1,CSL_EDMA3_CMD_INTR_ENABLE,&regionIntr);
	CSL_edma3HwControl(hEdma2,CSL_EDMA3_CMD_INTR_ENABLE,&regionIntr);

	/* Trigger channel */
	CSL_edma3HwChannelControl(hChannel[0],CSL_EDMA3_CMD_CHANNEL_SET,NULL);
	CSL_edma3HwChannelControl(hChannel[8],CSL_EDMA3_CMD_CHANNEL_SET,NULL);

	regionIntr.region = CSL_EDMA3_REGION_GLOBAL;
	regionIntr.intr   = 0;
	regionIntr.intrh  = 0;

	CSL_Edma3CtrlErrStat errStat;

	/* Wait for edma1 to finish */
	do {
		CSL_edma3GetHwStatus(hEdma1,CSL_EDMA3_QUERY_INTRPEND,&regionIntr); //interrupt pending
		CSL_edma3GetHwStatus(hEdma1,CSL_EDMA3_QUERY_CTRLERROR, &errStat);
	} while ((regionIntr.intr & 0x00FF) != 0x00FF); //check channel 0 to 15 for completion

	/* Clear the pending bit */
	CSL_edma3HwControl(hEdma1,CSL_EDMA3_CMD_INTRPEND_CLEAR,&regionIntr);

	regionIntr.region = CSL_EDMA3_REGION_GLOBAL;
	regionIntr.intr   = 0;
	regionIntr.intrh  = 0;

	/* Wait for edma2 to finish */
	do {
		CSL_edma3GetHwStatus(hEdma2,CSL_EDMA3_QUERY_INTRPEND,&regionIntr); //interrupt pending
		CSL_edma3GetHwStatus(hEdma2,CSL_EDMA3_QUERY_CTRLERROR, &errStat);
	} while ((regionIntr.intr & 0x00FF) != 0x00FF); //check channel 0 to 15 for completion
	CSL_edma3HwControl(hEdma2,CSL_EDMA3_CMD_INTRPEND_CLEAR,&regionIntr);


	/* The test passed. */
	return 0;
}

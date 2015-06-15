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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* FFTC test application input data/ configuration files */
#include <ti/csl/csl_qm_queue.h>
#include <ti/csl/csl_psc.h>
#include <ti/csl/csl_pscAux.h>


/* QM Accumalator firmware include */
#include <ti/drv/qmss/qmss_qm.h>
#include <ti/drv/cppi/cppi_drv.h>
#include <ti/drv/cppi/cppi_desc.h>

/* CSL INTC include */
#include <ti/csl/src/intc/csl_intc.h>

#include <ti/csl/csl_tsc.h>
#include <ti/csl/cslr_fftc.h>

#include "osal.h"
#include "init.h"


/* Type Definition */
typedef struct Cplx16{
	short	imag;
	short	real;
} Cplx16;

typedef struct Cplx32{
	long	imag;
	long	real;
} Cplx32;

typedef struct CplxSp{
	float	imag;
	float	real;
} CplxSp;

#define     FFTC_TEST_NUM_HOST_DESC             32
#define     FFTC_TEST_SIZE_HOST_DESC            64
#define     FFTC_CPPI_HOST_DESC_SIZE           (32)
#define     FFTC_TEST_SAMPLE_SIZE               4

static int freeRxQueue[2] 	= {QUEUE_FREE_RX_FFTC_A, 	QUEUE_FREE_RX_FFTC_B};
static int rxQueue[2] 		= {QUEUE_RX_FFTC_A, 		QUEUE_RX_FFTC_B};
static int txQueue[2] 		= {QUEUE_TX_FFTC_A, 		QUEUE_TX_FFTC_B};

int fftc_send (int fftc_ix, Cplx16* in, Cplx16* out, int fftSize, int numBlocks){
    Cplx16 *pResultBuffer;
    uint8_t* phy_out, *phy_in;
	uint32_t ResultBufferLen;

    int lg = setFFTSize(fftc_ix, fftSize);

    int bufferSize = (sizeof(Cplx16) * numBlocks) << lg;

	void* pCppiDesc;
    int descSize;
	Cppi_DescType descType;
	Qmss_Queue queue_free_fftc;

	/* Setup RX Packet */
	/* Get a free descriptor */
	if ((pCppiDesc = Qmss_queuePop (QUEUE_FREE_FFTC)) == NULL){
		printf ("Out of descriptors!! Cannot get a free Tx descriptor \n");
	}

	descSize  = QMSS_DESC_SIZE(pCppiDesc);
	pCppiDesc = (void*) (QMSS_DESC_PTR (pCppiDesc));

	Osal_DescBeginMemAccess(pCppiDesc, descSize);
	descType  = Cppi_getDescType (pCppiDesc);

	queue_free_fftc.qMgr = 0;
	queue_free_fftc.qNum = QUEUE_FREE_FFTC;

	phy_out = Osal_qmssVirtToPhy(out);
	Cppi_setData 			(Cppi_DescType_HOST, pCppiDesc, (uint8_t*)phy_out, bufferSize);
	Cppi_setOriginalBufInfo (Cppi_DescType_HOST, pCppiDesc, (uint8_t*)phy_out, bufferSize);
	Cppi_setReturnQueue 	(Cppi_DescType_HOST, pCppiDesc, queue_free_fftc);
	Cppi_setPacketLen 		(Cppi_DescType_HOST, pCppiDesc, bufferSize);

	/* Sync Descriptor */
	Osal_DescEndMemAccess(pCppiDesc, descSize);

	/* Push descriptor back to free queue */
	Qmss_queuePushDescSize (freeRxQueue[fftc_ix], pCppiDesc, descSize);

	/* Send a Tx descriptor */
	if ((pCppiDesc = Qmss_queuePop (QUEUE_FREE_FFTC)) == NULL){
		printf ("Out of descriptors!! Cannot get a free Tx descriptor \n");
	}

	descSize  = QMSS_DESC_SIZE(pCppiDesc);
	pCppiDesc = (void*) (QMSS_DESC_PTR (pCppiDesc));

	Osal_DescBeginMemAccess(pCppiDesc, descSize);

	descType  = Cppi_getDescType (pCppiDesc);

	queue_free_fftc.qMgr = 0;
	queue_free_fftc.qNum = QUEUE_FREE_FFTC;

	phy_in = Osal_qmssVirtToPhy(in);
	Cppi_setData 			(Cppi_DescType_HOST, pCppiDesc, (uint8_t*)phy_in, bufferSize);
	Cppi_setOriginalBufInfo (Cppi_DescType_HOST, pCppiDesc, (uint8_t*)phy_in, bufferSize);
	Cppi_setReturnQueue 	(Cppi_DescType_HOST, pCppiDesc, queue_free_fftc);
	Cppi_setPacketLen 		(Cppi_DescType_HOST, pCppiDesc, bufferSize);

	/* Sync Descriptor */
	Osal_DescEndMemAccess(pCppiDesc, descSize);

	/* Push descriptor onto FFTC Tx queue */
	Qmss_queuePushDescSize (txQueue[fftc_ix], pCppiDesc, descSize);

	/* Get the raw result from the engine. */
	int i=0;
	do{
		pCppiDesc = Qmss_queuePop (rxQueue[fftc_ix]);
		i++;
	}while (pCppiDesc == NULL && i<10000);

	if(i==10000){
		printf("FFTC timeout\n");
		printQueueState();
	}

	descSize  = QMSS_DESC_SIZE(pCppiDesc);
	pCppiDesc = (void*) (QMSS_DESC_PTR (pCppiDesc));

	Osal_DescBeginMemAccess(pCppiDesc, descSize);

	descType  = Cppi_getDescType (pCppiDesc);

	/* Get Data buffer containing the result and its length */
	Cppi_getData (descType, pCppiDesc, (uint8_t**)&pResultBuffer, &ResultBufferLen);
	pResultBuffer = Osal_qmssPhyToVirt(pResultBuffer);

	/* Verify Data Location */
	if(pResultBuffer != out){
		printf("Result Location: get %#x, %#x expected\n", (int)pResultBuffer, (int)out);
		return 1;
	}

	/* Verify Length */
	if(ResultBufferLen != bufferSize){
		printf("Bad Result Length: get %d, %d expected\n", ResultBufferLen, bufferSize);
		return 1;
	}

	/* Send Descriptor to free Queue */
	Cppi_setData 			(Cppi_DescType_HOST, pCppiDesc, (uint8_t*)0, 0);
	Cppi_setOriginalBufInfo (Cppi_DescType_HOST, pCppiDesc, (uint8_t*)0, 0);
	Cppi_setPacketLen 		(Cppi_DescType_HOST, pCppiDesc, 0);

	Osal_DescEndMemAccess(pCppiDesc, descSize);

	Qmss_queuePushDescSize(QUEUE_FREE_FFTC, pCppiDesc, descSize);

	/* Return success. */
	return 0;
}

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
#include "K2DspLrtCommunicator.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <platform.h>
#include <lrt.h>
#include <algorithm>

extern "C" {
#include <ti/drv/qmss/qmss_drv.h>
#include <ti/drv/cppi/cppi_drv.h>
#include <ti/drv/cppi/cppi_desc.h>
#include "init.h"
#include "cache.h"
}

#define CTRL_SPIDER_TO_LRT    (QUEUE_CTRL_DOWN_0    + Platform::get()->getLrt()->getIx())
#define CTRL_LRT_TO_SPIDER    (QUEUE_CTRL_UP_0    + Platform::get()->getLrt()->getIx())

static Cppi_Desc *cur_mono_pkt_in;
static int cur_mono_pkt_in_size;
static Cppi_Desc *cur_mono_pkt_out;
static int cur_mono_pkt_out_size;
static Cppi_Desc *cur_mono_trace_out;
static int cur_mono_trace_out_size;

K2DspLrtCommunicator::K2DspLrtCommunicator() {
    cur_mono_pkt_in = 0;
    cur_mono_pkt_out = 0;
    cur_mono_trace_out = 0;
    maxCtrlMsgSize = 0;
    maxTraceMsgSize = 0;
    maxDataMsgSize = 0;
}

K2DspLrtCommunicator::~K2DspLrtCommunicator() {
    printf("LRT Ctrl msg Max: %d / %d\n", maxCtrlMsgSize, CTRL_DESC_SIZE);
    printf("LRT Trace msg Max: %d / %d\n", maxTraceMsgSize, TRACE_DESC_SIZE);
    printf("LRT Data msg Max: %d / %d\n", maxDataMsgSize, DATA_DESC_SIZE);
}

void *K2DspLrtCommunicator::ctrl_start_send(int size) {
    int dataOffset;
    if (cur_mono_pkt_out != 0)
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");

    maxCtrlMsgSize = std::max(maxCtrlMsgSize, size);

    while (cur_mono_pkt_out == 0) {
        cur_mono_pkt_out = (Cppi_Desc *) Qmss_queuePop(QUEUE_FREE_CTRL);

        if (cur_mono_pkt_out != 0) {
            /* Get Packet info */
            cur_mono_pkt_out_size = CTRL_DESC_SIZE;//QMSS_DESC_SIZE(cur_mono_pkt_out);
            cur_mono_pkt_out = (Cppi_Desc *) QMSS_DESC_PTR(cur_mono_pkt_out);

            /* Clear Cache */
            Osal_qmssBeginMemAccess(cur_mono_pkt_out, cur_mono_pkt_out_size);

            /* Get info */
            dataOffset = Cppi_getDataOffset(Cppi_DescType_MONOLITHIC, cur_mono_pkt_out);
            break;
        }
//		usleep(100);
    }

    if (size > cur_mono_pkt_out_size - dataOffset) {
        printf("%d > %d\n", size, cur_mono_pkt_out_size - dataOffset);
        throw std::runtime_error("LrtCommunicator: Try to send a message too big");
    }

    /* Add data to current descriptor */
    void *data_pkt = (void *) (((int) cur_mono_pkt_out) + dataOffset);
    return data_pkt;
}

void K2DspLrtCommunicator::ctrl_end_send(int size) {
    if (cur_mono_pkt_out) {
        /* Send the descriptor */
        Osal_qmssEndMemAccess(cur_mono_pkt_out, cur_mono_pkt_out_size);
        Qmss_queuePushDesc(CTRL_LRT_TO_SPIDER, (void *) cur_mono_pkt_out);

        cur_mono_pkt_out = 0;
        cur_mono_pkt_out_size = 0;
    } else
        throw std::runtime_error("LrtCommunicator: Try to send a free'd message");
}

int K2DspLrtCommunicator::ctrl_start_recv(void **data) {
    int dataOffset;

    if (cur_mono_pkt_in == 0) {
        cur_mono_pkt_in = (Cppi_Desc *) Qmss_queuePop(CTRL_SPIDER_TO_LRT);
        if (cur_mono_pkt_in == 0) {
            return 0;
        }

        /* Get Packet info */
        cur_mono_pkt_in_size = CTRL_DESC_SIZE;//QMSS_DESC_SIZE(cur_mono_pkt_in);
        cur_mono_pkt_in = (Cppi_Desc *) QMSS_DESC_PTR(cur_mono_pkt_in);

        /* Clear Cache */
        Osal_qmssBeginMemAccess(cur_mono_pkt_in, cur_mono_pkt_in_size);

        /* Get info */
        dataOffset = Cppi_getDataOffset(Cppi_DescType_MONOLITHIC, cur_mono_pkt_in);
    } else
        throw std::runtime_error("LrtCommunicator: Try to receive a message when the previous one is not free'd");

    void *data_pkt = (void *) (((int) cur_mono_pkt_in) + dataOffset);
    int data_size = cur_mono_pkt_in_size - dataOffset;

    *data = data_pkt;
    return data_size;
}

void K2DspLrtCommunicator::ctrl_end_recv() {
    if (cur_mono_pkt_in) {
        /* Send the descriptor */
        Osal_qmssEndMemAccess(cur_mono_pkt_in, cur_mono_pkt_in_size);
        Qmss_queuePushDesc(QUEUE_FREE_CTRL, cur_mono_pkt_in);

        cur_mono_pkt_in = 0;
        cur_mono_pkt_in_size = 0;
    } else
        throw std::runtime_error("LrtCommunicator: Try to send a free'd message");
}

void *K2DspLrtCommunicator::trace_start_send(int size) {
    int dataOffset;

    if (cur_mono_trace_out != 0)
        throw std::runtime_error("LrtCommunicator: Try to send a trace msg when previous one is not sent");

    maxTraceMsgSize = std::max(maxTraceMsgSize, size);

    while (cur_mono_trace_out == 0) {
        cur_mono_trace_out = (Cppi_Desc *) Qmss_queuePop(QUEUE_FREE_TRACE);

        if (cur_mono_trace_out != 0) {
            /* Get Packet info */
            cur_mono_trace_out_size = TRACE_DESC_SIZE;//QMSS_DESC_SIZE(cur_mono_trace_out);
            cur_mono_trace_out = (Cppi_Desc *) QMSS_DESC_PTR(cur_mono_trace_out);

            /* Clear Cache */
            Osal_qmssBeginMemAccess(cur_mono_trace_out, cur_mono_trace_out_size);

            /* Get info */
            dataOffset = Cppi_getDataOffset(Cppi_DescType_MONOLITHIC, cur_mono_trace_out);
            break;
        }
//		usleep(100);
    }

    if (size > cur_mono_trace_out_size - dataOffset) {
        printf("%d > %d\n", size, cur_mono_trace_out_size - dataOffset);
        throw std::runtime_error("LrtCommunicator: Try to send a message too big");
    }

    /* Add data to current descriptor */
    void *data_pkt = (void *) (((int) cur_mono_trace_out) + dataOffset);
    return data_pkt;
}

void K2DspLrtCommunicator::trace_end_send(int size) {
    if (cur_mono_trace_out) {
        /* Send the descriptor */
        Osal_qmssEndMemAccess(cur_mono_trace_out, cur_mono_trace_out_size);
        Qmss_queuePushDesc(QUEUE_TRACE, cur_mono_trace_out);

        cur_mono_trace_out = 0;
        cur_mono_trace_out_size = 0;
    } else
        throw std::runtime_error("LrtCommunicator: Try to send a free'd message");
}

long K2DspLrtCommunicator::data_start_send(Fifo *f) {
    return (long) Platform::get()->virt_to_phy((void *) (f->alloc));
}

void K2DspLrtCommunicator::data_end_send(Fifo *f) {
    /* Write back cache */
//	if(f->ntoken){
    Osal_qmssEndMemAccess(
            Platform::get()->virt_to_phy((void *) (f->alloc)),
            f->size);
//	}

    if (f->ntoken) {
        Cppi_Desc *mono_pkt;
        int queueId = QUEUE_DATA_BASE + f->id;

        if (queueId < QUEUE_DATA_BASE || queueId > QUEUE_LAST)
            throw std::runtime_error("Error: request queue out of bound\n");

        for (int i = 0; i < f->ntoken; i++) {
            do {
                mono_pkt = (Cppi_Desc *) Qmss_queuePop(QUEUE_FREE_DATA);
            } while (mono_pkt == 0);

            /* Get Packet info */
            int mono_pkt_size = DATA_DESC_SIZE;//QMSS_DESC_SIZE(mono_pkt);
            mono_pkt = (Cppi_Desc *) QMSS_DESC_PTR(mono_pkt);

//			/* Clear Cache */
//			Osal_qmssBeginMemAccess(mono_pkt, mono_pkt_size);
//
//			/* Write back cache */
//			Osal_qmssEndMemAccess(mono_pkt, mono_pkt_size);

            /* Send Packet */
            Qmss_queuePushDesc(queueId, mono_pkt);
        }
    }
}

long K2DspLrtCommunicator::data_recv(Fifo *f) {
    if (f->ntoken) {
        Cppi_Desc *mono_pkt;
        int queueId = QUEUE_DATA_BASE + f->id;

        if (queueId < QUEUE_DATA_BASE || queueId > QUEUE_LAST)
            throw std::runtime_error("Error: request queue out of bound\n");

        for (int i = 0; i < f->ntoken; i++) {
            do {
                mono_pkt = (Cppi_Desc *) Qmss_queuePop(queueId);
            } while (mono_pkt == 0);

            /* Get Packet info */
            int mono_pkt_size = DATA_DESC_SIZE;//QMSS_DESC_SIZE(mono_pkt);
            mono_pkt = (Cppi_Desc *) QMSS_DESC_PTR(mono_pkt);

//			/* Clear Cache */
//			Osal_qmssBeginMemAccess(mono_pkt, mono_pkt_size);
//
//			/* Write back cache */
//			Osal_qmssEndMemAccess(mono_pkt, mono_pkt_size);

            Qmss_queuePushDesc(QUEUE_FREE_DATA, mono_pkt);
        }
    }

    /* Invalidate cache */
//	if(f->ntoken){
    Osal_qmssBeginMemAccess(
            Platform::get()->virt_to_phy((void *) (f->alloc)),
            f->size);
//	}

    return (long) Platform::get()->virt_to_phy((void *) (f->alloc));
}

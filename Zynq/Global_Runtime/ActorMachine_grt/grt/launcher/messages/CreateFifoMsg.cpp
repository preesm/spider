
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#include "CreateFifoMsg.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../Memory.h"
#include <hwQueues.h>

CreateFifoMsg::CreateFifoMsg(SRDAGGraph* graph, SRDAGEdge* edge, Memory *mem){
	fifoID = graph->getEdgeIndex(edge);
	fifoSize = edge->getTokenRate()+1;
	fifoAdd = mem->alloc(fifoSize);
}

CreateFifoMsg::CreateFifoMsg(SRDAGGraph* graph, SRDAGEdge* edge, UINT32 token_size, Memory *mem){
	fifoID = graph->getEdgeIndex(edge);
	fifoSize = edge->getTokenRate() * token_size;
	fifoAdd = mem->alloc(fifoSize);
}

CreateFifoMsg::CreateFifoMsg(PiSDFEdge* edge, UINT32 token_size, Memory *mem){
	fifoID = edge->getId();
	fifoSize = edge->getProductionInt() * token_size;
	fifoAdd = mem->alloc(fifoSize);
}

CreateFifoMsg::CreateFifoMsg(UINT32 fifoID, UINT32 fifoSize, Memory *mem){
	this->fifoID = fifoID;
	this->fifoAdd = mem->alloc(fifoSize);
	this->fifoSize = fifoSize;
};

void CreateFifoMsg::send(int LRTID){
	OS_CtrlQPushInt(LRTID, MSG_CREATE_FIFO);
	OS_CtrlQPushInt(LRTID, fifoID);
	OS_CtrlQPushInt(LRTID, fifoSize);
	OS_CtrlQPushInt(LRTID, fifoAdd);
}

int CreateFifoMsg::prepare(int* data, int offset){
	int size = 0;
	data[offset + size++] = MSG_CREATE_FIFO;
	data[offset + size++] = fifoID;
	data[offset + size++] = fifoSize;
	data[offset + size++] = fifoAdd;
	return size;
}

void CreateFifoMsg::prepare(int slave, launcher* launch){
	launch->addUINT32ToSend(slave, MSG_CREATE_FIFO);
	launch->addUINT32ToSend(slave, fifoID);
	launch->addUINT32ToSend(slave, fifoSize);
	launch->addUINT32ToSend(slave, fifoAdd);
//	launch->addUINT32ToReceive(slave, MSG_CREATE_FIFO);
}

/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include "StopTaskMsg.h"
#include <hwQueues.h>
#include <grt_definitions.h>

StopTaskMsg::StopTaskMsg(INT32 _TaskID, INT32 _VectorID): TaskID(_TaskID), VectorID(_VectorID) {
}

void StopTaskMsg::send(int LRTID){
	RTQueuePush_UINT32(LRTID, RTCtrlQueue, MSG_STOP_TASK);
	RTQueuePush_UINT32(LRTID, RTCtrlQueue, TaskID);
	RTQueuePush_UINT32(LRTID, RTCtrlQueue, VectorID);

	RTQueuePop_UINT32(LRTID, RTCtrlQueue);
}

void StopTaskMsg::sendWOCheck(int LRTID){
	RTQueuePush_UINT32(LRTID, RTCtrlQueue, MSG_STOP_TASK);
	RTQueuePush_UINT32(LRTID, RTCtrlQueue, TaskID);
	RTQueuePush_UINT32(LRTID, RTCtrlQueue, VectorID);
}

int StopTaskMsg::prepare(int* data, int offset){
	int size = 0;
	data[offset + size++] = MSG_STOP_TASK;
	data[offset + size++] = TaskID;
	data[offset + size++] = VectorID;
	return size;
}

//void StopTaskMsg::prepare(int slave, launcher* launch){
//	launch->addUINT32ToSend(slave, MSG_STOP_TASK);
//}

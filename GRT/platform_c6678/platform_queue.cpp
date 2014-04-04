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

#include <platform_types.h>
#include <platform_queue.h>
#include <grt_definitions.h>

extern "C"{
void 	__c_platform_queue_Init();
UINT32 	__c_platform_QPush(UINT8 slaveId, platformQType queueType, void* data, int size);
UINT32 	__c_platform_QPushUINT32(UINT8 slaveId, platformQType queueType, UINT32 data);
UINT32 	__c_platform_QPop(UINT8 slaveId, platformQType queueType, void* data, int size);
UINT32 	__c_platform_QPopUINT32(UINT8 slaveId, platformQType queueType);
UINT32 	__c_platform_QNonBlockingPop(UINT8 slaveId, platformQType queueType, void* data, int size);
void __c_platform_QPush_finalize(UINT8 slaveId, platformQType queueType);
}

void platform_queue_Init(){
	__c_platform_queue_Init();
}

UINT32 platform_QPush(UINT8 slaveId, platformQType queueType, void* data, int size){
	return __c_platform_QPush(slaveId, queueType, data, size);
}

UINT32 platform_QPushUINT32(UINT8 slaveId, platformQType queueType, UINT32 data){
	return __c_platform_QPushUINT32(slaveId, queueType, data);
}

void platform_QPush_finalize(UINT8 slaveId, platformQType queueType){
	__c_platform_QPush_finalize(slaveId, queueType);
}

UINT32 platform_QPop(UINT8 slaveId, platformQType queueType, void* data, int size){
	return __c_platform_QPop(slaveId, queueType, data, size);
}

UINT32 platform_QPopUINT32(UINT8 slaveId, platformQType queueType){
	return __c_platform_QPopUINT32(slaveId, queueType);
}
UINT32 platform_QNonBlockingPop(UINT8 slaveId, platformQType queueType, void* data, int size){
	return __c_platform_QNonBlockingPop(slaveId, queueType, data, size);
}

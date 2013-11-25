/********************************************************************************
 * File    : lrt_prototypes.h
 * By      : Yaset Oliva Venegas
 * Version : 1.0
 * Date	   : October 2012
 * Descrip : This file contains the prototypes of the RunTime's functions.
 * 			Prototypes should be MTAPI-compliant.
 *********************************************************************************/

#ifndef LRT_PROTOTYPES
#define LRT_PROTOTYPES

#include "lrt_definitions.h"

/***************** At lrt_am.c *****************************/

/***************** At lrt_core.c ***************************/
void init_lrt();
void OSStartCur();
void OSSched();

/***************** At lrt_ctrlMsg.c ***********************/
void send_ext_msg(UINT32 addr, UINT32 msg_type, void* msg);
void wait_ext_msg();

/***************** At lrt_dataFifo.c **********************/
UINT8 create_fifo();
UINT8 create_fifo_args(UINT8 id, UINT32 size, UINT32 address);
void flush_fifo();
void flush_fifo_args(UINT8 fifo_id);
BOOLEAN check_input_fifo(UINT8 in_fifo_id, UINT32 size);
BOOLEAN check_output_fifo(UINT8 out_fifo_id, UINT32 size);
UINT32 get_fifo_cnt(UINT8 fifo_id);
void write_output_fifo(UINT8 out_fifo_id, UINT32 size, UINT8* buffer);
void read_input_fifo(UINT8 in_fifo_id, UINT32 size, UINT8* buffer);

/***************** At lrt_debug.c *************************/
void exitWithCode(int exitCode);

/***************** At lrt_dotWriter.c *********************/
void dotWriter(OS_TCB* graph, const char* path);

/***************** At lrt_taskMngr.c **********************/
void LrtTaskCreate();
UINT8 OSTaskQuery(UINT8 id, OS_TCB *p_task_data);
AM_ACTOR_ACTION_STRUCT* OSCurActionQuery();
void LrtTaskDeleteCur();
void OSTaskDel();
void OSWorkingMemoryInit();
void* OSAllocWorkingMemory(int size);
void OSFreeWorkingMemory();

/***************** At lrt_monitor.c **********************/
void initMonitor();
void resetMonitor();
MonitorAction switchMonitor(MonitorAction action);
void printResult();

#endif

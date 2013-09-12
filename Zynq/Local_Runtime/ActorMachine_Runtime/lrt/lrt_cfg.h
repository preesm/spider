
/*******************************************************************
*
* CAUTION: This file is automatically generated by libgen.
* Version: Xilinx EDK 14.2 EDK_P.28xd
* DO NOT EDIT.
*
* Copyright (c) 1995-2012 Xilinx, Inc.  All rights reserved.

* 
* Description: Local Runtime Parameters
*
*******************************************************************/

#ifndef OS_CFG_H
#define OS_CFG_H

#define OS_LOWEST_PRIO 				5
#define OS_MAX_TASKS 				1
#define NB_LOCAL_FUNCTIONS 			10
#define OS_DEBUG_EN 				1
#define CONTROL_COMM				0

#define SCHED_POLICY_RR				0
#define SCHED_POLICY_FP				0
#define ACTOR_MACHINE				1

#define OS_MAX_SH_MEM				1
#define OS_MAX_CTRL_Q				2


//#define AM_STATE_MAX_CONDITIONS	2
//#define AM_MAX_NB_EDGES			100

#ifdef ARM
#define AM_MAX_NB_VERTICES		300
#define AM_MAX_NB_ACTIONS		60
#define AM_MAX_NB_CONDITIONS	60
#else
#define AM_MAX_NB_VERTICES		200
#define AM_MAX_NB_CONDITIONS	50
#define AM_MAX_NB_ACTIONS		20
#endif

#define AM_MAX_NB_SUCCESSORS	2
#define OS_NB_FIFO		150

#ifdef ARM
#define MAX_NB_ARGS		OS_NB_FIFO
#define MAX_NB_FIFO		OS_NB_FIFO
#define WORKING_MEMORY_SIZE 720*400*3
#else
#define MAX_NB_ARGS		50
#define MAX_NB_FIFO		100
#define WORKING_MEMORY_SIZE 720*(400/8)
#endif

#endif

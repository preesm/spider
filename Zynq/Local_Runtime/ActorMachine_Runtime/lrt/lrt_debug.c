/*
 * lrt_debug.c
 *
 *  Created on: 23 janv. 2013
 *      Author: yoliva
 */

#include "lrt_prototypes.h"
#include <print.h>
#include <stdlib.h>

/**
 Exits returning a code representing the current error

 @param exitCode: code representing the current error
*/
void exitWithCode(int exitCode){
	// ExitCodes:
	// 1000: Try to create FIFO with ID >= OS_NB_FIFO
	// 1001: Try to get a FIFO handle with ID >= OS_NB_FIFO
	// 1002: Try to get a FIFO handle from an uninitialized handle
	// 1003: Try to create a Task with ID >= OS_MAX_TASKS
	// 1004: Try to create a Task with Nb Vertices    > AM_MAX_NB_VERTICES
	// 1005: Try to create a Task with Nb Conditions  > AM_MAX_NB_CONDITIONS
	// 1006: Try to create a Task with Nb Actions     > AM_MAX_NB_ACTIONS
	// 1007: Try to create a Task with InitState      > Nb Vertices
	// 1008: Try to create an Action with functionID  > NB_LOCAL_FUNCTIONS
	// 1009: Try to create an Action with Nb Fifo IN  > MAX_NB_FIFO
	// 1010: Try to create an Action with Nb Fifo OUT > MAX_NB_FIFO
	// 1011: Try to create an Action with Nb Args     > MAX_NB_ARGS
	// 1012: Try to create an Task on a already initialized ID
	// 1013: Try to delete an uninitialized task
	// 1014: Bad Monitor Action
	// 1015: Not enough working memory

	zynq_puts("\nExiting with code ");
	zynq_putdec(exitCode);
	zynq_puts("\n");
	exit(exitCode);
}

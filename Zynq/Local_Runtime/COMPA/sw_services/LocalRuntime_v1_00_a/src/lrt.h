/********************************************************************************
 * File    : lrt.h
 * By      : Yaset Oliva Venegas
 * Version : 1.0
 * Date	   : October 2012
 * Descrip : This file contains the interface of the Local Runtime and applications.
 *********************************************************************************/

#ifndef LRT
#define LRT

#include "lrt_definitions.h"


/**************** At lrt_core.c file ************************
 * Function : init_lrt
 * Params   :
 * Return   :
 * Descrip  : Does all the initializations and starts waiting for external messages.
 * 			This is called by the application's main program. The execution should never
 * 			come back to the application.
 *
 */
extern void init_lrt();

#endif

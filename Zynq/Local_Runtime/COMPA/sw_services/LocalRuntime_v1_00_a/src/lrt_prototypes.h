/********************************************************************************
 * File    : lrt_prototypes.h
 * By      : Yaset Oliva Venegas
 * Version : 1.0
 * Date	   : October 2012
 * Descrip : This file contains the prototypes of the RunTime's functions.
 * 			Prototypes should be MTAPI-compliant.
*********************************************************************************/

#ifndef RT_PROTOTYPES
#define RT_PROTOTYPES

//#include "mtapi/mtapi.h"
#include "lrt_definitions.h"





///* ************** At lrt_initialization.h file****************
// *
// * Function : mtapi_initialization
// * Params   :
// * Return   :
// * Descrip  : Initializes the MTAPI.
//*/
//extern void mtapi_initialization();







/* ************** At lrt_task_mngr.c file****************
 *
 * Function : mtapi_task_create
 * Params   : funct_addr	is pointer to the functional code.
 * Return   :
 * Descrip  : Creates an task.
*/
extern void mtapi_task_create(void* funct_addr);







/*
 * Function : mtapi_task_start
 * Params   : 
			  funct_addr	is the address of the functional code of the targeted task.
 * Descrip  : Starts a task.
*/





/*
 * Descrip	: Iterates on the OSTCBList until there is no more active tasks.
 */
extern void OS_Sched();





/*
 * Function : OSTaskCreateExt
 * Descrip  : Creates a task.
*/
INT8U  OSTaskCreateExt (FUNCTION_TYPE,
                        void*,
                        OS_STK*,
                        INT8U,
                        INT16U,
                        OS_STK*,
                        INT32U,
                        void*,
                        INT16U);





/* ************** At lrt_ext_msg_mngr.c file****************
 *
 * Function : wait_for_ext_msg
 * Params   :
			  addr (optional) is a memory address in case a shared memory is used instead of a FIFO.
 * Descrip  : This is the function of the idle task. It waits for a external message is placed into
* 			the FIFO or memory address.
*/
//extern void  wait_for_ext_msg(INT32U addr);
extern void  wait_for_ext_msg();






///*$PAGE*/
///*
//*********************************************************************************************************
//*                                              read_fsl_fifo (work in progress!!)
//*
//* Description: It reads data from the FSL fifo.
//*
//* Arguments  :
//*
//* Returns    : none
//*
//*********************************************************************************************************
//*/
//extern void  read_fsl_fifo();







///*$PAGE*/
///*
//*********************************************************************************************************
//*                                              write_fsl_fifo (work in progress!!)
//*
//* Description: It writes data into the FSL fifo.
//*
//* Arguments  :
//*
//* Returns    : none
//*
//*********************************************************************************************************
//*/
//extern void  write_fsl_fifo(INT32U);







/*$PAGE*/
/*
*********************************************************************************************************
*                                              get_sh_mem_hndl
*
* Description: Gets the pointer to the "i"th shared memory handle.
*
* Arguments  : sh_mem is a pointer to the structure that stores the shared memory's informations.
* 			   size is the amount of data to be read in bytes.
* 			   buffer is a pointer to a data block that will contain the read data.
*
* Returns    : perr will contain the error code : OS_ERR_NONE or OS_ERR_SH_MEM_NO_ENOUGH_DATA
*
*********************************************************************************************************
*/
OS_MEM* get_sh_mem_hndl(INT32U i);









/*$PAGE*/
/*
*********************************************************************************************************
*                                              read_sh_mem
*
* Description: Reads data (tokens) from a circular buffer on a shared memory.
*
* Arguments  : sh_mem is a pointer to the structure that stores the shared memory's informations.
* 			   size is the amount of data to be read in bytes.
* 			   buffer is a pointer to a data block that will contain the read data.
*
* Returns    : perr will contain the error code : OS_ERR_NONE or OS_ERR_SH_MEM_NO_ENOUGH_DATA
*
*********************************************************************************************************
*/
extern void  read_sh_mem(OS_MEM* sh_mem, INT32U size, INT8U* buffer, INT8U *perr);







/*$PAGE*/
/*
*********************************************************************************************************
*                                              write_sh_mem
*
* Description: Writes data (tokens) into a circular buffer on a shared memory.
*
* Arguments  : sh_mem is a pointer to the structure that stores the shared memory's informations.
* 			   size is the amount of data to be written in bytes.
* 			   buffer is a pointer to the data block to be copied.
*
* Returns    : perr will contain the error code : OS_ERR_NONE or OS_ERR_SH_MEM_NO_ENOUGH_ESPACE
*
*********************************************************************************************************
*/
extern void  write_sh_mem(OS_MEM* sh_mem, INT32U size, INT8U* buffer, INT8U *perr);










/* ************** At lrt_debug.c file****************
 *
 */

extern void print(char *ptr);


extern void putnum(unsigned int num);

#endif

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

#include <stdlib.h>
#include <platform_print.h>
#include "lrt_debug.h"

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
	// 1016: Data to be sent exceeds the MAX_DATA_WORDS.

	platform_puts("\nExiting with code ");
	platform_putdec(exitCode);
	platform_puts("\n");
	exit(exitCode);
}

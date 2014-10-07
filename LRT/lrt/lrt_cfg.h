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

#ifndef OS_CFG_H
#define OS_CFG_H

#define USE_AM						0

#define OS_LOWEST_PRIO 				15
#define OS_MAX_TASKS 				1
#define OS_MAX_TASKS_TIME			550*8
#define NB_LOCAL_FUNCTIONS 			15
#define OS_DEBUG_EN 				1
#define CONTROL_COMM				0

#define SCHED_POLICY_RR				0
#define SCHED_POLICY_FP				0
#define ACTOR_MACHINE				1


//#define AM_STATE_MAX_CONDITIONS	2
//#define AM_MAX_NB_EDGES			100

#define AM_MAX_NB_VERTICES		0 //200
#define AM_MAX_NB_CONDITIONS	0 //50
#define AM_MAX_NB_ACTIONS		0 //20

//#define AM_MAX_NB_SUCCESSORS	2
//#define OS_NB_FIFO		150

#define MAX_NB_FIFO			100//100
#define MAX_NB_PARAMETERS	100
#define WORKING_MEMORY_SIZE 64000//500*512*2//720*(400/8)

#endif

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

#include "testCom.h"
#include <platform.h>
#include <lrt.h>
#include <lrt_prototypes.h>
#include <stdio.h>
#include <stdlib.h>

void Generate(){
//	printf("Generate Started\n");

	UINT8 tab[MAX_TAB];
	UINT32 i;

	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	int tabSize = action->param_value[0];

	for(i=0; i<tabSize; i++){
		tab[i] = i;
	}

	write_output_fifo(action->fifo_out_id[0], tabSize, tab);

//	printf("Generate Finished\n");
}

void Transform1(){
//	printf("Transform1 Started\n");

	UINT8 tab[MAX_TAB];
	UINT32 i;

	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	int consoSize1 = action->param_value[0];

	read_input_fifo(action->fifo_in_id[0], consoSize1, tab);

	for(i=0; i<10; i++){
		tab[i] += 2;
	}

	write_output_fifo(action->fifo_out_id[0], consoSize1, tab);

//	printf("Transform1 Finished\n");
}

void Transform2(){
//	printf("Transform2 Started\n");

	UINT8 tab[MAX_TAB];
	UINT32 i;

	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	int consoSize2 = action->param_value[0];

	read_input_fifo(action->fifo_in_id[0], consoSize2, tab);

	for(i=0; i<10; i++){
		tab[i] -= 3;
	}

	write_output_fifo(action->fifo_out_id[0], consoSize2, tab);

//	printf("Transform2 Finished\n");
}

void Verify(){
//	printf("Verify Started\n");

	UINT8 tab[MAX_TAB];
	UINT32 i;

	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	int tabSize = action->param_value[0];

	read_input_fifo(action->fifo_in_id[0], tabSize, tab);

	for(i=0; i<tabSize; i++){
		if(tab[i] != (UINT8)(i-1))
			printf("Error tab[%d] = %d instead of %d\n", i, tab[i], (UINT8)(i-1));
	}

//	printf("Verify Finished\n");
}

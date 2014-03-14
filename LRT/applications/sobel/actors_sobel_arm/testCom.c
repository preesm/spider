/*
 * testCom.c
 *
 *  Created on: Jul 5, 2013
 *      Author: jheulot
 */

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

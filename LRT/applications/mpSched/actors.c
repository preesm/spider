
#include "actors.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NB_TAPS 256

void fir(float* in, float* out, int nb_sample){
	int i, j;
	float last[NB_TAPS];
	float taps[NB_TAPS];

	int last_id = 0;
	memset(last,0,NB_TAPS*sizeof(float));
	for(i=0; i<NB_TAPS; i++){
		taps[i] = 1.0/NB_TAPS;
	}

	for(i=0; i<nb_sample; i++){
		out[i] = 0;
		last[last_id] = in[i];
		for(j=0; j<NB_TAPS; j++){
			out[i] += taps[j]*last[(last_id+j)%NB_TAPS];
		}
		last_id = (last_id+1)%NB_TAPS;
	}

}

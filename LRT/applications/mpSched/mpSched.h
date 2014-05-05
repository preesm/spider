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

#include <string.h>
#include <platform_types.h>


#define NVAL	3
static UINT8 nValues[10] = {3,2,1};
#define MVALS	nValues

void config(UINT8* inputFIFOs[],
			UINT8* outputFIFOs[],
			UINT32 params[])
{
	UINT32 N;
	UINT32 NMAX = params[0];

	N = NVAL;

	UINT8* out_M = outputFIFOs[0];

	// Sending parameter's value.
	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, OSTCBCur->vertexId);
	platform_queue_push_UINT32(PlatformCtrlQueue, N);
	platform_queue_push_finalize(PlatformCtrlQueue);

	memcpy(out_M, MVALS, NMAX);
}


void mFilter(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 NMAX = params[0];
	UINT32 N = params[1];

	UINT8* in_m = inputFIFOs[0];
	UINT8* out_m = outputFIFOs[0];

	memcpy(out_m, in_m, N);
}


void src(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 i,j;
	UINT32 N = params[0];
	UINT32 NBSAMPLES = params[1];

	UINT8* out = outputFIFOs[0];

	for(i=0; i<NBSAMPLES; i++){
		for(j=0; j<N; j++){
			out[i+j*NBSAMPLES] = i;
		}
	}
}

void snk(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 i,j;
	UINT32 N = params[0];
	UINT32 NBSAMPLES = params[1];

	UINT8* in = inputFIFOs[0];

	BOOL test = TRUE;

	for(i=0; i<NBSAMPLES; i++){
		for(j=0; j<N; j++){
			UINT8 val = i+MVALS[j];
			if(in[i+j*NBSAMPLES] != val){
				printf("error at (%d,%d) : get %d instead of %d\n", i,j, in[i+j*NBSAMPLES], val);
				test = FALSE;
			}
		}
	}

	if(test){
		printf("Passed\n");
	}
}

void setM(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT8* in_m = inputFIFOs[0];

	// Sending parameter's value.
	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, OSTCBCur->vertexId);
	platform_queue_push_UINT32(PlatformCtrlQueue, in_m[0]);
	platform_queue_push_finalize(PlatformCtrlQueue);
}

void initSwitch(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 M = params[0];
	UINT32 i;

	UINT8* out = outputFIFOs[0];

	out[0] = 0;
	for(i=1; i<M; i++){
		out[i] = 1;
	}
}

void switchFct(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 M = params[1];
	UINT32 NBSAMPLES = params[0];
	UINT32 i;

	UINT8 select = inputFIFOs[0][0];
	UINT8 *in0 = inputFIFOs[1];
	UINT8 *in1 = inputFIFOs[2];
	UINT8 *out = outputFIFOs[0];

	if(select == 0)
		memcpy(out, in0, NBSAMPLES);
	else
		memcpy(out, in1, NBSAMPLES);
}


void FIR(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 NBSAMPLES = params[0];
	UINT32 i;

	UINT8* in = inputFIFOs[0];
	UINT8* out = outputFIFOs[0];

	for(i=0; i<NBSAMPLES; i++)
		out[i] = in[i]+1;
}



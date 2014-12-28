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

#include "Tests.h"

#include <cstdio>

#define VERBOSE 0

void test0_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	static int i=1;
#if VERBOSE
	printf("Execute C\n");
#endif
	outParams[0] = i++;
}

void test0_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
	out[1] = 2;
}

void test0_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	char* in = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B: ");
	for(int i=0;i<N; i++){
		printf("%d ", in[i]);
	}
	printf("\n");
#endif

	memcpy(out, in, N);
}

void test0_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	char* in = (char*)inputFIFOs[0];

	char expected[6] = {1,2,1,2,1,2};
	int nb;
	switch(N){
	case 1:
	case 2:
		nb = 2;
		break;
	case 3:
		nb = 6;
		break;
	}

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}

void test1_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	static int i=1;
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	out[0] = 1;
	out[1] = 2;
	out[2] = 3;
	outParams[0] = i++;
}

void test1_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	char* in = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	memcpy(out, in, N);
}

void test1_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	char* in = (char*)inputFIFOs[0];

	char expected[6] = {1,2,3,1};
	int nb;
	switch(N){
	case 1:
	case 3:
		nb = 3;
		break;
	case 2:
		nb = 4;
		break;
	}

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}


void test2_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	static int i=1;
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	out[0] = 1;
	outParams[0] = i++;
}

void test2_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	for(int i=0; i<N; i++)
		out[i] = i+1;
}

void test2_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in0 = (char*)inputFIFOs[0];
	char* in1 = (char*)inputFIFOs[1];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B\n");
#endif

	out[0] = in0[0] + in1[0];
}

void test2_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	char* in = (char*)inputFIFOs[0];

	char expected[2] = {2,3};
	int nb = N;

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}

void test3_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
	out[1] = 2;
	out[2] = 3;
	out[3] = 4;
}

void test3_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B\n");
#endif

	out[0] = in[0]+1;
}

void test3_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	out[0] = in[0]+2;
}

void test3_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in = (char*)inputFIFOs[0];

	char expected[4] = {2,4,4,6};
	int nb = 4;

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}


void test4_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
	out[1] = 2;
	out[2] = 3;
	out[3] = 4;
}

void test4_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B\n");
#endif

	out[0] = in[0]+1;
}

void test4_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	out[0] = in[0]+2;
}

void test4_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in = (char*)inputFIFOs[0];

	char expected[4] = {2,3,5,6};
	int nb = 4;

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}

void test5_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
}

void test5_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B\n");
#endif

	out[0] = in[0]+1;
}

void test5_H(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	for(int i=0; i<4; i++){
		out[i] = in[i]+1;
	}
}

void test5_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in = (char*)inputFIFOs[0];

	char expected[4] = {3,3};
	int nb = 2;

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}

void test6_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
	out[1] = 2;
	out[2] = 3;
	out[3] = 4;
}

void test6_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in = (char*)inputFIFOs[0];

	char expected[4] = {2,3,4,5};
	int nb = 4;

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}

void test6_H(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	out[0] = in[0]+1;
}


void test7_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
	out[1] = 2;
	out[2] = 3;
	out[3] = 4;
}

void test7_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B\n");
#endif

	out[0] = in[0]+2;
	out[1] = in[1]+2;
}

void test7_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	out[0] = in[0]+1;
}

void test7_H(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in0  = (char*)inputFIFOs[0];
	char* in1  = (char*)inputFIFOs[1];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute H\n");
#endif

	out[0] = in0[0] + in1[0];
}

void test7_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in = (char*)inputFIFOs[0];

	char expected[4] = {5,9,4,8};
	int nb = 4;

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}

void test8_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
}

void test8_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B\n");
#endif

	out[0] = in[0]+2;
}

void test8_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	out[0] = 2;
	out[1] = 3;
	out[2] = 4;
}

void test8_H(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in0  = (char*)inputFIFOs[0];
	char* in1  = (char*)inputFIFOs[1];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute H\n");
#endif

	out[0] = in0[0] + in1[0];
}

void test8_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in0 = (char*)inputFIFOs[0];
	char* in1 = (char*)inputFIFOs[0];

	printf("Test: ");
	if(		in0[0] == 3
		&& 	in1[0] == 5){
		printf("FAILED\n");
		return;
	}
	printf("PASSED\n");
}

lrtFct test_fcts[NB_FCT_TEST] = {
		&test0_A,
		&test0_B,
		&test0_C,
		&test0_Check,

		&test1_A,
		&test1_C,
		&test1_Check,

		&test2_A,
		&test2_B,
		&test2_C,
		&test2_Check,

		&test3_A,
		&test3_B,
		&test3_C,
		&test3_Check,

		&test4_A,
		&test4_B,
		&test4_C,
		&test4_Check,

		&test5_A,
		&test5_B,
		&test5_H,
		&test5_Check,

		&test6_A,
		&test6_H,
		&test6_Check,

		&test7_A,
		&test7_B,
		&test7_C,
		&test7_H,
		&test7_Check,

		&test8_A,
		&test8_B,
		&test8_C,
		&test8_H,
		&test8_Check};


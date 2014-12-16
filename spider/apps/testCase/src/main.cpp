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

#include <spider.h>
#include <platformLinux.h>
#include "Tests.h"

#include <cstdio>
#include <cstdlib>

#define STACK_SIZE (6*1024*1024)
#define ARCHI_STACK_SIZE (1024)

int main(int argc, char* argv[]){
	PlatformLinux platform;
	PiSDFGraph *topPisdf;
	SpiderConfig cfg;
	SRDAGGraph srdag;

	void* memory = malloc(STACK_SIZE);
	StaticStack stack = StaticStack(memory,STACK_SIZE);

	void* archiMem = malloc(ARCHI_STACK_SIZE);
	StaticStack archiStack = StaticStack(archiMem,ARCHI_STACK_SIZE);

	platform.init(1, &archiStack);

	SharedMemArchi archi = SharedMemArchi(&archiStack,
			/* Nb PE */		1,
			/* Nb PE Type*/ 1);

	DummyMemAlloc memAlloc = DummyMemAlloc(1000, 1000);
	ListScheduler scheduler = ListScheduler();

	archi.setPETypeRecvSpeed(0, 1, 10);
	archi.setPETypeSendSpeed(0, 1, 10);
	archi.setPEType(0, 0);
	archi.setName(0, "PE0");

	cfg.createSrdag = false;
	cfg.srdag = &srdag;
	cfg.memAlloc = &memAlloc;
	cfg.scheduler = &scheduler;

	printf("Start\n");

//	try{
		for(int i=1; i<=3; i++){
			char name[20];
			sprintf(name, "test0_%d.gv", i);
			stack.freeAll();
			srdag = SRDAGGraph(&stack);
			getLrt()->setFctTbl(test0_fcts, 3);
			topPisdf = initPisdf_test0(&archi, &stack, i);
			jit_ms(topPisdf, &archi, &cfg);
			test_Test0(topPisdf, &srdag, i, &stack);
			srdag.print("test0.gv");
		}

		for(int i=1; i<=2; i++){
			char name[20];
			sprintf(name, "test1_%d.gv", i);
			stack.free();
			srdag = SRDAGGraph(&stack);
			topPisdf = initPisdf_test1(&archi, &stack, i);
			jit_ms(topPisdf, &archi, &cfg);
			test_Test1(topPisdf, &srdag, i, &stack);
		}

		for(int i=1; i<=2; i++){
			char name[20];
			sprintf(name, "test2_%d.gv", i);
			stack.free();
			srdag = SRDAGGraph(&stack);
			topPisdf = initPisdf_test2(&archi, &stack, i);
			jit_ms(topPisdf, &archi, &cfg);
			test_Test2(topPisdf, &srdag, i, &stack);
		}

		stack.free();
		srdag = SRDAGGraph(&stack);
		topPisdf = initPisdf_test3(&archi, &stack);
		jit_ms(topPisdf, &archi, &cfg);
		test_Test3(topPisdf, &srdag, &stack);

		stack.free();
		srdag = SRDAGGraph(&stack);
		topPisdf = initPisdf_test4(&archi, &stack);
		jit_ms(topPisdf, &archi, &cfg);
		test_Test4(topPisdf, &srdag, &stack);

		stack.free();
		srdag = SRDAGGraph(&stack);
		topPisdf = initPisdf_test5(&archi, &stack);
		jit_ms(topPisdf, &archi, &cfg);
		test_Test5(topPisdf, &srdag, &stack);

		stack.free();
		srdag = SRDAGGraph(&stack);
		topPisdf = initPisdf_test6(&archi, &stack);
		jit_ms(topPisdf, &archi, &cfg);
		test_Test6(topPisdf, &srdag, &stack);

		stack.free();
		srdag = SRDAGGraph(&stack);
		topPisdf = initPisdf_test7(&archi, &stack);
		jit_ms(topPisdf, &archi, &cfg);
		test_Test7(topPisdf, &srdag, &stack);

		stack.free();
		srdag = SRDAGGraph(&stack);
		topPisdf = initPisdf_test8(&archi, &stack);
		jit_ms(topPisdf, &archi, &cfg);
		test_Test8(topPisdf, &srdag, &stack);

		stack.free();
		srdag = SRDAGGraph(&stack);
		topPisdf = initPisdf_test9(&archi, &stack);
		jit_ms(topPisdf, &archi, &cfg);
		test_Test9(topPisdf, &srdag, &stack);

		stack.free();
		srdag = SRDAGGraph(&stack);
		topPisdf = initPisdf_testA(&archi, &stack);
		jit_ms(topPisdf, &archi, &cfg);
		test_TestA(topPisdf, &srdag, &stack);
//
//	}catch(const char* s){
//		printf("Exception : %s\n", s);
//	}
	printf("finished\n");

	stack.printStat();
	archiStack.printStat();

	free(memory);

	return 0;
}

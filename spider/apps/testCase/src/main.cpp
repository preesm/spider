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

#define STACK_SIZE (600*1024)
#define ARCHI_STACK_SIZE (1024)

int main(int argc, char* argv[]){
	SpiderConfig cfg;

	DynStack srdagStack("SrdagStack");
	DynStack pisdfStack("PisdfStack");
	DynStack testStack("TestStack");
	DynStack archiStack("ArchiStack");
	DynStack transfoStack("TransfoStack");

	PlatformLinux platform(1, &archiStack);

	SharedMemArchi archi(
			/* Stack */  	&archiStack,
			/* Nb PE */		1,
			/* Nb PE Type*/ 1);

	DummyMemAlloc memAlloc = DummyMemAlloc(1000, 1000);
	ListScheduler scheduler = ListScheduler();

	archi.setPETypeRecvSpeed(0, 1, 10);
	archi.setPETypeSendSpeed(0, 1, 10);
	archi.setPEType(0, 0);
	archi.setName(0, "PE0");

	printf("Start\n");

//	try{
		for(int i=1; i<=3; i++){
			srdagStack.freeAll();
			pisdfStack.freeAll();
			testStack.freeAll();
			memAlloc.reset();

			SRDAGGraph srdag(&srdagStack);

			cfg.createSrdag = false;
			cfg.srdag = &srdag;
			cfg.memAlloc = &memAlloc;
			cfg.scheduler = &scheduler;
			cfg.transfoStack = &transfoStack;
			getLrt()->setFctTbl(test0_fcts, 4);

			PiSDFGraph *topPisdf = initPisdf_test0(&archi, &pisdfStack, i);
			topPisdf->print("pi.gv");

			jit_ms(topPisdf, &archi, &cfg);

			test_Test0(topPisdf, &srdag, i, &testStack);

			topPisdf->~PiSDFGraph();
			pisdfStack.free(topPisdf);
		}

		for(int i=1; i<=3; i++){
			srdagStack.freeAll();
			pisdfStack.freeAll();
			testStack.freeAll();
			memAlloc.reset();

			SRDAGGraph srdag(&srdagStack);

			cfg.createSrdag = false;
			cfg.srdag = &srdag;
			cfg.memAlloc = &memAlloc;
			cfg.scheduler = &scheduler;
			cfg.transfoStack = &transfoStack;
			getLrt()->setFctTbl(test1_fcts, NB_FCT_TEST1);

			PiSDFGraph *topPisdf = initPisdf_test1(&archi, &pisdfStack, i);
			topPisdf->print("pi.gv");

			jit_ms(topPisdf, &archi, &cfg);

			test_Test1(topPisdf, &srdag, i, &testStack);

			topPisdf->~PiSDFGraph();
			pisdfStack.free(topPisdf);
		}

		for(int i=1; i<=2; i++){
			srdagStack.freeAll();
			pisdfStack.freeAll();
			testStack.freeAll();
			memAlloc.reset();

			SRDAGGraph srdag(&srdagStack);

			cfg.createSrdag = false;
			cfg.srdag = &srdag;
			cfg.memAlloc = &memAlloc;
			cfg.scheduler = &scheduler;
			cfg.transfoStack = &transfoStack;
			getLrt()->setFctTbl(test2_fcts, NB_FCT_TEST2);

			PiSDFGraph *topPisdf = initPisdf_test2(&archi, &pisdfStack, i);
			topPisdf->print("pi.gv");

			jit_ms(topPisdf, &archi, &cfg);

			test_Test2(topPisdf, &srdag, i, &testStack);

			topPisdf->~PiSDFGraph();
			pisdfStack.free(topPisdf);
		}

	{
		srdagStack.freeAll();
		pisdfStack.freeAll();
		testStack.freeAll();
		memAlloc.reset();

		SRDAGGraph srdag(&srdagStack);

		cfg.createSrdag = false;
		cfg.srdag = &srdag;
		cfg.memAlloc = &memAlloc;
		cfg.scheduler = &scheduler;
		cfg.transfoStack = &transfoStack;
		getLrt()->setFctTbl(test3_fcts, NB_FCT_TEST3);

		PiSDFGraph *topPisdf = initPisdf_test3(&archi, &pisdfStack);
		topPisdf->print("pi.gv");

		jit_ms(topPisdf, &archi, &cfg);

		test_Test3(topPisdf, &srdag, &testStack);

		topPisdf->~PiSDFGraph();
		pisdfStack.free(topPisdf);
	}

	{
		srdagStack.freeAll();
		pisdfStack.freeAll();
		testStack.freeAll();
		memAlloc.reset();

		SRDAGGraph srdag(&srdagStack);

		cfg.createSrdag = false;
		cfg.srdag = &srdag;
		cfg.memAlloc = &memAlloc;
		cfg.scheduler = &scheduler;
		cfg.transfoStack = &transfoStack;
		getLrt()->setFctTbl(test4_fcts, NB_FCT_TEST4);

		PiSDFGraph *topPisdf = initPisdf_test4(&archi, &pisdfStack);
		topPisdf->print("pi.gv");

		jit_ms(topPisdf, &archi, &cfg);

		test_Test4(topPisdf, &srdag, &testStack);

		topPisdf->~PiSDFGraph();
		pisdfStack.free(topPisdf);
	}

	{
		srdagStack.freeAll();
		pisdfStack.freeAll();
		testStack.freeAll();
		memAlloc.reset();

		SRDAGGraph srdag(&srdagStack);

		cfg.createSrdag = false;
		cfg.srdag = &srdag;
		cfg.memAlloc = &memAlloc;
		cfg.scheduler = &scheduler;
		cfg.transfoStack = &transfoStack;
		getLrt()->setFctTbl(test5_fcts, NB_FCT_TEST5);

		PiSDFGraph *topPisdf = initPisdf_test5(&archi, &pisdfStack);
		topPisdf->print("pi.gv");

		jit_ms(topPisdf, &archi, &cfg);

		test_Test5(topPisdf, &srdag, &testStack);

		topPisdf->~PiSDFGraph();
		pisdfStack.free(topPisdf);
	}

	{
		srdagStack.freeAll();
		pisdfStack.freeAll();
		testStack.freeAll();
		memAlloc.reset();

		SRDAGGraph srdag(&srdagStack);

		cfg.createSrdag = false;
		cfg.srdag = &srdag;
		cfg.memAlloc = &memAlloc;
		cfg.scheduler = &scheduler;
		cfg.transfoStack = &transfoStack;
		getLrt()->setFctTbl(test6_fcts, NB_FCT_TEST6);

		PiSDFGraph *topPisdf = initPisdf_test6(&archi, &pisdfStack);
		topPisdf->print("pi.gv");

		jit_ms(topPisdf, &archi, &cfg);

		test_Test6(topPisdf, &srdag, &testStack);

		topPisdf->~PiSDFGraph();
		pisdfStack.free(topPisdf);
	}

	{
		srdagStack.freeAll();
		pisdfStack.freeAll();
		testStack.freeAll();
		memAlloc.reset();

		SRDAGGraph srdag(&srdagStack);

		cfg.createSrdag = false;
		cfg.srdag = &srdag;
		cfg.memAlloc = &memAlloc;
		cfg.scheduler = &scheduler;
		cfg.transfoStack = &transfoStack;
		getLrt()->setFctTbl(test7_fcts, NB_FCT_TEST7);

		PiSDFGraph *topPisdf = initPisdf_test7(&archi, &pisdfStack);
		topPisdf->print("pi.gv");

		jit_ms(topPisdf, &archi, &cfg);

		test_Test7(topPisdf, &srdag, &testStack);

		topPisdf->~PiSDFGraph();
		pisdfStack.free(topPisdf);
	}

	{
		srdagStack.freeAll();
		pisdfStack.freeAll();
		testStack.freeAll();
		memAlloc.reset();

		SRDAGGraph srdag(&srdagStack);

		cfg.createSrdag = false;
		cfg.srdag = &srdag;
		cfg.memAlloc = &memAlloc;
		cfg.scheduler = &scheduler;
		cfg.transfoStack = &transfoStack;
		getLrt()->setFctTbl(test8_fcts, NB_FCT_TEST8);

		PiSDFGraph *topPisdf = initPisdf_test8(&archi, &pisdfStack);
		topPisdf->print("pi.gv");

		jit_ms(topPisdf, &archi, &cfg);

		test_Test8(topPisdf, &srdag, &testStack);

		topPisdf->~PiSDFGraph();
		pisdfStack.free(topPisdf);
	}

//	}catch(const char* s){
//		printf("Exception : %s\n", s);
//	}
	printf("finished\n");

	return 0;
}

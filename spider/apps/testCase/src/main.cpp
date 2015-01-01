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

int main(int argc, char* argv[]){
	SpiderConfig cfg;
	ExecutionStat stat;

	DynStack pisdfStack("PisdfStack");
	DynStack archiStack("ArchiStack");
	DynStack testStack("TestStack");

	PlatformLinux platform(2, &archiStack, test_fcts, NB_FCT_TEST);
	Archi* archi = platform.getArchi();

	cfg.memAllocType = MEMALLOC_DUMMY;
	cfg.memAllocStart = (void*)1000;
	cfg.memAllocSize = 1000;

	cfg.schedulerType = SCHEDULER_LIST;

	cfg.srdagStack = {STACK_DYNAMIC, "SrdagStack", 0, 0};
	cfg.transfoStack = {STACK_DYNAMIC, "TransfoStack", 0, 0};

	spider_init(cfg);

	printf("Start\n");

//	try{
		for(int i=1; i<=3; i++){
			char path[10];
			sprintf(path, "test0_%d.xml", i);

			pisdfStack.freeAll();
			testStack.freeAll();

			PiSDFGraph *topPisdf = initPisdf_test0(archi, &pisdfStack, i);
			topPisdf->print("pi.gv");

			Platform::get()->rstTime();

			spider_launch(archi, topPisdf);

			test_Test0(topPisdf, spider_getLastSRDAG(), i, &testStack);

			freePisdf_test0(topPisdf, &pisdfStack);

			spider_printGantt(archi, spider_getLastSRDAG(), path, "latex.tex", &stat);
		}

		for(int i=1; i<=3; i++){
			pisdfStack.freeAll();
			testStack.freeAll();

			PiSDFGraph *topPisdf = initPisdf_test1(archi, &pisdfStack, i);
			topPisdf->print("pi.gv");

			spider_launch(archi, topPisdf);

			test_Test1(topPisdf, spider_getLastSRDAG(), i, &testStack);

			freePisdf_test1(topPisdf, &pisdfStack);
		}

		for(int i=1; i<=2; i++){
			pisdfStack.freeAll();
			testStack.freeAll();

			PiSDFGraph *topPisdf = initPisdf_test2(archi, &pisdfStack, i);
			topPisdf->print("pi.gv");

			spider_launch(archi, topPisdf);

			test_Test2(topPisdf, spider_getLastSRDAG(), i, &testStack);

			freePisdf_test2(topPisdf, &pisdfStack);
		}

	{
		pisdfStack.freeAll();
		testStack.freeAll();

		PiSDFGraph *topPisdf = initPisdf_test3(archi, &pisdfStack);
		topPisdf->print("pi.gv");

		spider_launch(archi, topPisdf);

		test_Test3(topPisdf, spider_getLastSRDAG(), &testStack);

		freePisdf_test3(topPisdf, &pisdfStack);
	}

	{
		pisdfStack.freeAll();
		testStack.freeAll();

		PiSDFGraph *topPisdf = initPisdf_test4(archi, &pisdfStack);
		topPisdf->print("pi.gv");

		spider_launch(archi, topPisdf);

		test_Test4(topPisdf, spider_getLastSRDAG(), &testStack);

		freePisdf_test4(topPisdf, &pisdfStack);
	}

	{
		pisdfStack.freeAll();
		testStack.freeAll();

		PiSDFGraph *topPisdf = initPisdf_test5(archi, &pisdfStack);
		topPisdf->print("pi.gv");

		spider_launch(archi, topPisdf);

		test_Test5(topPisdf, spider_getLastSRDAG(), &testStack);

		freePisdf_test5(topPisdf, &pisdfStack);
	}

	{
		pisdfStack.freeAll();
		testStack.freeAll();

		PiSDFGraph *topPisdf = initPisdf_test6(archi, &pisdfStack);
		topPisdf->print("pi.gv");

		spider_launch(archi, topPisdf);

		test_Test6(topPisdf, spider_getLastSRDAG(), &testStack);

		freePisdf_test6(topPisdf, &pisdfStack);
	}

	{
		pisdfStack.freeAll();
		testStack.freeAll();

		PiSDFGraph *topPisdf = initPisdf_test7(archi, &pisdfStack);
		topPisdf->print("pi.gv");

		spider_launch(archi, topPisdf);

		test_Test7(topPisdf, spider_getLastSRDAG(), &testStack);

		freePisdf_test7(topPisdf, &pisdfStack);
	}

//	}catch(const char* s){
//		printf("Exception : %s\n", s);
//	}
	printf("finished\n");

	spider_free();

	return 0;
}

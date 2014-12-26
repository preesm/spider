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

#ifndef TESTS_H
#define TESTS_H

#include <spider.h>

PiSDFGraph* initPisdf_test0(Archi* archi, Stack* stack, int N);
PiSDFGraph* initPisdf_test1(Archi* archi, Stack* stack, int N);
PiSDFGraph* initPisdf_test2(Archi* archi, Stack* stack, int N);
PiSDFGraph* initPisdf_test3(Archi* archi, Stack* stack);
PiSDFGraph* initPisdf_test4(Archi* archi, Stack* stack);
PiSDFGraph* initPisdf_test5(Archi* archi, Stack* stack);
PiSDFGraph* initPisdf_test6(Archi* archi, Stack* stack);
PiSDFGraph* initPisdf_test7(Archi* archi, Stack* stack);
PiSDFGraph* initPisdf_test8(Archi* archi, Stack* stack);

void test_Test0(PiSDFGraph* pisdf, SRDAGGraph* srdag, int N, Stack* stack);
void test_Test1(PiSDFGraph* pisdf, SRDAGGraph* srdag, int N, Stack* stack);
void test_Test2(PiSDFGraph* pisdf, SRDAGGraph* srdag, int N, Stack* stack);
void test_Test3(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack);
void test_Test4(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack);
void test_Test5(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack);
void test_Test6(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack);
void test_Test7(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack);
void test_Test8(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack);

#define NB_FCT_TEST0 4
#define NB_FCT_TEST1 3
#define NB_FCT_TEST2 4
#define NB_FCT_TEST3 4
#define NB_FCT_TEST4 4
#define NB_FCT_TEST5 4
#define NB_FCT_TEST6 3
#define NB_FCT_TEST7 5
#define NB_FCT_TEST8 6

extern lrtFct test0_fcts[NB_FCT_TEST0];
extern lrtFct test1_fcts[NB_FCT_TEST1];
extern lrtFct test2_fcts[NB_FCT_TEST2];
extern lrtFct test3_fcts[NB_FCT_TEST3];
extern lrtFct test4_fcts[NB_FCT_TEST4];
extern lrtFct test5_fcts[NB_FCT_TEST5];
extern lrtFct test6_fcts[NB_FCT_TEST6];
extern lrtFct test7_fcts[NB_FCT_TEST7];
extern lrtFct test8_fcts[NB_FCT_TEST8];

#endif//TESTS_H

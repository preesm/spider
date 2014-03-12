/*
 * testAM.h
 *
 *  Created on: Jun 19, 2013
 *      Author: jheulot
 */

#ifndef TESTAM_H_
#define TESTAM_H_

#include "../scheduling/architecture/Architecture.h"
#include "../graphs/CSDAG/CSDAGGraph.h"

void createTestCom(CSDAGGraph* graph, Architecture* archi);
void createZynqArchi(Architecture* archi, int nbUBlazes);
void createSobel(CSDAGGraph* graph, Architecture* archi);


#endif /* TESTAM_H_ */

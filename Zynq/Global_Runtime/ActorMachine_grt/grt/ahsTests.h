/*
 * cmpFrontTests.h
 *
 *  Created on: Jan 8, 2013
 *      Author: jheulot
 */

#ifndef CMPFRONTTESTS_H
#define CMPFRONTTESTS_H

#include "graphs/CSDAG/CSDAGGraph.h"
#include "scheduling/architecture/Architecture.h"
#include "ExecutionStat.h"

/**
 Scheduling a LTE graph on an architecture with nbSlaves cores

 @param scheduler: the scheduler object
 @param csGraph: the reference CSDAG graph containing timings and constraints
 @param archi: the target architecture
 @param nbSlaves: Nb of cores
*/

ExecutionStat test100LTE(int nbSlaves);

ExecutionStat testLTE(int nbSlaves);

ExecutionStat testSimpleLTE();
ExecutionStat testMaxLTE();

int testSDF3( int nbSlaves);

#endif /* CMPFRONTTESTS_H */

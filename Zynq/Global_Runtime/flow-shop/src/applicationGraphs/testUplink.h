/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef TEST_UPLINK_H
#define TEST_UPLINK_H

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "../scheduling/architecture/Architecture.h"
#include "../graphs/CSDAG/CSDAGGraph.h"

/**
 * Testing uplink graph
 * 
 * @author mpelcat
 */

/**
 Scheduling the max uplink case on an architecture with 2 cores of a Faraday
 and a decreasing number of Tomahawk cores

 @param scheduler: the scheduler object
 @param csGraph: the reference CSDAG graph containing timings and constraints
 @param archi: the target architecture
*/
//void testArchis(TransformerScheduler* scheduler, CSDAGGraph* csDag, SRDAGGraph* srDag, Architecture* archi);

/**
 Scheduling random graphs on an architecture with 2 cores of a Faraday and a Tomahawk

 @param scheduler: the scheduler object
 @param csGraph: the reference CSDAG graph containing timings and constraints
 @param archi: the target architecture
*/
//void testRandomGraphs(TransformerScheduler* scheduler, CSDAGGraph* csDag, Architecture* archi);

/**
 Scheduling one fixed graph

 @param scheduler: the scheduler object
 @param csGraph: the reference CSDAG graph containing timings and constraints
 @param archi: the target architecture
*/
//void testFixedGraph(TransformerScheduler* scheduler, CSDAGGraph* csDag, Architecture* archi);

/**
 Scheduling graphs with increasing size on an architecture with 2 cores of a Faraday and a Tomahawk

 @param scheduler: the scheduler object
 @param csGraph: the reference CSDAG graph containing timings and constraints
 @param archi: the target architecture
*/
//void testGraphSize(TransformerScheduler* scheduler, CSDAGGraph* csDag, Architecture* archi);

// nbSlaves cores with full mesh homogeneous interconnection
void createSimpleArchi(Architecture* archi, int nbSlaves);

// Uplink parameterized with 100 users max
void createGraphUplink100Max(CSDAGGraph* graph, Architecture* archi);

// Uplink parameterized with a custom max number of UEs and RBs
void setRandomParamsUplinkCustom();

// Setting parameters for 100 CBs and 100 UEs
void set100CB_100UE();

void createGraphUplinkSimpleTest(CSDAGGraph* graph, Architecture* archi);
void createGraphUplinkPerfTest(CSDAGGraph* graph, Architecture* archi);
void createGraphUplinkMaxTest(CSDAGGraph* graph, Architecture* archi);

#endif

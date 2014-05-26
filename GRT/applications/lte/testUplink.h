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

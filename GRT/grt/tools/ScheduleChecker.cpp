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

#include "ScheduleChecker.h"

#include "../scheduling/architecture/Architecture.h"
#include "../graphs/SRDAG/SRDAGGraph.h"

ScheduleChecker::ScheduleChecker() {
}

ScheduleChecker::~ScheduleChecker() {
}

inline int testConcurency(SRDAGGraph* srDag, Schedule* schedule, Architecture* archi){
	int test=1;
//	// Check core concurrency
//
//	for(int slave=0; slave<archi->getNbSlaves() && test; slave++){
//		for(int i=0; i<schedule->getNbVertex(slave)-1 && test; i++){
//			SRDAGVertex* vertex = schedule->getVertex(slave, i);
//			SRDAGVertex* nextVertex = schedule->getVertex(slave, i+1);
//			int vertexID = srDag->getVertexIndex(vertex);
//			int nextVertexID = srDag->getVertexIndex(nextVertex);
//
//			if(schedule->getVertexEndTime(vertexID)>schedule->getVertexEndTime(nextVertexID)){
//				test = 0;
//				printf("Superposition: task %s%d and %s%d\n",
//						vertex->getCsDagReference()->getName(),
//						vertex->getReferenceIndex(),
//						nextVertex->getCsDagReference()->getName(),
//						nextVertex->getReferenceIndex());
//			}
//		}
//	}

	return test;
}

inline int testCommunication(SRDAGGraph* srDag, Schedule* schedule, Architecture* archi){
	int test=1;
//	// Check Graph Dependency
//	for (int i=0 ; i<srDag->getNbVertices() && test; i++){
//		SRDAGVertex* vertex = srDag->getVertex(i);
//		int vertexID = srDag->getVertexIndex(vertex);
//
//		for (int j=0 ; j<vertex->getNbInputEdge() && test; j++){
//			SRDAGVertex* precVertex = vertex->getInputEdge(j)->getSource();
//			int precVertexID = srDag->getVertexIndex(precVertex);
//
//			if(schedule->getVertexEndTime(vertexID) >= schedule->getVertexEndTime(precVertexID))
//				test=1;
//			else{
//				test=0;
//				printf("Communication: task %s%d -> %s%d\n",
//						vertex->getCsDagReference()->getName(),
//						vertex->getReferenceIndex(),
//						precVertex->getCsDagReference()->getName(),
//						precVertex->getReferenceIndex());
//			}
//		}
//	}
	return test;
}

inline int testConcurency(SRDAGGraph* srDag, Architecture* archi){
	int test=1;
//	// Check core concurency
//	for (int i=0 ; i<srDag->getNbVertices() && test; i++){
//		for (int j=0 ; j<srDag->getNbVertices() && test; j++){
//			SRDAGVertex* vertexI = srDag->getVertex(i);
//			SRDAGVertex* vertexJ = srDag->getVertex(j);
//
//			if(		vertexI->getSlaveIndex() < 0 ||
//					vertexJ->getSlaveIndex() < 0 ||
//					vertexJ->getSlaveIndex() >= archi->getNbSlaves() ||
//					vertexJ->getSlaveIndex() >= archi->getNbSlaves() ){
//				test = 0;
//				printf("Using non-identified Slave: %d or %d\n ", vertexI->getSlaveIndex(), vertexJ->getSlaveIndex());
//			}
//
//			if(j!=i && (vertexI->getSlaveIndex() == vertexJ->getSlaveIndex())){
//				unsigned int startI = vertexI->getTLevel();
//				unsigned int endI = startI + vertexI->getCsDagReference()->getIntTiming(0);
//				unsigned int startJ = vertexJ->getTLevel();
//				unsigned int endJ = startJ + vertexJ->getCsDagReference()->getIntTiming(0);
//
//				if((startI <= startJ && endI > startJ) ||
//						(startJ <= startI && endJ > startI)){
//					test = 0;
//					printf("Superposition: task %s%d and %s%d\n",
//							vertexI->getCsDagReference()->getName(),
//							vertexI->getReferenceIndex(),
//							vertexJ->getCsDagReference()->getName(),
//							vertexJ->getReferenceIndex());
//				}
//			}
//		}
//	}
	return test;
}

inline int testCommunication(SRDAGGraph* srDag, Architecture* archi){
	int test=1;
//	// Check Graph Dependency
//	for (int i=0 ; i<srDag->getNbVertices() && test; i++){
//		SRDAGVertex* vertexI = srDag->getVertex(i);
//		unsigned int startI = vertexI->getTLevel();
//
//		for (int j=0 ; j<vertexI->getNbInputEdge() && test; j++){
//			SRDAGVertex* vertexJ = vertexI->getInputEdge(j)->getSource();
//			unsigned int endJ = vertexJ->getTLevel() + vertexJ->getCsDagReference()->getIntTiming(0);
//
//			if(startI >= endJ) test=1;
//			else{
//				test=0;
//				printf("Communication: task %s%d -> %s%d\n",
//						vertexJ->getCsDagReference()->getName(),
//						vertexJ->getReferenceIndex(),
//						vertexI->getCsDagReference()->getName(),
//						vertexI->getReferenceIndex());
//			}
//		}
//	}
	return test;
}

void ScheduleChecker::checkSchedule(SRDAGGraph* srDag, Schedule* schedule, Architecture* archi){
	int ret=1;
#ifdef DISPLAY
	printf("Test Schedule:\n");
#endif
	ret = testConcurency(srDag, schedule, archi);
#ifdef DISPLAY
	printf("\tCore Conc.: %s\n", (ret)?("Ok"):("Failed"));
#endif
	if(!ret) exitWithCode(1049);
	ret = testCommunication(srDag, schedule, archi);
#ifdef DISPLAY
	printf("\tCommunic. : %s\n", (ret)?("Ok"):("Failed"));
#endif
	if(!ret) exitWithCode(1049);
}

void ScheduleChecker::checkList(SRDAGGraph* srDag, Architecture* archi){
	int ret=1;
#ifdef DISPLAY
	printf("Test List Schedule:\n");
#endif
	ret = testConcurency(srDag, archi);
#ifdef DISPLAY
	printf("\tCore Conc.: %s\n", (ret)?("Ok"):("Failed"));
#endif
	if(!ret) exitWithCode(1049);
	ret = testCommunication(srDag, archi);
#ifdef DISPLAY
	printf("\tCommunic. : %s\n", (ret)?("Ok"):("Failed"));
#endif
	if(!ret) exitWithCode(1049);
}


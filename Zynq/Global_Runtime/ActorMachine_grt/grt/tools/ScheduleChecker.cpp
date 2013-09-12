/*
 * ScheduleChecker.cpp
 *
 *  Created on: Jan 24, 2013
 *      Author: jheulot
 */

#include "ScheduleChecker.h"

#include "../scheduling/architecture/Architecture.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "../graphs/CSDAG/CSDAGGraph.h"
#include "../graphs/JobSet/JobSet.h"

ScheduleChecker::ScheduleChecker() {
}

ScheduleChecker::~ScheduleChecker() {
}

inline int testConcurency(SRDAGGraph* srDag, Schedule* schedule, Architecture* archi){
	int test=1;
	// Check core concurency

	for(int slave=0; slave<archi->getNbSlaves() && test; slave++){
		for(int i=0; i<schedule->getNbVertex(slave)-1 && test; i++){
			SRDAGVertex* vertex = schedule->getVertex(slave, i);
			SRDAGVertex* nextVertex = schedule->getVertex(slave, i+1);
			int vertexID = srDag->getVertexIndex(vertex);
			int nextVertexID = srDag->getVertexIndex(nextVertex);

			if(schedule->getVertexEndTime(vertexID)>schedule->getVertexEndTime(nextVertexID)){
				test = 0;
				printf("Superposition: task %s%d and %s%d\n",
						vertex->getCsDagReference()->getName(),
						vertex->getReferenceIndex(),
						nextVertex->getCsDagReference()->getName(),
						nextVertex->getReferenceIndex());
			}
		}
	}

	return test;
}

inline int testCommunication(SRDAGGraph* srDag, Schedule* schedule, Architecture* archi){
	int test=1;
	// Check Graph Dependency
	for (int i=0 ; i<srDag->getNbVertices() && test; i++){
		SRDAGVertex* vertex = srDag->getVertex(i);
		int vertexID = srDag->getVertexIndex(vertex);

		for (int j=0 ; j<vertex->getNbInputEdge() && test; j++){
			SRDAGVertex* precVertex = vertex->getInputEdge(j)->getSource();
			int precVertexID = srDag->getVertexIndex(precVertex);

			if(schedule->getVertexEndTime(vertexID) >= schedule->getVertexEndTime(precVertexID))
				test=1;
			else{
				test=0;
				printf("Communication: task %s%d -> %s%d\n",
						vertex->getCsDagReference()->getName(),
						vertex->getReferenceIndex(),
						precVertex->getCsDagReference()->getName(),
						precVertex->getReferenceIndex());
			}
		}
	}
	return test;
}

inline int testConcurency(SRDAGGraph* srDag, Architecture* archi){
	int test=1;
	// Check core concurency
	for (int i=0 ; i<srDag->getNbVertices() && test; i++){
		for (int j=0 ; j<srDag->getNbVertices() && test; j++){
			SRDAGVertex* vertexI = srDag->getVertex(i);
			SRDAGVertex* vertexJ = srDag->getVertex(j);

			if(		vertexI->getSlaveIndex() < 0 ||
					vertexJ->getSlaveIndex() < 0 ||
					vertexJ->getSlaveIndex() >= archi->getNbSlaves() ||
					vertexJ->getSlaveIndex() >= archi->getNbSlaves() ){
				test = 0;
				printf("Using non-identified Slave: %d or %d\n ", vertexI->getSlaveIndex(), vertexJ->getSlaveIndex());
			}

			if(j!=i && (vertexI->getSlaveIndex() == vertexJ->getSlaveIndex())){
				unsigned int startI = vertexI->getTLevel();
				unsigned int endI = startI + vertexI->getCsDagReference()->getIntTiming(0);
				unsigned int startJ = vertexJ->getTLevel();
				unsigned int endJ = startJ + vertexJ->getCsDagReference()->getIntTiming(0);

				if((startI <= startJ && endI > startJ) ||
						(startJ <= startI && endJ > startI)){
					test = 0;
					printf("Superposition: task %s%d and %s%d\n",
							vertexI->getCsDagReference()->getName(),
							vertexI->getReferenceIndex(),
							vertexJ->getCsDagReference()->getName(),
							vertexJ->getReferenceIndex());
				}
			}
		}
	}
	return test;
}

inline int testCommunication(SRDAGGraph* srDag, Architecture* archi){
	int test=1;
	// Check Graph Dependency
	for (int i=0 ; i<srDag->getNbVertices() && test; i++){
		SRDAGVertex* vertexI = srDag->getVertex(i);
		unsigned int startI = vertexI->getTLevel();

		for (int j=0 ; j<vertexI->getNbInputEdge() && test; j++){
			SRDAGVertex* vertexJ = vertexI->getInputEdge(j)->getSource();
			unsigned int endJ = vertexJ->getTLevel() + vertexJ->getCsDagReference()->getIntTiming(0);

			if(startI >= endJ) test=1;
			else{
				test=0;
				printf("Communication: task %s%d -> %s%d\n",
						vertexJ->getCsDagReference()->getName(),
						vertexJ->getReferenceIndex(),
						vertexI->getCsDagReference()->getName(),
						vertexI->getReferenceIndex());
			}
		}
	}
	return test;
}

inline int testPipeline(SRDAGGraph* srDag, JobSet  *jobSet, Architecture* archi){
	int test=1;
	// Check Graph Dependency
	for (int i=0 ; i<srDag->getNbVertices() && test; i++){
		SRDAGVertex* vertexI = srDag->getVertex(i);
		int setI = jobSet->getOperation(vertexI->getCsDagReference()->getFunctionIndex())->getMachineSetId();

		for (int j=0 ; j<vertexI->getNbInputEdge() && test; j++){
			SRDAGVertex* vertexJ = vertexI->getInputEdge(j)->getSource();
			int setJ = jobSet->getOperation(vertexJ->getCsDagReference()->getFunctionIndex())->getMachineSetId();

			if(setI == setJ){
				if((test = (vertexJ->getSlaveIndex() == vertexI->getSlaveIndex()))==0)
					printf("Same set different slave: task %s%d (core %d) -> %s%d (core %d)\n",
						vertexJ->getCsDagReference()->getName(),
						vertexJ->getReferenceIndex(),
						vertexJ->getSlaveIndex(),
						vertexI->getCsDagReference()->getName(),
						vertexI->getReferenceIndex(),
						vertexI->getSlaveIndex());
			}else{
				if((test = (setJ < setI)) == 0)
					printf("Wrong set order: task %s%d -> %s%d\n",
						vertexJ->getCsDagReference()->getName(),
						vertexJ->getReferenceIndex(),
						vertexI->getCsDagReference()->getName(),
						vertexI->getReferenceIndex());
			}
		}
	}
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

void ScheduleChecker::checkFlow(SRDAGGraph* srDag, JobSet* jobSet, Architecture* archi){
	int ret=1;
#ifdef DISPLAY
	printf("Test FlowShop Schedule:\n");
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
	ret = testPipeline(srDag, jobSet, archi);
#ifdef DISPLAY
	printf("\tPipeline  : %s\n", (ret)?("Ok"):("Failed"));
#endif
	if(!ret) exitWithCode(1049);
}

///*
// * cmpFrontTests.cpp
// *
// *  Created on: Jan 8, 2013
// *      Author: jheulot
// */
//
//#include "applicationGraphs/testUplink.h"
//#include "ahsTests.h"
//#include "scheduling/FlowShopScheduler/FlowShopScheduler.h"
//#include "scheduling/FlowShopScheduler/processorMapper.h"
//#include "scheduling/ListScheduler/ListScheduler.h"
//#include "transformations/JobTransformer/JobTransformer.h"
//#include "transformations/CSDAGTransformer/CSDAGTransformer.h"
//#include "tools/Timer.h"
//#include "tools/DotWriter.h"
//#include "tools/ScheduleWriter.h"
//#include "tools/ScheduleChecker.h"
//#include "tools/Sdf3Parser.h"
//#include "graphs/CSDAG/CSDAGGraph.h"
//#include "graphs/ActorMachine/AMGraph.h"
//#include "graphs/JobSet/JobSet.h"
//#include "ExecutionStat.h"
//
//#include <iostream>
//#include <fstream>
//
//#define CSDAG_FILE_PATH 			"csdag.gv"
//#define STRUCTURE_FILE_PATH 		"srDag.gv"
//#define GANTT_FLOW_FILE_PATH 		"ganttEventsFlow.xml"
//#define GANTT_LIST_FILE_PATH 		"ganttEventsList.xml"
//#define MATLAB_SPEEDUPS_FILE_PATH 	"speedups.dat"
//#define MATLAB_SPANS_FILE_PATH 		"spans.dat"
//#define MATLAB_WORKS_FILE_PATH 		"works.dat"
//
//ExecutionStat test100LTE(int nbSlaves){
//	ExecutionStat execStat[100];
//	for(int i=0; i<100; i++){
//		execStat[i] = testLTE(nbSlaves);
//	}
//	execStat[0].average(execStat, 100);
//	return execStat[0];
//}
//
//
//ExecutionStat testLTE(int nbSlaves){
//	CSDAGTransformer 	csDAGTransformer;
//	JobTransformer 		jobTransformer;
//
//	FlowShopScheduler 	flowShopScheduler;
//	ListScheduler 		listScheduler;
//
//	ProcessorMapper processorMapper;
//
//	Architecture 	archi;
//	CSDAGGraph 		csDag;
//	SRDAGGraph 		srDagFlow;
//	SRDAGGraph 		srDagList;
//	JobSet 			jobSet;
//
//	DotWriter 		dotWriter;
//	ScheduleWriter 	schedWriter;
//	ExecutionStat execStat;
//	ScheduleChecker scheduleChecker;
//
//	AMGraph amGraph;
//
//	createSimpleArchi(&archi, nbSlaves);
//	printf("Allocating %i PEs\n", nbSlaves);
//
//	createGraphUplinkPerfTest(&csDag,&archi);
//
//	jobSet.flush(); srDagFlow.flush(); srDagList.flush();
//	listScheduler.setArchitecture(&archi);
//	flowShopScheduler.setArchitecture(&archi);
//
//#ifdef DISPLAY
//	dotWriter.write(&csDag, CSDAG_FILE_PATH, 1);
//#endif
//
//	timer.resetAndStart();
//	csDag.resolveTimings(&archi);
//	csDAGTransformer.transform(&csDag, &srDagFlow, &archi);
//	execStat.srDAGTransfTime = timer.getValueAndReset();
//
//	csDAGTransformer.transform(&csDag, &srDagList, &archi);
//	execStat.t1Latency = srDagList.getMaxTime();
//	execStat.criticalPath = srDagList.getCriticalPath();
//	dotWriter.write(&srDagList, STRUCTURE_FILE_PATH, 1);
//
//	timer.resetAndStart();
//	jobTransformer.transform(&srDagFlow, &jobSet, &archi);
//	execStat.jobTransfTime = timer.getValueAndReset();
//
//	processorMapper.doMapping(&srDagFlow, &jobSet, archi.getNbSlaves(), srDagFlow.getNbVertices());
//	execStat.flowMappingTime = timer.getValueAndReset();
//
//#ifdef DISPLAY
//	jobSet.printJobType();
//	jobSet.printMapping();
//#endif
//
//	execStat.nbFunction = csDag.getNbVertices();
//	for(int j=0; j<csDag.getNbVertices(); j++){
//		execStat.nbSlavesFunction[j] = jobSet.getMachineSet(jobSet.getOperation(j)->getMachineSetId())->getCount();
//	}
//
//	timer.resetAndStart();
//	flowShopScheduler.schedule(&csDag, &jobSet);
//	execStat.flowScheduleTime = timer.getValueAndReset();
//	execStat.flowMakespan = flowShopScheduler.getMakespan();
//	execStat.flowThroughput = flowShopScheduler.getThroughput();
//#ifdef DISPLAY
//	schedWriter.write(&csDag, &srDagFlow, &archi, GANTT_FLOW_FILE_PATH);
//#endif
//
//	scheduleChecker.checkFlow(&srDagFlow, &jobSet, &archi);
//
//	timer.resetAndStart();
//	listScheduler.schedule(&csDag, &srDagList);
//	execStat.listMakespan = listScheduler.getMakespan(&srDagList);
//	execStat.listThroughput = listScheduler.getThroughput(&srDagList);
//	execStat.listScheduleTime = timer.getValueAndReset();
//#ifdef DISPLAY
//	schedWriter.write(&csDag, &srDagList, &archi, GANTT_LIST_FILE_PATH);
//#endif
//
//	scheduleChecker.checkList(&srDagFlow, &archi);
//
//	amGraph = AMGraph(srDagList.getVertex(10));
//	amGraph.toDot("amgraph.cv");
//
//	return execStat;
//}
//
//ExecutionStat testMaxLTE(){
//	CSDAGTransformer 	csDAGTransformer;
//	JobTransformer 		jobTransformer;
//
//	FlowShopScheduler 	flowShopScheduler;
//	ListScheduler 		listScheduler;
//
//	ProcessorMapper processorMapper;
//
//	Architecture 	archi;
//	CSDAGGraph 		csDag;
//	SRDAGGraph 		srDagFlow;
//	SRDAGGraph 		srDagList;
//	JobSet 			jobSet;
//
//	DotWriter 		dotWriter;
//	ScheduleWriter 	schedWriter;
//	ExecutionStat execStat;
//	ScheduleChecker scheduleChecker;
//
//	createSimpleArchi(&archi, 3);
//	printf("Allocating %i PEs\n", 3);
//
//	createGraphUplinkMaxTest(&csDag,&archi);
//
//	jobSet.flush(); srDagFlow.flush(); srDagList.flush();
//	listScheduler.setArchitecture(&archi);
//	flowShopScheduler.setArchitecture(&archi);
//
//	#ifdef DISPLAY
//	dotWriter.write(&csDag, CSDAG_FILE_PATH, 1);
//	#endif
//
//	timer.resetAndStart();
//	csDag.resolveTimings(&archi);
//	csDAGTransformer.transform(&csDag, &srDagFlow, &archi);
//	execStat.srDAGTransfTime = timer.getValueAndReset();
//
//	csDAGTransformer.transform(&csDag, &srDagList, &archi);
//	execStat.t1Latency = srDagList.getMaxTime();
//	execStat.criticalPath = srDagList.getCriticalPath();
//	dotWriter.write(&srDagList, STRUCTURE_FILE_PATH, 1);
//
//	timer.resetAndStart();
//	jobTransformer.transform(&srDagFlow, &jobSet, &archi);
//	execStat.jobTransfTime = timer.getValueAndReset();
//
//	processorMapper.doMapping(&srDagFlow, &jobSet, archi.getNbSlaves(), srDagFlow.getNbVertices());
//	execStat.flowMappingTime = timer.getValueAndReset();
//
//#ifdef DISPLAY
//	jobSet.printJobType();
//	jobSet.printMapping();
//#endif
//
//	execStat.nbFunction = csDag.getNbVertices();
//	for(int j=0; j<csDag.getNbVertices(); j++){
//		execStat.nbSlavesFunction[j] = jobSet.getMachineSet(jobSet.getOperation(j)->getMachineSetId())->getCount();
//	}
//
//	timer.resetAndStart();
//	flowShopScheduler.schedule(&csDag, &jobSet);
//	execStat.flowScheduleTime = timer.getValueAndReset();
//	execStat.flowMakespan = flowShopScheduler.getMakespan();
//	execStat.flowThroughput = flowShopScheduler.getThroughput();
//
//#ifdef DISPLAY
//	schedWriter.write(&csDag, &srDagFlow, &archi, GANTT_FLOW_FILE_PATH);
//#endif
//
//	scheduleChecker.checkFlow(&srDagFlow, &jobSet, &archi);
//
//	timer.resetAndStart();
//	listScheduler.schedule(&csDag, &srDagList);
//	execStat.listMakespan = listScheduler.getMakespan(&srDagList);
//	execStat.listThroughput = listScheduler.getThroughput(&srDagList);
//	execStat.listScheduleTime = timer.getValueAndReset();
//
//#ifdef DISPLAY
//	schedWriter.write(&csDag, &srDagList, &archi, GANTT_LIST_FILE_PATH);
//#endif
//
//	scheduleChecker.checkList(&srDagFlow, &archi);
//
//	return execStat;
//}
//
//ExecutionStat testSimpleLTE(){
//	CSDAGTransformer 	csDAGTransformer;
//	JobTransformer 		jobTransformer;
//
//	FlowShopScheduler 	flowShopScheduler;
//	ListScheduler 		listScheduler;
//
//	ProcessorMapper processorMapper;
//
//	Architecture 	archi;
//	CSDAGGraph 		csDag;
//	SRDAGGraph 		srDagFlow;
//	SRDAGGraph 		srDagList;
//	JobSet 			jobSet;
//
//	DotWriter 		dotWriter;
//	ScheduleWriter 	schedWriter;
//	ExecutionStat execStat;
//	ScheduleChecker scheduleChecker;
//
//	createSimpleArchi(&archi, 7);
//	printf("Allocating %i PEs\n", 7);
//
//	createGraphUplinkSimpleTest(&csDag,&archi);
//
//	jobSet.flush(); srDagFlow.flush(); srDagList.flush();
//	listScheduler.setArchitecture(&archi);
//	flowShopScheduler.setArchitecture(&archi);
//
//	#ifdef DISPLAY
//	dotWriter.write(&csDag, CSDAG_FILE_PATH, 1);
//	#endif
//
//	timer.resetAndStart();
//	csDag.resolveTimings(&archi);
//	csDAGTransformer.transform(&csDag, &srDagFlow, &archi);
//	execStat.srDAGTransfTime = timer.getValueAndReset();
//
//	csDAGTransformer.transform(&csDag, &srDagList, &archi);
//	execStat.t1Latency = srDagList.getMaxTime();
//	execStat.criticalPath = srDagList.getCriticalPath();
//	dotWriter.write(&srDagList, STRUCTURE_FILE_PATH, 1);
//
//	timer.resetAndStart();
//	jobTransformer.transform(&srDagFlow, &jobSet, &archi);
//	execStat.jobTransfTime = timer.getValueAndReset();
//
//	processorMapper.do7Mapping(&jobSet);
//	execStat.flowMappingTime = timer.getValueAndReset();
//
//#ifdef DISPLAY
//	jobSet.printJobType();
//	jobSet.printMapping();
//#endif
//
//	execStat.nbFunction = csDag.getNbVertices();
//	for(int j=0; j<csDag.getNbVertices(); j++){
//		execStat.nbSlavesFunction[j] = jobSet.getMachineSet(jobSet.getOperation(j)->getMachineSetId())->getCount();
//	}
//
//	timer.resetAndStart();
//	flowShopScheduler.schedule(&csDag, &jobSet);
//	execStat.flowScheduleTime = timer.getValueAndReset();
//	execStat.flowMakespan = flowShopScheduler.getMakespan();
//	execStat.flowThroughput = flowShopScheduler.getThroughput();
//
//#ifdef DISPLAY
//	schedWriter.write(&csDag, &srDagFlow, &archi, GANTT_FLOW_FILE_PATH);
//#endif
//
//	scheduleChecker.checkFlow(&srDagFlow, &jobSet, &archi);
//
//	timer.resetAndStart();
//	listScheduler.schedule(&csDag, &srDagList);
//	execStat.listMakespan = listScheduler.getMakespan(&srDagList);
//	execStat.listThroughput = listScheduler.getThroughput(&srDagList);
//	execStat.listScheduleTime = timer.getValueAndReset();
//
//#ifdef DISPLAY
//	schedWriter.write(&csDag, &srDagList, &archi, GANTT_LIST_FILE_PATH);
//#endif
//
//	scheduleChecker.checkList(&srDagFlow, &archi);
//
//	return execStat;
//}
///*
//int testSDF3(int nbSlaves){
//	CSDAGGraph *csDag = new CSDAGGraph();
//	Architecture *archi = new Architecture();
//	TransformerScheduler *schedulerFlow = new TransformerScheduler(FLOW_SHOP_SCHEDULER_TYPE);
//	TransformerScheduler *schedulerList = new TransformerScheduler(LIST_SCHEDULER_TYPE);
//
//	createSimpleArchi(archi, nbSlaves);
//	printf("Allocating %i PEs\n", nbSlaves);
//
//	std::ofstream outFile;
//	outFile.open("out.csv",  ios_base::app);
//
//	float Tinf = 93702;
//	float T1 = 438100;
//
//	//createGraphUplinkPerfTest(csDag,archi);
//	createGraphUplinkSimpleTest(csDag,archi);
//	//sdf3Parser.parse("test.sdf",  csDag, archi);
//
//	schedulerFlow->generateAndSchedule(csDag, archi);
//	schedulerList->generateAndSchedule(csDag, archi);
//
//	cout << nbSlaves << ": " << T1/schedulerFlow->getScheduler()->getMakespan(schedulerFlow->getAlgorithmStructure())
//			<< "  " << T1/schedulerList->getScheduler()->getMakespan(schedulerList->getAlgorithmStructure())
//			<< "  " << nbSlaves
//			<< "  " << T1/Tinf
//			<< "  " << T1/(T1/nbSlaves+Tinf)
//			<< "  " << T1/schedulerFlow->getScheduler()->getThroughput(schedulerFlow->getAlgorithmStructure())
//			<< "  " << T1/schedulerList->getScheduler()->getThroughput(schedulerList->getAlgorithmStructure())
//			<< endl;
//
//	outFile << nbSlaves << "," << T1/schedulerFlow->getScheduler()->getMakespan(schedulerFlow->getAlgorithmStructure())
//				<< "," << T1/schedulerList->getScheduler()->getMakespan(schedulerList->getAlgorithmStructure())
//				<< "," << nbSlaves
//				<< "," << T1/Tinf
//				<< "," << T1/(T1/nbSlaves+Tinf)
//				<< "," << T1/schedulerFlow->getScheduler()->getThroughput(schedulerFlow->getAlgorithmStructure())
//				<< "," << T1/schedulerList->getScheduler()->getThroughput(schedulerList->getAlgorithmStructure())
//				<< endl;
//
//	outFile.close();
//
//	delete csDag;
//	delete archi;
//	delete schedulerFlow;
//	delete schedulerList;
//
//	return 0;
//}
//*/

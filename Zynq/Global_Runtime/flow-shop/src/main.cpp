/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 * Main function of the DAG online scheduler runtime
 * 
 * @author mpelcat
 */

#include "applicationGraphs/testUplink.h"
#include "ahsTests.h"
#include <iostream>
#include <fstream>

#include "tools/Sdf3Parser.h"
#include "tools/DotWriter.h"


int main(int argc, char* argv[]){/*
	if(argc!=2){
		printf("Usage: %s nbSlaves\n", argv[0]);
		return 0;
	}
	int N=atoi(argv[1]);*/

	std::ofstream outFile;
	outFile.open("out.csv",  ios_base::trunc);

//	for(int i=1; i<256; i++){
	int i = 8;{
		ExecutionStat s = testLTE(i);

		cout << i << ":\tSpeedup\t\tThroughput\tExecTime" << endl
			<< "  Flow: " << (double)s.t1Latency/s.flowMakespan << "\t\t " << (double)s.t1Latency/s.flowThroughput
						  << "\t " << s.getFlowExecutionTime() << "\t( " << s.getFlowExecutionTime()/667000.0 << "ms )" << endl
			<< "  List: " << (double)s.t1Latency/s.listMakespan << "\t\t " << (double)s.t1Latency/s.listThroughput
						  << "\t " << s.getListExecutionTime() << "\t( " << s.getListExecutionTime()/667000.0 << "ms )" << endl;

		outFile << i << ","
				<< s.flowScheduleTime	<< ","
				<< s.listScheduleTime	<< ","
				<< s.srDAGTransfTime	<< ","
				<< s.jobTransfTime		<< ","
				<< s.flowMappingTime	<< ","
				<< s.flowMakespan		<< ","
				<< s.listMakespan		<< ","
				<< s.flowThroughput		<< ","
				<< s.listThroughput		<< ","
				<< s.criticalPath		<< ","
				<< s.t1Latency;
		for(unsigned int j=0; j<s.nbFunction; j++){
			outFile << "," << s.nbSlavesFunction[j];
		}
		outFile << endl;
	}
	outFile.close();

	//testSDF3(N);
	printf("Finished\n");
	return 0;
}


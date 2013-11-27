
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/


#include <iostream>
#include <fstream>

#include "tools/DotWriter.h"

#include "hwQueues.h"

#include "mpeg4_part2_main.h"


//ExecutionStat testAM();
void testCom();
void sobel(int nbSlaves);
int mpeg4_main(int argc, char* argv[]);

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("Usage: %s nbSlaves\n", argv[0]);
		return 0;
	}
	int N = atoi(argv[1]);

//	std::ofstream outFile;
//	outFile.open("out.csv",  ios_base::trunc);

//	testAM();

//	testCom(N);

//	testCom();

	printf("Starting with %d slaves max\n",N);

//	sobel(N);

	mpeg4_part2_main(N);

//	for(int i=1; i<256; i++){
////	int i = 110;{
//		ExecutionStat s = testLTE(i);
//
//		cout << i << ":\tSpeedup\t\tThroughput\tExecTime" << endl
//			<< "  Flow: " << (double)s.t1Latency/s.flowMakespan << "\t\t " << (double)s.t1Latency/s.flowThroughput
//						  << "\t " << s.getFlowExecutionTime() << "\t( " << s.getFlowExecutionTime()/667000.0 << "ms )" << endl
//			<< "  List: " << (double)s.t1Latency/s.listMakespan << "\t\t " << (double)s.t1Latency/s.listThroughput
//						  << "\t " << s.getListExecutionTime() << "\t( " << s.getListExecutionTime()/667000.0 << "ms )" << endl;
//
//		outFile << i << ","
//				<< s.flowScheduleTime	<< ","
//				<< s.listScheduleTime	<< ","
//				<< s.srDAGTransfTime	<< ","
//				<< s.jobTransfTime		<< ","
//				<< s.flowMappingTime	<< ","
//				<< s.flowMakespan		<< ","
//				<< s.listMakespan		<< ","
//				<< s.flowThroughput		<< ","
//				<< s.listThroughput		<< ","
//				<< s.criticalPath		<< ","
//				<< s.t1Latency;
//		for(unsigned int j=0; j<s.nbFunction; j++){
//			outFile << "," << s.nbSlavesFunction[j];
//		}
//		outFile << endl;
//	}
//	outFile.close();

	//testSDF3(N);
	while(1);
	return 0;
}


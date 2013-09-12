///*********************************************************
//Copyright or � or Copr. IETR/INSA: Maxime Pelcat
//
//Contact mpelcat for more information:
//mpelcat@insa-rennes.fr
//
//This software is a computer program whose purpose is to execute
//parallel applications.
//
// *********************************************************/
//
//#include "testUplink.h"
//
///**
// * Creating test case graphs and archis
// *
// * @author mpelcat
// */
//
//// Uplink parameterized with 4 users max
//void createGraphAndArchiUplink4Max(CSDAGGraph* graph, Architecture* archi){
//
//	// Architecture
//	archi->addSlave(0,"core0");
//	archi->addSlave(0,"core1");
//	archi->addSlave(0,"core2");
//	archi->addSlave(0,"core3");
//	//archi->addSlave(0,"core4");
//	//archi->addSlave(0,"core5");
//	//archi->addSlave(0,"core6");
//	//archi->addSlave(0,"core7");
//
//	// Variables
//	globalParser.addVariable("ul_max_tones_per_user",20);
//	globalParser.addVariable("ul_nb_user",4);
//	globalParser.addVariable("ul_tones_user1",20);
//	globalParser.addVariable("ul_tones_user2",10);
//	globalParser.addVariable("ul_tones_user3",5);
//	globalParser.addVariable("ul_tones_user4",1);
//	globalParser.addVariable("ul_nb_cbs_user1",20);
//	globalParser.addVariable("ul_nb_cbs_user2",10);
//	globalParser.addVariable("ul_nb_cbs_user3",5);
//	globalParser.addVariable("ul_nb_cbs_user4",1);
//	globalParser.addVariable("ul_block_size",20);
//
//	globalParser.addVariable("defaultTime",100);
//
//	// Vertices
//	CSDAGVertex* vStaticPart = graph->addVertex("StaticPart");
//	CSDAGVertex* vConvergence = graph->addVertex("Convergence");
//	CSDAGVertex* vKeepCurrentUserTones = graph->addVertex("KeepCurrentUserTones");
//	CSDAGVertex* vfOffsetConstDemDecDeint = graph->addVertex("fOffsetConstDemDecDeint");
//	CSDAGVertex* vBitProcessing = graph->addVertex("BitProcessing");
//	CSDAGVertex* vTCP3dec = graph->addVertex("TCP3dec");
//	CSDAGVertex* vCRCCheck2 = graph->addVertex("CRCCheck2");
//
//	// Timings -> approximate retrieved from TI uplink.
//	vStaticPart->addTiming(0,"1"); // Not taken into account
//	vStaticPart->addAllConstraints(1, archi->getNbSlaves());
//	vConvergence->addTiming(0,"1000"); // Not taken into account
//	vConvergence->addAllConstraints(1, archi->getNbSlaves());
//	vKeepCurrentUserTones->addTiming(0,"1000"); // Not taken into account
//	vKeepCurrentUserTones->addAllConstraints(1, archi->getNbSlaves());
//	vfOffsetConstDemDecDeint->addTiming(0,"40000");
//	vfOffsetConstDemDecDeint->addAllConstraints(1, archi->getNbSlaves());
//	vBitProcessing->addTiming(0,"13000");
//	vBitProcessing->addAllConstraints(1, archi->getNbSlaves());
//	vTCP3dec->addTiming(0,"250000");
//	vTCP3dec->addAllConstraints(1, archi->getNbSlaves());
//	vCRCCheck2->addTiming(0,"2700");
//	vCRCCheck2->addAllConstraints(1, archi->getNbSlaves());
//
//	// Edges
//	graph->addEdge(vStaticPart,"1",vConvergence,"1");
//	graph->addEdge(vConvergence,"ul_max_tones_per_user*ul_nb_user",vKeepCurrentUserTones,"ul_max_tones_per_user");
//	graph->addEdge(vKeepCurrentUserTones,"{ul_tones_user1,ul_tones_user2,ul_tones_user3,ul_tones_user4}",vfOffsetConstDemDecDeint,"{ul_tones_user1,ul_tones_user2,ul_tones_user3,ul_tones_user4}");
//	graph->addEdge(vfOffsetConstDemDecDeint,"{ul_tones_user1,ul_tones_user2,ul_tones_user3,ul_tones_user4}",vBitProcessing,"{ul_tones_user1/ul_nb_cbs_user1,ul_tones_user2/ul_nb_cbs_user2,ul_tones_user3/ul_nb_cbs_user3,ul_tones_user4/ul_nb_cbs_user4}");
//	graph->addEdge(vBitProcessing,"(3*ul_block_size)+12",vTCP3dec,"(3*ul_block_size)+12");
//	graph->addEdge(vTCP3dec,"ul_block_size",vCRCCheck2,"{ul_block_size*ul_nb_cbs_user1,ul_block_size*ul_nb_cbs_user2,ul_block_size*ul_nb_cbs_user3,ul_block_size*ul_nb_cbs_user4}");
//}
//
//// Architecture with one TCI6488 (3 cores) and one TCI6486 (6 cores)
//void createArchiFaradayTomahawk(Architecture* archi){
//	static char tempStr[8];
//
//	// Architecture
//
//	// Faraday cores (minus one for scheduling)
//	for(int i=0; i<2; i++){
//		sprintf(tempStr,"far%d",i);
//		archi->addSlave(0,tempStr);
//	}
//
//	// Tomahawk cores cores
//	for(int i=2; i<8; i++){
//		sprintf(tempStr,"tom%d",i);
//		archi->addSlave(1,tempStr);
//	}
//
//	// 1.6Gbytes/s locally on Faraday
//	for(int i=0; i<2; i++){
//		for(int j=0; j<2; j++){
//			archi->setComRate(i,j,1600000*8);
//			archi->setComRate(j,i,1600000*8);
//		}
//	}
//
//	// 1.6Gbytes/s locally on Tomahawk
//	for(int i=2; i<8; i++){
//		for(int j=2; j<8; j++){
//			archi->setComRate(i,j,1600000*8);
//			archi->setComRate(j,i,1600000*8);
//		}
//	}
//
//	// 1Gbit/s = 1000000 kBit/s inter (RapidIO serial link)
//	for(int i=0; i<2; i++){
//		for(int j=2; j<8; j++){
//			archi->setComRate(i,j,1000000);
//			archi->setComRate(j,i,1000000);
//		}
//	}
//
//	// Sender and receiver on the same core: no com cost
//	for(int i=0; i<8; i++){
//			archi->setComRate(i,i,-1);
//	}
//}
//
//// nbSlaves cores with full mesh homogeneous interconnection
//void createSimpleArchi(Architecture* archi, int nbSlaves){
//	static char tempStr[8];
//
//	// Architecture
//
//	// Faraday cores (minus one for scheduling)
//	for(int i=0; i<nbSlaves; i++){
//		sprintf(tempStr,"slave%03d",i);
//		archi->addSlave(0,tempStr);
//	}
//
//	// 1Gbytes/s locally on Faraday
//	for(int i=0; i<nbSlaves; i++){
//		for(int j=0; j<nbSlaves; j++){
//			archi->setComRate(i,j,1000000*8);
//			archi->setComRate(j,i,1000000*8);
//		}
//	}
//
//	// Sender and receiver on the same core: no com cost
//	for(int i=0; i<nbSlaves; i++){
//			archi->setComRate(i,i,-1);
//	}
//}
//
//// Uplink parameterized with 100 users max
//void createGraphUplink100Max(CSDAGGraph* graph, Architecture* archi){
//
//	int N=100;
//
//	static char tempStr[4000];
//
//	// Variables
//
//	globalParser.addVariable("ul_max_tones_per_user",1);
//	globalParser.addVariable("ul_nb_cb",100);
//	globalParser.addVariable("ul_nb_user",N);
//
//	// Initializing tones per user
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"ul_tones_user%d",i);
//		globalParser.addVariable(tempStr,1*12);
//	}
//
//	// Initializing code blocks
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(tempStr,1);
//	}
//
//	globalParser.addVariable("ul_block_size",200);
//	globalParser.addVariable("defaultTime",100);
//
//	// Adding a parameter pattern to a vertex: the parameters will be solved in the order of instance calls
//	// and their value passed to the actor in the order they have been added. Moreover, assigning
//	// a variable in the pattern means this variable value will be interpreted before production
//	// and consumption patterns are evaluated (ul_tones_user can be different for each firing of an actor).
//	strcpy(tempStr, "{");
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"%sul_tones_user=ul_tones_user%d",tempStr,i);
//		if(i != N-1){
//			strcat(tempStr, ",");
//		}
//	}
//	strcat(tempStr, "}");
//
//	// Vertices
//	// The functions are retrieved from the table named "functionPointers" with the index set here
//	CSDAGVertex* vStaticPart = graph->addVertex("StaticPart");
//	vStaticPart->setFunctionIndex(0);
//	vStaticPart->addParamPattern("ul_nb_user");
//
//	CSDAGVertex* vConvergence = graph->addVertex("Convergence");
//	vConvergence->setFunctionIndex(1);
//
//	CSDAGVertex* vKeepCurrentUserTones = graph->addVertex("KeepCurrentUserTones");
//	vKeepCurrentUserTones->setFunctionIndex(2);
//	vKeepCurrentUserTones->addParamPattern(tempStr); // Adding ul_tones_user=ul_tones_user%d
//
//	CSDAGVertex* vfOffsetConstDemDecDeint = graph->addVertex("fOffsetConstDemDecDeint");
//	vfOffsetConstDemDecDeint->setFunctionIndex(3);
//	vfOffsetConstDemDecDeint->addParamPattern(tempStr);
//
//	CSDAGVertex* vBitProcessing = graph->addVertex("BitProcessing");
//	vBitProcessing->setFunctionIndex(4);
//	vBitProcessing->addParamPattern(tempStr);
//
//	strcpy(tempStr, "{");
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"%sul_nb_cbs_user=ul_nb_cbs_user%d",tempStr,i);
//		if(i != N-1){
//			strcat(tempStr, ",");
//		}
//	}
//	strcat(tempStr, "}");
//
//	vBitProcessing->addParamPattern(tempStr);
//
//	CSDAGVertex* vTCP3dec = graph->addVertex("TCP3dec");
//	vTCP3dec->setFunctionIndex(5);
//
//	CSDAGVertex* vCRCCheck2 = graph->addVertex("CRCCheck2");
//	vCRCCheck2->setFunctionIndex(6);
//	vCRCCheck2->addParamPattern(tempStr);
//
//	// Timings -> retrieved from Preesm LTE_1Cell_TPIC_2Iters uplink project. In thousands of cycles
//	vStaticPart->addTiming(0,"1"); // Faraday c64x+ (1 GHz)
//	vStaticPart->addTiming(1,"1"); // Tomahawk c64x+ (700 MHz)
//	vStaticPart->addAllConstraints(1, archi->getNbSlaves());
//	vConvergence->addTiming(0,"1"); // Faraday c64x+
//	vConvergence->addTiming(1,"1"); // Tomahawk c64x+
//	vConvergence->addAllConstraints(1, archi->getNbSlaves());
//	vKeepCurrentUserTones->addTiming(0,"1"); // Faraday c64x+
//	vKeepCurrentUserTones->addTiming(1,"1"); // Tomahawk c64x+
//	vKeepCurrentUserTones->addAllConstraints(1, archi->getNbSlaves());
//	vfOffsetConstDemDecDeint->addTiming(0,"40000"); // Faraday c64x+
//	vfOffsetConstDemDecDeint->addTiming(1,"57142"); // Tomahawk c64x+
//	vfOffsetConstDemDecDeint->addAllConstraints(1, archi->getNbSlaves());
//	vBitProcessing->addTiming(0,"13000"); // Faraday c64x+
//	vBitProcessing->addTiming(1,"18571"); // Tomahawk c64x+
//	vBitProcessing->addAllConstraints(1, archi->getNbSlaves());
//	vTCP3dec->addTiming(0,"38000"); // Faraday c64x+
//	vTCP3dec->addTiming(1,"54285"); // Tomahawk c64x+
//	vTCP3dec->addAllConstraints(1, archi->getNbSlaves());
//	vCRCCheck2->addTiming(0,"2700"); // Faraday c64x+ // 2700
//	vCRCCheck2->addTiming(1,"3857"); // Tomahawk c64x+
//	vCRCCheck2->addAllConstraints(1, archi->getNbSlaves());
//
//	// Edges
//	graph->addEdge(vStaticPart,"1",vConvergence,"1");
//	graph->addEdge(vConvergence,"ul_max_tones_per_user*ul_nb_user",vKeepCurrentUserTones,"ul_max_tones_per_user");
//	graph->addEdge(vKeepCurrentUserTones,"ul_tones_user",vfOffsetConstDemDecDeint,"ul_tones_user");
//	graph->addEdge(vfOffsetConstDemDecDeint,"ul_tones_user*4",vBitProcessing,"12*4");
//	graph->addEdge(vBitProcessing,"(3*ul_block_size)+12",vTCP3dec,"(3*ul_block_size)+12");
//	graph->addEdge(vTCP3dec,"ul_block_size",vCRCCheck2,"ul_block_size*ul_nb_cbs_user");
//}
//
//// Uplink parameterized with 100 users max
//void setRandomParamsUplink100Max(){
//
//	static char name[40];
//
//	/* initialize random seed: */
//	srand ( (unsigned int)time(NULL) );
//
//
//    // Choosing the max number of tones per user
//	int maxRBsPerUser = 10;
//    // Choosing a maximum number of users
//	int maxN = 100;
//    // Choosing a maximum total number of tones
//	int maxAvailableRBs = 100;
//
//    // Choosing a number of users
//	int N = rand()%maxN + 1;
//
//    // Choosing the total number of tones
//	int availableRBs = rand()%maxAvailableRBs + 1;
//	globalParser.addVariable("ul_nb_cb",availableRBs);
//
//	int nbUser = N;
//	int u;
//
//	// Initializing the number of codeblocks and number of tones per user to zero for all users
//	for(int i=0; i<100; i++){
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,0);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,0);
//	}
//
//	// Calculating the real  the number of codeblocks and number of tones per user
//	for(int i=0; i<N; i++){
//
//		if(i == N-1){
//			u = availableRBs; // We would need to assert u <= maxAvailableRBs
//		}
//		else if(availableRBs>1){
//			// Setting the number of RBs for each user
//			u = (availableRBs < maxRBsPerUser) ? availableRBs : maxRBsPerUser;
//			u = (rand()%u)+1;
//
//			if(u > maxRBsPerUser){
//				u = maxRBsPerUser;
//			}
//		}
//		else if(availableRBs==1) u = 1;
//		else u = 0;
//
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,u*12);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,u);
//		availableRBs = availableRBs - u;
//
//		if(u == 0 && nbUser > i){
//			nbUser = i;
//		}
//	}
//
//	//globalParser.addVariable("ul_max_tones_per_user",maxRBsPerUser*12);
//	globalParser.addVariable("ul_nb_user",nbUser);
//}
//
//
//// Uplink parameterized with a custom max number of UEs and RBs
//void setRandomParamsUplinkCustom(){
//
//	static char name[40];
//
//	/* initialize random seed: */
//	//srand ( (unsigned int)time(NULL) );
//
//
//    // Choosing the max number of tones per user
//	int maxRBsPerUser = 10;
//    // Choosing a maximum number of users
//	int maxN = 20;
//    // Choosing a maximum total number of tones
//	int maxAvailableRBs = 50;
//
//    // Choosing a number of users
//	int N = rand()%maxN + 1;
//
//    // Choosing the total number of tones
//	int availableRBs = rand()%maxAvailableRBs + 1;
//	globalParser.addVariable("ul_nb_cb",availableRBs);
//
//	int nbUser = N;
//	int u;
//
//	// Initializing the number of codeblocks and number of tones per user to zero for all users
//	for(int i=0; i<100; i++){
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,0);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,0);
//	}
//
//	// Calculating the real  the number of codeblocks and number of tones per user
//	for(int i=0; i<N; i++){
//
//		if(i == N-1){
//			u = availableRBs; // We would need to assert u <= maxAvailableRBs
//		}
//		else if(availableRBs>1){
//			// Setting the number of RBs for each user
//			u = (availableRBs < maxRBsPerUser) ? availableRBs : maxRBsPerUser;
//			u = (rand()%u)+1;
//
//			if(u > maxRBsPerUser){
//				u = maxRBsPerUser;
//			}
//		}
//		else if(availableRBs==1) u = 1;
//		else u = 0;
//
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,u*12);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,u);
//		availableRBs = availableRBs - u;
//
//		if(u == 0 && nbUser > i){
//			nbUser = i;
//		}
//	}
//
//	//globalParser.addVariable("ul_max_tones_per_user",maxRBsPerUser*12);
//	globalParser.addVariable("ul_nb_user",nbUser);
//}
//
//// Uplink parameterized with a fixed max number of UEs and RBs
//void setRandomParamsUplinkFixed(){
//
//	static char name[40];
//
//	/* initialize random seed: */
//	//srand ( (unsigned int)time(NULL) );
//
//
//    // Choosing the max number of tones per user
//	int maxRBsPerUser = 1;
//    // Choosing a maximum number of users
//	int maxN = 100;
//    // Choosing a maximum total number of tones
//	int maxAvailableRBs = 10;
//
//    // Choosing a number of users
//	int N = 3;
//
//    // Choosing the total number of tones
//	int availableRBs = 10;
//	globalParser.addVariable("ul_nb_cb",availableRBs);
//
//	int nbUser = N;
//	int u;
//
//	// Initializing the number of codeblocks and number of tones per user to zero for all users
//	for(int i=0; i<100; i++){
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,0);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,0);
//	}
//
//	// Calculating the real  the number of codeblocks and number of tones per user
//	for(int i=0; i<N; i++){
//
//		if(i == N-1){
//			u = availableRBs; // We would need to assert u <= maxAvailableRBs
//		}
//		else if(availableRBs>1){
//			// Setting the number of RBs for each user
//			u = (availableRBs < maxRBsPerUser) ? availableRBs : maxRBsPerUser;
//			u = 10;
//
//			if(u > maxRBsPerUser){
//				u = maxRBsPerUser;
//			}
//		}
//		else if(availableRBs==1) u = 1;
//		else u = 0;
//
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,u*12);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,u);
//		availableRBs = availableRBs - u;
//
//		if(u == 0 && nbUser > i){
//			nbUser = i;
//		}
//	}
//
//	//globalParser.addVariable("ul_max_tones_per_user",maxRBsPerUser*12);
//	globalParser.addVariable("ul_nb_user",nbUser);
//}
//
//// Testing all combinations of nb_user and nb_cbs
//int currentN = 1;
//int currentAvailableTones = 1;
//void testAllParamsUplink100Max(){
//
//	static char name[40];
//
//    // Choosing the max number of tones per user
//	int maxTonesPerUser = 100;
//
//    // Choosing a number of users
//	int N = currentN;
//
//    // Choosing the total number of tones
//	int availableTones = currentAvailableTones;
//	globalParser.addVariable("ul_nb_cb",currentAvailableTones);
//
//	int nbUser = N;
//	int u;
//
//	// Initializing the number of codeblocks and number of tones per user to zero for all users
//	for(int i=0; i<100; i++){
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,0);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,0);
//	}
//
//	// Calculating the real  the number of codeblocks and number of tones per user
//	for(int i=0; i<N; i++){
//
//		if(i == N-1){
//			u = availableTones; // We would need to assert u <= maxAvailableTones
//		}
//		else if(availableTones>1){
//			u = 1;
//		}
//		else if(availableTones==1) u = 1;
//		else u = 0;
//
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,u*12);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,u);
//		availableTones = availableTones - u;
//
//		if(u == 0 && nbUser > i){
//			nbUser = i;
//		}
//	}
//
//	//globalParser.addVariable("ul_max_tones_per_user",maxTonesPerUser*12);
//	globalParser.addVariable("ul_nb_user",nbUser);
//
//	currentN++;
//
//	if(currentN > 100){
//		currentAvailableTones++;
//		currentN = 1;
//	}
//
//	if(currentAvailableTones > 100){
//		currentAvailableTones = 1;
//	}
//}
//
//// Setting parameters for 100 CBs and 100 UEs
//void set100CB_100UE(){
//
//	static char name[40];
//
//	int maxTonesPerUser = 12;
//    // Choosing a number of users
//
//    // Choosing the total number of tones
//	int availableTones = 1200;
//	globalParser.addVariable("ul_nb_cb",currentAvailableTones);
//
//	// Initializing the number of codeblocks and number of tones per user to zero for all users
//	for(int i=0; i<100; i++){
//		sprintf(name,"ul_tones_user%d",i);
//		globalParser.addVariable(name,12);
//		sprintf(name,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(name,1);
//	}
//
//	globalParser.addVariable("ul_nb_user",100);
//}
//
//void createGraphUplinkMaxTest(CSDAGGraph* graph, Architecture* archi){
//
//	int N=100;
//
//	static char tempStr[4000];
//
//	// Variables
//
//	globalParser.addVariable("ul_max_tones_per_user",1);
//	globalParser.addVariable("ul_nb_cb",100);
//	globalParser.addVariable("ul_nb_user",N);
//
//	for(int i=0; i<100; i++){
//		sprintf(tempStr,"ul_tones_user%d",i);
//		globalParser.addVariable(tempStr,1*12);
//		sprintf(tempStr,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(tempStr,1);
//	}
//
//	globalParser.addVariable("ul_block_size",200);
//	globalParser.addVariable("defaultTime",100);
//
//	// Adding a parameter pattern to a vertex: the parameters will be solved in the order of instance calls
//	// and their value passed to the actor in the order they have been added. Moreover, assigning
//	// a variable in the pattern means this variable value will be interpreted before production
//	// and consumption patterns are evaluated (ul_tones_user can be different for each firing of an actor).
//	strcpy(tempStr, "{");
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"%sul_tones_user=ul_tones_user%d",tempStr,i);
//		if(i != N-1){
//			strcat(tempStr, ",");
//		}
//	}
//	strcat(tempStr, "}");
//
//	// Vertices
//	// The functions are retrieved from the table named "functionPointers" with the index set here
//	CSDAGVertex* vConvergence = graph->addVertex("Convergence");
//	vConvergence->setFunctionIndex(0);
//
//	CSDAGVertex* vKeepCurrentUserTones = graph->addVertex("KeepCurrentUserTones");
//	vKeepCurrentUserTones->setFunctionIndex(1);
//	vKeepCurrentUserTones->addParamPattern(tempStr); // Adding ul_tones_user=ul_tones_user%d
//
//	CSDAGVertex* vfOffsetConstDemDecDeint = graph->addVertex("fOffsetConstDemDecDeint");
//	vfOffsetConstDemDecDeint->setFunctionIndex(2);
//	vfOffsetConstDemDecDeint->addParamPattern(tempStr);
//
//	CSDAGVertex* vBitProcessing = graph->addVertex("BitProcessing");
//	vBitProcessing->setFunctionIndex(3);
//	vBitProcessing->addParamPattern(tempStr);
//
//	strcpy(tempStr, "{");
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"%sul_nb_cbs_user=ul_nb_cbs_user%d",tempStr,i);
//		if(i != N-1){
//			strcat(tempStr, ",");
//		}
//	}
//	strcat(tempStr, "}");
//
//	vBitProcessing->addParamPattern(tempStr);
//
//	CSDAGVertex* vTCP3dec = graph->addVertex("TCP3dec");
//	vTCP3dec->setFunctionIndex(4);
//
//	CSDAGVertex* vCRCCheck2 = graph->addVertex("CRCCheck2");
//	vCRCCheck2->setFunctionIndex(5);
//	vCRCCheck2->addParamPattern(tempStr);
//
//	vConvergence->addTiming(0,"1"); // Faraday c64x+
//	vConvergence->addAllConstraints(1, archi->getNbSlaves());
//
//	vKeepCurrentUserTones->addTiming(0,"1"); // Faraday c64x+
//	vKeepCurrentUserTones->addAllConstraints(1, archi->getNbSlaves());
//
//	vfOffsetConstDemDecDeint->addTiming(0,"40000"); // Faraday c64x+
//	vfOffsetConstDemDecDeint->addAllConstraints(1, archi->getNbSlaves());
//
//	vBitProcessing->addTiming(0,"13000"); // Faraday c64x+
//	vBitProcessing->addAllConstraints(1, archi->getNbSlaves());
//
//	vTCP3dec->addTiming(0,"38000"); // Faraday c64x+
//	vTCP3dec->addAllConstraints(1, archi->getNbSlaves());
//
//	vCRCCheck2->addTiming(0,"2700"); // Faraday c64x+ // 2700
//	vCRCCheck2->addAllConstraints(1, archi->getNbSlaves());
//
//	// Edges
//	graph->addEdge(vConvergence,"ul_max_tones_per_user*ul_nb_user",vKeepCurrentUserTones,"ul_max_tones_per_user");
//	graph->addEdge(vKeepCurrentUserTones,"ul_tones_user",vfOffsetConstDemDecDeint,"ul_tones_user");
//	graph->addEdge(vfOffsetConstDemDecDeint,"ul_tones_user*4",vBitProcessing,"12*4");
//	graph->addEdge(vBitProcessing,"(3*ul_block_size)+12",vTCP3dec,"(3*ul_block_size)+12");
//	graph->addEdge(vTCP3dec,"ul_block_size",vCRCCheck2,"ul_block_size*ul_nb_cbs_user");
//}
//
//void createGraphUplinkPerfTest(CSDAGGraph* graph, Architecture* archi){
//
//	int N=50;
//
//	static char tempStr[4000];
//
//	// Variables
//
//	globalParser.addVariable("ul_max_tones_per_user",1);
//	globalParser.addVariable("ul_nb_cb",50);
//	globalParser.addVariable("ul_nb_user",N);
//
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"ul_tones_user%d",i);
//		globalParser.addVariable(tempStr,1*12);
//		sprintf(tempStr,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(tempStr,1);
//	}
///*
//	for(int i=2; i<6; i++){
//		sprintf(tempStr,"ul_tones_user%d",i);
//		globalParser.addVariable(tempStr,5*12);
//		sprintf(tempStr,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(tempStr,5);
//	}
//
//	for(int i=6; i<12; i++){
//		sprintf(tempStr,"ul_tones_user%d",i);
//		globalParser.addVariable(tempStr,2*12);
//		sprintf(tempStr,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(tempStr,2);
//	}
//
//	for(int i=12; i<50; i++){
//		sprintf(tempStr,"ul_tones_user%d",i);
//		globalParser.addVariable(tempStr,1*12);
//		sprintf(tempStr,"ul_nb_cbs_user%d",i);
//		globalParser.addVariable(tempStr,1);
//	}
//*/
//	globalParser.addVariable("ul_block_size",200);
//	globalParser.addVariable("defaultTime",100);
//
//	// Adding a parameter pattern to a vertex: the parameters will be solved in the order of instance calls
//	// and their value passed to the actor in the order they have been added. Moreover, assigning
//	// a variable in the pattern means this variable value will be interpreted before production
//	// and consumption patterns are evaluated (ul_tones_user can be different for each firing of an actor).
//	strcpy(tempStr, "{");
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"%sul_tones_user=ul_tones_user%d",tempStr,i);
//		if(i != N-1){
//			strcat(tempStr, ",");
//		}
//	}
//	strcat(tempStr, "}");
//
//	// Vertices
//	// The functions are retrieved from the table named "functionPointers" with the index set here
//	CSDAGVertex* vConvergence = graph->addVertex("Convergence");
//	vConvergence->setFunctionIndex(0);
//
//	CSDAGVertex* vKeepCurrentUserTones = graph->addVertex("KeepCurrentUserTones");
//	vKeepCurrentUserTones->setFunctionIndex(1);
//	vKeepCurrentUserTones->addParamPattern(tempStr); // Adding ul_tones_user=ul_tones_user%d
//
//	CSDAGVertex* vfOffsetConstDemDecDeint = graph->addVertex("fOffsetConstDemDecDeint");
//	vfOffsetConstDemDecDeint->setFunctionIndex(2);
//	vfOffsetConstDemDecDeint->addParamPattern(tempStr);
//
//	CSDAGVertex* vBitProcessing = graph->addVertex("BitProcessing");
//	vBitProcessing->setFunctionIndex(3);
//	vBitProcessing->addParamPattern(tempStr);
//
//	strcpy(tempStr, "{");
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"%sul_nb_cbs_user=ul_nb_cbs_user%d",tempStr,i);
//		if(i != N-1){
//			strcat(tempStr, ",");
//		}
//	}
//	strcat(tempStr, "}");
//
//	vBitProcessing->addParamPattern(tempStr);
//
//	CSDAGVertex* vTCP3dec = graph->addVertex("TCP3dec");
//	vTCP3dec->setFunctionIndex(4);
//
//	CSDAGVertex* vCRCCheck2 = graph->addVertex("CRCCheck2");
//	vCRCCheck2->setFunctionIndex(5);
//	vCRCCheck2->addParamPattern(tempStr);
//
//	// Timings -> retrieved from Preesm LTE_1Cell_TPIC_2Iters uplink project. In thousands of cycles
//	/*
//	vStaticPart->addTiming(0,"1"); // Faraday c64x+ (1 GHz)
//	vConvergence->addTiming(0,"1"); // Faraday c64x+
//	vKeepCurrentUserTones->addTiming(0,"1"); // Faraday c64x+
//	vfOffsetConstDemDecDeint->addTiming(0,"10"); // Faraday c64x+
//	vBitProcessing->addTiming(0,"5"); // Faraday c64x+
//	vTCP3dec->addTiming(0,"10"); // Faraday c64x+
//	vCRCCheck2->addTiming(0,"1"); // Faraday c64x+
//*/
//	vConvergence->addTiming(0,"1"); // Faraday c64x+
//	vConvergence->addAllConstraints(1, archi->getNbSlaves());
//
//	vKeepCurrentUserTones->addTiming(0,"1"); // Faraday c64x+
//	vKeepCurrentUserTones->addAllConstraints(1, archi->getNbSlaves());
//
//	vfOffsetConstDemDecDeint->addTiming(0,"40000"); // Faraday c64x+
//	vfOffsetConstDemDecDeint->addAllConstraints(1, archi->getNbSlaves());
//
//	vBitProcessing->addTiming(0,"13000"); // Faraday c64x+
//	vBitProcessing->addAllConstraints(1, archi->getNbSlaves());
//
//	vTCP3dec->addTiming(0,"38000"); // Faraday c64x+
//	vTCP3dec->addAllConstraints(1, archi->getNbSlaves());
//
//	vCRCCheck2->addTiming(0,"2700"); // Faraday c64x+ // 2700
//	vCRCCheck2->addAllConstraints(1, archi->getNbSlaves());
//
//	// Edges
//	graph->addEdge(vConvergence,"ul_max_tones_per_user*ul_nb_user",vKeepCurrentUserTones,"ul_max_tones_per_user");
//	graph->addEdge(vKeepCurrentUserTones,"ul_tones_user",vfOffsetConstDemDecDeint,"ul_tones_user");
//	graph->addEdge(vfOffsetConstDemDecDeint,"ul_tones_user*4",vBitProcessing,"12*4");
//	graph->addEdge(vBitProcessing,"(3*ul_block_size)+12",vTCP3dec,"(3*ul_block_size)+12");
//	graph->addEdge(vTCP3dec,"ul_block_size",vCRCCheck2,"ul_block_size*ul_nb_cbs_user");
//}
//
//void createGraphUplinkSimpleTest(CSDAGGraph* graph, Architecture* archi){
//
//	int N=3;
//
//	static char tempStr[4000];
//
//	// Variables
//
//	globalParser.addVariable("ul_max_tones_per_user",1);
//	globalParser.addVariable("ul_nb_cb",100);
//	globalParser.addVariable("ul_nb_user",N);
//
//	globalParser.addVariable("ul_tones_user0",36);
//	globalParser.addVariable("ul_tones_user1",24);
//	globalParser.addVariable("ul_tones_user2",12);
//	globalParser.addVariable("ul_nb_cbs_user0",3);
//	globalParser.addVariable("ul_nb_cbs_user1",2);
//	globalParser.addVariable("ul_nb_cbs_user2",1);
//
//
//	globalParser.addVariable("ul_block_size",200);
//	globalParser.addVariable("defaultTime",100);
//
//	// Adding a parameter pattern to a vertex: the parameters will be solved in the order of instance calls
//	// and their value passed to the actor in the order they have been added. Moreover, assigning
//	// a variable in the pattern means this variable value will be interpreted before production
//	// and consumption patterns are evaluated (ul_tones_user can be different for each firing of an actor).
//	strcpy(tempStr, "{");
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"%sul_tones_user=ul_tones_user%d",tempStr,i);
//		if(i != N-1){
//			strcat(tempStr, ",");
//		}
//	}
//	strcat(tempStr, "}");
//
//	// Vertices
//	// The functions are retrieved from the table named "functionPointers" with the index set here
//	CSDAGVertex* vConvergence = graph->addVertex("Convergence");
//	vConvergence->setFunctionIndex(0);
//
//	CSDAGVertex* vKeepCurrentUserTones = graph->addVertex("KeepCurrentUserTones");
//	vKeepCurrentUserTones->setFunctionIndex(1);
//	vKeepCurrentUserTones->addParamPattern(tempStr); // Adding ul_tones_user=ul_tones_user%d
//
//	CSDAGVertex* vfOffsetConstDemDecDeint = graph->addVertex("fOffsetConstDemDecDeint");
//	vfOffsetConstDemDecDeint->setFunctionIndex(2);
//	vfOffsetConstDemDecDeint->addParamPattern(tempStr);
//
//	CSDAGVertex* vBitProcessing = graph->addVertex("BitProcessing");
//	vBitProcessing->setFunctionIndex(3);
//	vBitProcessing->addParamPattern(tempStr);
//
//	strcpy(tempStr, "{");
//	for(int i=0; i<N; i++){
//		sprintf(tempStr,"%sul_nb_cbs_user=ul_nb_cbs_user%d",tempStr,i);
//		if(i != N-1){
//			strcat(tempStr, ",");
//		}
//	}
//	strcat(tempStr, "}");
//
//	vBitProcessing->addParamPattern(tempStr);
//
//	CSDAGVertex* vTCP3dec = graph->addVertex("TCP3dec");
//	vTCP3dec->setFunctionIndex(4);
//
//	CSDAGVertex* vCRCCheck2 = graph->addVertex("CRCCheck2");
//	vCRCCheck2->setFunctionIndex(5);
//	vCRCCheck2->addParamPattern(tempStr);
//
//
//	vConvergence->addTiming(0,"1000"); // Faraday c64x+
//	vConvergence->addAllConstraints(1, archi->getNbSlaves());
//
//	vKeepCurrentUserTones->addTiming(0,"1000"); // Faraday c64x+
//	vKeepCurrentUserTones->addAllConstraints(1, archi->getNbSlaves());
//
//	vfOffsetConstDemDecDeint->addTiming(0,"40000"); // Faraday c64x+
//	vfOffsetConstDemDecDeint->addAllConstraints(1, archi->getNbSlaves());
//
//	vBitProcessing->addTiming(0,"13000"); // Faraday c64x+
//	vBitProcessing->addAllConstraints(1, archi->getNbSlaves());
//
//	vTCP3dec->addTiming(0,"38000"); // Faraday c64x+
//	vTCP3dec->addAllConstraints(1, archi->getNbSlaves());
//
//	vCRCCheck2->addTiming(0,"2700"); // Faraday c64x+ // 2700
//	vCRCCheck2->addAllConstraints(1, archi->getNbSlaves());
//
//	// Edges
//	graph->addEdge(vConvergence,"ul_max_tones_per_user*ul_nb_user",vKeepCurrentUserTones,"ul_max_tones_per_user");
//	graph->addEdge(vKeepCurrentUserTones,"ul_tones_user",vfOffsetConstDemDecDeint,"ul_tones_user");
//	graph->addEdge(vfOffsetConstDemDecDeint,"ul_tones_user*4",vBitProcessing,"12*4");
//	graph->addEdge(vBitProcessing,"(3*ul_block_size)+12",vTCP3dec,"(3*ul_block_size)+12");
//	graph->addEdge(vTCP3dec,"ul_block_size",vCRCCheck2,"ul_block_size*ul_nb_cbs_user");
//}
//
///**
// Scheduling the max uplink case on an architecture with 2 cores of a Faraday
// and a decreasing number of Tomahawk cores
//
// @param scheduler: the scheduler object
// @param csGraph: the reference CSDAG graph containing timings and constraints
// @param archi: the target architecture
//*//*
//void testArchis(TransformerScheduler* scheduler, CSDAGGraph* csDag, Architecture* archi)
//{
//	createArchiFaradayTomahawk(archi);
//	createGraphUplink100Max(csDag,archi);
//
//	while(archi->getNbSlaves() > 0){
//		scheduler->generateAndSchedule(csDag, archi);
//
//		archi->removeLastSlave();
//	}
//}
//*/
///**
// Scheduling one fixed graph
//
// @param scheduler: the scheduler object
// @param csGraph: the reference CSDAG graph containing timings and constraints
// @param archi: the target architecture
//*//*
//void testFixedGraph(TransformerScheduler* scheduler, CSDAGGraph* csDag, Architecture* archi)
//{
//	createSimpleArchi(archi, 7);
//	createGraphUplink100Max(csDag,archi);
//
//	setRandomParamsUplinkFixed();
//
//	scheduler->generateAndSchedule(csDag, archi);
//}
//*/
///**
// Scheduling graphs with increasing size on an architecture with 2 cores of a Faraday and a Tomahawk
//
// @param scheduler: the scheduler object
// @param csGraph: the reference CSDAG graph containing timings and constraints
// @param archi: the target architecture
//*//*
//void testGraphSize(TransformerScheduler* scheduler, CSDAGGraph* csDag, Architecture* archi)
//{
//	createArchiFaradayTomahawk(archi);
//	createGraphUplink100Max(csDag,archi);
//
//	while(1){
//		scheduler->generateAndSchedule(csDag, archi);
//
//		testAllParamsUplink100Max();
//	}
//}
//*/

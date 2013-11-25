/*
 * amTest.cpp
 *
 *  Created on: Jun 17, 2013
 *      Author: jheulot
 */


/*
 * cmpFrontTests.cpp
 *
 *  Created on: Jan 8, 2013
 *      Author: jheulot
 */

#include "applicationGraphs/testAM.h"
#include "ahsTests.h"
#include "scheduling/FlowShopScheduler/FlowShopScheduler.h"
#include "scheduling/FlowShopScheduler/processorMapper.h"
#include "scheduling/ListScheduler/ListScheduler.h"
#include "transformations/JobTransformer/JobTransformer.h"
#include "transformations/CSDAGTransformer/CSDAGTransformer.h"
#include "tools/DotWriter.h"
#include "tools/ScheduleWriter.h"
#include "tools/ScheduleChecker.h"
#include "tools/Sdf3Parser.h"
#include "graphs/CSDAG/CSDAGGraph.h"
#include "graphs/ActorMachine/AMGraph.h"
#include "graphs/JobSet/JobSet.h"
#include "graphs/Schedule/Schedule.h"
#include "ExecutionStat.h"

#include <hwQueues.h>
#include <Timer.h>

#include "launcher/launcher.h"

#include <iostream>
#include <fstream>

#ifndef DESKTOP
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#define CSDAG_FILE_PATH 			"csdag.gv"
#define STRUCTURE_FILE_PATH 		"srDag.gv"
#define GANTT_FLOW_FILE_PATH 		"ganttEventsFlow.xml"
#define GANTT_LIST_FILE_PATH 		"ganttEventsList.xml"
#define MATLAB_SPEEDUPS_FILE_PATH 	"speedups.dat"
#define MATLAB_SPANS_FILE_PATH 		"spans.dat"
#define MATLAB_WORKS_FILE_PATH 		"works.dat"

static int width=720, height=400, nbSlices=25, color=0, nbActSlaves=1, nbMaxSlaves=0;
static int estimatedPerf=0, computationTime=0;
static ExecutionStat execStat;

void testComArgs(CSDAGGraph *csDag, Architecture* archi, launcher* launch){
	CSDAGTransformer 	csDAGTransformer;
	ListScheduler 		listScheduler;

	DotWriter 		dotWriter;
	ScheduleWriter 	schedWriter;
	ScheduleChecker scheduleChecker;

	SRDAGGraph srDag;
	Schedule schedule;

	srDag.flush();
	listScheduler.setArchitecture(archi);

	//	timer.resetAndStart();
	csDag->resolveTimings(archi);
	csDag->resolveEdgeTokenRates();
	csDag->resolveVerticesParams();

	dotWriter.write(csDag, CSDAG_FILE_PATH, 1);

	csDAGTransformer.transform(csDag, &srDag, archi);
	//	execStat.srDAGTransfTime = timer.getValueAndReset();

	//	execStat.t1Latency = srDagList.getMaxTime();
	//	execStat.criticalPath = srDagList.getCriticalPath();
	dotWriter.write(&srDag, STRUCTURE_FILE_PATH, 1);

	execStat.nbSRDAGVertices = srDag.getNbVertices();
	execStat.nbSRDAGEdges = srDag.getNbEdges();

	//	timer.resetAndStart();
	listScheduler.schedule(&srDag, &schedule, archi);
	//	execStat.listMakespan = listScheduler.getMakespan(&srDagList);
	//	execStat.listThroughput = listScheduler.getThroughput(&srDagList);
	//	execStat.listScheduleTime = timer.getValueAndReset();
	//
	schedWriter.write(&schedule, &srDag, archi, "test.xml");
	//
	scheduleChecker.checkSchedule(&srDag, &schedule, archi);
	//

	launch->reset();
	launch->prepare(&srDag, archi, &schedule, &execStat);

//	return execStat;

}

void sobelArgs(CSDAGGraph *csDag, Architecture* archi, launcher* launch){
	CSDAGTransformer 	csDAGTransformer;
	ListScheduler 		listScheduler;

	DotWriter 		dotWriter;
	ScheduleWriter 	schedWriter;
	ScheduleChecker scheduleChecker;

	SRDAGGraph srDag;
	Schedule schedule;

	srDag.flush();
	listScheduler.setArchitecture(archi);

	//	timer.resetAndStart();
	csDag->resolveTimings(archi);
	csDag->resolveEdgeTokenRates();
	csDag->resolveVerticesParams();

	dotWriter.write(csDag, CSDAG_FILE_PATH, 1);

	csDAGTransformer.transform(csDag, &srDag, archi);
	//	execStat.srDAGTransfTime = timer.getValueAndReset();

	//	execStat.t1Latency = srDagList.getMaxTime();
	//	execStat.criticalPath = srDagList.getCriticalPath();
	dotWriter.write(&srDag, STRUCTURE_FILE_PATH, 1);

	execStat.nbSRDAGVertices = srDag.getNbVertices();
	execStat.nbSRDAGEdges = srDag.getNbEdges();
	execStat.nbFunction = csDag->getNbVertices()+1;

	//	timer.resetAndStart();
	listScheduler.schedule(&srDag, &schedule, archi);
	//	execStat.listMakespan = listScheduler.getMakespan(&srDagList);
	//	execStat.listThroughput = listScheduler.getThroughput(&srDagList);
	//	execStat.listScheduleTime = timer.getValueAndReset();
	//
	schedWriter.write(&schedule, &srDag, archi, "test.xml");
	//
	scheduleChecker.checkSchedule(&srDag, &schedule, archi);
	//

	launch->reset();
	launch->prepare(&srDag, archi, &schedule, &execStat);

//	return execStat;

}

static int dataChanged;

int printMenu(){
	if(dataChanged == 0) return 0;
	dataChanged = 0;

	clear();
	mvprintw(0,0,"Params:"
			"\t(1) Width     = %d\n"
			"\t(2) Height    = %d\n"
			"\t(3) Nb Slices = %d\n"
			"\t(4) Color     = %d\n"
			"\t(5) NbSlaves  = %d\n\n",
			width,height,nbSlices, color, nbActSlaves);
	printw("Stats: "
			"\tSRDAG Vertices number:     %d\n"
			"\tSRDAG Edges (Fifo) number: %d\n"
			"\tMemory Usage: %d bytes (%.1f Mbytes)\n"
			"\tEstim. Perf:  %d cycles (%d fps)\n"
//			"\tComp time:    %d cycles (%d ms)\n"
			"\tRunning Cores:    %d\n\n",
			execStat.nbSRDAGVertices, execStat.nbSRDAGEdges,
			execStat.memAllocated, execStat.memAllocated/1024.0/1024,
			execStat.listMakespan, 100000000/execStat.listMakespan,
//			computationTime, computationTime,
			execStat.nbRunningCore);
//	for(int i=0; i<nbActSlaves; i++){
//		printw( "Core %d"
//				"\tActor Machine (%d vertices, %d actions, %d conditions)\n"
//				"\tConfiguration message length: %d bytes (%.1f kbytes)\n\n",
//				i, execStat.nbAMVertices[i], execStat.nbAMActions[i],
//				execStat.nbAMConds[i], execStat.msgLength[i], execStat.msgLength[i]/1024.0);
//	}
	printw( "Activity:\n");
	int y = getcury(stdscr);

	int tab_y = y;
	mvprintw(tab_y++,0,"|-------------------|");
	mvprintw(tab_y++,0,  "|    Core index     |");
	mvprintw(tab_y++,0,"|-------------------|");
	mvprintw(tab_y++,0,"|    AM Vertices    |");
	mvprintw(tab_y++,0,"|    AM Actions     |");
	mvprintw(tab_y++,0,"|   AM Conditions   |");
	mvprintw(tab_y++,0,"| Msg Length (bytes)|");
	mvprintw(tab_y++,0,"|-------------------|");
	mvprintw(tab_y++,0,"|      Default      |");
	mvprintw(tab_y++,0,"|   AM Management   |");
	mvprintw(tab_y++,0,"|     Scheduling    |");
	mvprintw(tab_y++,0,"|     FifoCheck     |");
	mvprintw(tab_y++,0,"|   Data Transfert  |");
	mvprintw(tab_y++,0,"| CtrlFifo Handling |");
	for(int j=0; j<(int)execStat.nbFunction; j++){
		mvprintw(tab_y++,0,"|      Action %1.1d      |", j);
	}
	mvprintw(tab_y++,0,"|-------------------|");

	for(int i=0; i<nbActSlaves; i++){
		int my_y = y;
		int my_x = 20+i*9;
		mvprintw(my_y++,my_x,"|--------|");
		mvprintw(my_y++,my_x,"| Core %1d |",i);
		mvprintw(my_y++,my_x,"|--------|");
		mvprintw(my_y++,my_x,"|  %4d  |", execStat.nbAMVertices[i]);
		mvprintw(my_y++,my_x,"|  %4d  |", execStat.nbAMActions[i]);
		mvprintw(my_y++,my_x,"|  %4d  |", execStat.nbAMConds[i]);
		mvprintw(my_y++,my_x,"|  %4d  |", execStat.msgLength[i]);
		mvprintw(my_y++,my_x,"|--------|");
		mvprintw(my_y++,my_x,"|   %2d   |", execStat.timings[i][Default]);
		mvprintw(my_y++,my_x,"|   %2d   |", execStat.timings[i][AMManagement]);
		mvprintw(my_y++,my_x,"|   %2d   |", execStat.timings[i][Scheduling]);
		mvprintw(my_y++,my_x,"|   %2d   |", execStat.timings[i][FifoCheck]);
		mvprintw(my_y++,my_x,"|   %2d   |", execStat.timings[i][DataTransfert]);
		mvprintw(my_y++,my_x,"|   %2d   |", execStat.timings[i][CtrlFifoHandling]);
		for(int j=0; j<(int)execStat.nbFunction; j++){
			mvprintw(my_y++,my_x,"|   %2d   |", execStat.timings[i][Action+j]);
		}
		mvprintw(my_y++,my_x,"|--------|");
	}
	tab_y++;
//	static int ref=0;
//	mvprintw(tab_y+3,0,"Refresh %d\n",ref++);
	refresh();
	return tab_y;
}

void getActivity(){
	static int state[MAX_SLAVES];//, slave=0;
	UINT32 value;
	for(int slave=0; slave<(int)nbMaxSlaves; slave++){
		while(OS_InfoQPop_nonBlocking(slave, &value, sizeof(UINT32)) == sizeof(UINT32) ){
			execStat.timings[slave][state[slave]] = value;
			state[slave] = (state[slave]+1)%(6+execStat.nbFunction);
			if(state[slave] == 0){
				dataChanged=1;
//					slave=(slave+1)%execStat.nbRunningCore;
//					if(slave == 0) dataChanged=1;
//					break;
			}
		}
	}
}


void sobel(int nbslave){
	nbMaxSlaves = nbslave;
	dataChanged = 1;

	static int *editValues[5] = {&width, &height, &nbSlices, &color, &nbActSlaves};
	static int bounds[5][2] = {{720,720},{400,400},{25,400},{0,1},{1,nbMaxSlaves}};

	CSDAGGraph csDag;
	Architecture arch;
	launcher launch(nbMaxSlaves);
	DotWriter dotWriter;
	char c, quit = 0;
	int computed=0;

	// Edit vars
	int editState = 0;
	int rank = 0;
	int value;

	// Variables
	globalParser.addVariable("Width",width);
	globalParser.addVariable("Height",height);
	globalParser.addVariable("nbSlices",nbSlices);
	globalParser.addVariable("Color",color);

	createZynqArchi(&arch, nbMaxSlaves);
	createSobel(&csDag, &arch);

	arch.setNbActiveSlaves(nbActSlaves);
	dotWriter.write(&csDag, "csDag_var.gv", 0);

	sobelArgs(&csDag, &arch, &launch);
	launch.launch(nbActSlaves);
	execStat.nbRunningCore = nbActSlaves;

	initscr();			/* Start curses mode 		*/
	raw();
	noecho();
	nodelay(stdscr, 1);

	int y_comments;
	while(!quit){
		int tmp_y = printMenu();
		if(tmp_y!=0) y_comments = tmp_y;
//		static int tmp;
//		mvprintw(y_comments+1,0,"Edit state %d while %d\n", editState, tmp++);
//		refresh();

		/* Edit Menu */
		switch(editState){
		case 0: /* Edit Menu not Activated */
			mvprintw(y_comments,0,"Actions: (E)dit, (C)ompute, (L)aunch and (Q)uit\n");
			c = getch();
			switch(c){
			case 'E':
			case 'e':
				editState = 1;
				break;
			case 'C':
			case 'c':
				globalParser.addVariable("Width",width);
				globalParser.addVariable("Height",height);
				globalParser.addVariable("nbSlices",nbSlices);
				globalParser.addVariable("Color",color);
				arch.setNbActiveSlaves(nbActSlaves);
				sobelArgs(&csDag, &arch, &launch);
				printMenu();
				computed=1;
				dataChanged = 1;
				printw("Done\n");
				break;
			case 'L':
			case 'l':
				if(computed == 0){
					mvprintw(y_comments,0,"You must compute before launch\n");
				}else{
					launch.stop();
					launch.launch(nbActSlaves);
					execStat.nbRunningCore = nbActSlaves;
				}
				break;
			case 'Q':
			case 'q':
				quit = 1;
				break;
			}
			break;

		case 1: /* Print Edit Cmd */
			mvprintw(y_comments,0,"Which value do you want to edit: ((Q) to quit)");
			if((rank = getch()) != ERR){
				if(rank == 'q' || rank == 'Q'){
					editState = 0;
				}else if(rank >= '1' && rank <= '5'){
					rank -= '1';
					editState++;
					value = 0;
				}
			}
			break;

		case 2: /* Get value */
			mvprintw(rank,24,"         ");
			mvprintw(rank,24,"%d", value);
			c = getch();
			if(c != ERR){
				if(c == '\r' || c == '\n'){
					editState++;
				}else if(c >= '0' && c <= '9'){
					value*=10;
					value+=c-'0';
				}
			}
			break;

		case 3: /* Value check */
			if(value >= bounds[rank][0] && value <= bounds[rank][1]){
				if(rank==2 && height%value != 0){
					mvprintw(y_comments,0,"nbSlices must divide height");
					if(getch() != ERR) editState = 1;
				}else{
					*editValues[rank] = value;
					editState = 0;
					dataChanged = 1;
					break;
				}
			}else{
				mvprintw(y_comments,0,"Value must be in [%d,%d]", bounds[rank][0],  bounds[rank][1]);
				if(getch() != ERR) editState = 1;
			}
			break;
		}

		getActivity();
	}

	launch.stopWOCheck();
	endwin();
}

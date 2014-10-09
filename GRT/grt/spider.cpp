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

#include "spider.h"

#include <platform.h>
#include <execution/execution.h>
#include <transformations/PiSDFTransformer/PiSDFTransformer.h>
#include <tools/ScheduleWriter.h>
#include <grt_definitions.h>

static SRDAGGraph 			topDag;
static BaseSchedule 		schedule;
static int iteration = 0;

void SPIDER_init(Architecture* arch){
	platform_init(arch->getNbSlaves());
}

void SPIDER_reset(){
	iteration = 0;
	initExecution();
	platform_time_reset();
	Launcher::init();
	schedule.reset();
}

int getGlobalIteration(){
	return iteration;
}

void SPIDER_launch(Architecture* arch, PiSDFGraph* topPisdf){
	static ListScheduler 		listScheduler;

	listScheduler.reset();
	topDag.reset();
	Launcher::reset();
	topPisdf->resetRefs();

	listScheduler.setArchitecture(arch);

	// Add topActor to topDag
	topDag.createVertexNo(0, 0, (PiSDFVertex*)topPisdf->getVertex(0));

	PiSDFTransformer::multiStepScheduling(arch, topPisdf, &listScheduler, &schedule, &topDag);

	iteration++;
}


void SPIDER_report(Architecture* arch, PiSDFGraph* topPisdf, ExecutionStat* execStat, int iter){
	memset(execStat, 0, sizeof(ExecutionStat));

	char file[MAX_FILE_NAME_SIZE+40];
		snprintf(file, MAX_FILE_NAME_SIZE+40, "Gantt_spider_cache_nvar9.xml");
		Launcher::createRealTimeGantt(arch, &topDag, file, execStat, true);
		execStat->SRDAGVertices = topDag.getNbVertices();
		execStat->SRDAGEdges = topDag.getNbEdges();
	//	snprintf(file, MAX_FILE_NAME_SIZE+40, "/home/jheulot/dev/mp-sched/ederc/simu%d.xml", iter);
	//	ScheduleWriter::write(&schedule, &topDag, arch, file);
	#if STAT
		printAlloc();
	#endif
}


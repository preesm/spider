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

#include <string.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include "debuggingOptions.h"

static PiSDFGraph graphs[MAX_NB_PiSDF_SUB_GRAPHS];
static UINT8 nb_graphs = 0;

void clearAllGraphs(){
	for(int i=0; i<MAX_NB_PiSDF_SUB_GRAPHS; i++)
		graphs[i].reset();
}

void PiSDFSimpleLoop(PiSDFGraph* graph, PiSDFAbstractVertex* parentVertex, Scenario* scenario, UINT32 NMAX){
	// Parameters.
	PiSDFParameter *paramN = graph->addParameter("N");
	PiSDFParameter *paramNMAX = graph->addParameter("NMAX");

	paramNMAX->setValue(NMAX);

#if EXEC == 0
	paramN->setValue(NMAX);
#endif

	// Configure vertices.
	PiSDFConfigVertex *vxReadFile = (PiSDFConfigVertex *)graph->addVertex("ReadFile", config_vertex);
	vxReadFile->setFunction_index(0);
	vxReadFile->addRelatedParam(paramN);
	vxReadFile->addParameter(paramNMAX);
	graph->setRootVertex(vxReadFile);

	PiSDFVertex *vxInitNLoop 	= (PiSDFVertex *)graph->addVertex("InitNLoop", pisdf_vertex);
	vxInitNLoop->addParameter(paramN);
	vxInitNLoop->addParameter(paramNMAX);
	vxInitNLoop->setFunction_index(1);

	PiSDFVertex *vxF 	= (PiSDFVertex *)graph->addVertex("F", pisdf_vertex);
	vxF->setFunction_index(2);

	PiSDFVertex *vxEndNLoop 	= (PiSDFVertex *)graph->addVertex("EndNLoop", pisdf_vertex);
	vxEndNLoop->addParameter(paramN);
	vxEndNLoop->addParameter(paramNMAX);
	vxEndNLoop->setFunction_index(3);

	PiSDFVertex *vxWriteFile 	= (PiSDFVertex *)graph->addVertex("WriteFile", pisdf_vertex);
	vxWriteFile->addParameter(paramNMAX);
	vxWriteFile->setFunction_index(4);

	// Edges.
	graph->addEdge(vxReadFile, 0, "NMAX", vxInitNLoop, 0, "NMAX", "0");
	
	graph->addEdge(vxInitNLoop, 0, "N", vxF, 0, "1", "0");

	graph->addEdge(vxF, 0, "1", vxEndNLoop, 0, "N", "0");

	graph->addEdge(vxEndNLoop, 0, "NMAX", vxWriteFile, 0, "NMAX", "0");

	// Timings
	scenario->setTiming(vxReadFile->getId(), 1, "100");
	scenario->setTiming(vxInitNLoop->getId(), 1, "100");
	scenario->setTiming(vxF->getId(), 1, "100");
	scenario->setTiming(vxEndNLoop->getId(), 1, "100");
	scenario->setTiming(vxWriteFile->getId(), 1, "100");

//	// Constraints
//	scenario->setConstraints(vxReadFile->getId(), 0);
//	scenario->setConstraints(roundB_0->getId(), 0);
//	scenario->setConstraints(roundB_1->getId(), 0);
//	scenario->setConstraints(vxBroad->getId(), 0);
//	scenario->setConstraints(vxInitNLoop->getId(), 0);
//	scenario->setConstraints(vxEndNLoop->getId(), 0);
//	scenario->setConstraints(vxWriteFile->getId(), 0);

}


void top(PiSDFGraph* graph, Scenario* scenario, UINT32 NMAX){
	nb_graphs = 0;
	PiSDFVertex *vxSimpleLoop = (PiSDFVertex *)graph->addVertex("SimpleLoop", pisdf_vertex);

	graph->setRootVertex(vxSimpleLoop);

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *SimpleLoop_subGraph = &graphs[nb_graphs]; nb_graphs++;
	PiSDFSimpleLoop(SimpleLoop_subGraph, vxSimpleLoop, scenario, NMAX);
	vxSimpleLoop->setSubGraph(SimpleLoop_subGraph);
}

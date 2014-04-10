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

void MLoop(PiSDFGraph* graph, PiSDFAbstractVertex* parentVertex, Scenario* scenario, UINT32 MMAX){
	// Parameters.
	PiSDFParameter *paramM = graph->addParameter("M");
	PiSDFParameter *paramMMAX = graph->addParameter("MMAX");

	paramMMAX->setValue(MMAX);
#if EXEC == 0
	paramM->setValue(3);
#endif

	// Interface vertices.
	PiSDFIfVertex *vxM_in = (PiSDFIfVertex*)graph->addVertex("M_in", input_vertex);
	vxM_in->setDirection(0);
	vxM_in->setParentVertex(parentVertex);
	vxM_in->setParentEdge(parentVertex->getInputEdge(0));
	vxM_in->setFunction_index(10);

	PiSDFIfVertex *vxLine_in = (PiSDFIfVertex*)graph->addVertex("Line_in", input_vertex);
	vxLine_in->setDirection(0);
	vxLine_in->addParameter(paramMMAX);
	vxLine_in->setParentVertex(parentVertex);
	vxLine_in->setParentEdge(parentVertex->getInputEdge(1));
	vxLine_in->setFunction_index(10);

	PiSDFIfVertex *vxLine_out = (PiSDFIfVertex*)graph->addVertex("Line_out", output_vertex);
	vxLine_out->setDirection(1);
	vxLine_out->addParameter(paramMMAX);
	vxLine_out->setParentVertex(parentVertex);
	vxLine_out->setParentEdge(parentVertex->getOutputEdge(0));
	vxLine_out->setFunction_index(10);


	// Configuration vertices
	PiSDFConfigVertex *vxConfigM = (PiSDFConfigVertex *)graph->addVertex("ConfigM", config_vertex);
	vxConfigM->setFunction_index(5);
	vxConfigM->addRelatedParam(paramM);


	// Others..
	PiSDFVertex *vxInitMLoop = (PiSDFVertex*)graph->addVertex("InitMLoop", pisdf_vertex);
	vxInitMLoop->setFunction_index(6);
	vxInitMLoop->addParameter(paramMMAX);
	vxInitMLoop->addParameter(paramM);

	PiSDFVertex *vxF = (PiSDFVertex *)graph->addVertex("F", pisdf_vertex);
	vxF->setFunction_index(7);

	PiSDFVertex *vxEndMLoop = (PiSDFVertex*)graph->addVertex("EndMLoop", pisdf_vertex);
	vxEndMLoop->setFunction_index(8);
	vxEndMLoop->addParameter(paramMMAX);
	vxEndMLoop->addParameter(paramM);


	// Edges.
	graph->addEdge(vxM_in, 0, "1", vxConfigM, 0, "1", "0");

	graph->addEdge(vxLine_in, 0, "MMAX", vxInitMLoop, 0, "MMAX", "0");

	graph->addEdge(vxInitMLoop, 0, "M", vxF, 0, "1", "0");

	graph->addEdge(vxF, 0, "1", vxEndMLoop, 0, "M", "0");

	graph->addEdge(vxEndMLoop, 0, "MMAX", vxLine_out, 0, "MMAX", "0");


	// Timings
	scenario->setTiming(vxM_in->getId(), 1, "100");
	scenario->setTiming(vxLine_in->getId(), 1, "100");
	scenario->setTiming(vxConfigM->getId(), 1, "100");
	scenario->setTiming(vxInitMLoop->getId(), 1, "100");
	scenario->setTiming(vxF->getId(), 1, "100");
	scenario->setTiming(vxEndMLoop->getId(), 1, "100");
	scenario->setTiming(vxLine_out->getId(), 1, "100");

//	// Constraints
//	scenario->setConstraints(vxM_in->getId(), 0);
//	scenario->setConstraints(vxLine_in->getId(), 0);
//	scenario->setConstraints(vxConfigM->getId(), 0);
//	scenario->setConstraints(vxInitMLoop->getId(), 0);
//	scenario->setConstraints(vxF->getId(), 0);
//	scenario->setConstraints(vxEndMLoop->getId(), 0);
//	scenario->setConstraints(vxLine_out->getId(), 0);
}


void PiSDFDoubleLoop(PiSDFGraph* graph, PiSDFAbstractVertex* parentVertex, Scenario* scenario, UINT32 NMAX, UINT32 MMAX){
	// Parameters.
	PiSDFParameter *paramN = graph->addParameter("N");
	PiSDFParameter *paramNMAX = graph->addParameter("NMAX");
	PiSDFParameter *paramMMAX = graph->addParameter("MMAX");

	paramNMAX->setValue(NMAX);
	paramMMAX->setValue(MMAX);

#if EXEC == 0
	paramN->setValue(3);
#endif

	// Configure vertices.
	PiSDFConfigVertex *vxReadFile = (PiSDFConfigVertex *)graph->addVertex("ReadFile", config_vertex);
	vxReadFile->setFunction_index(0);
	vxReadFile->addRelatedParam(paramN);
	vxReadFile->addParameter(paramNMAX);
	vxReadFile->addParameter(paramMMAX);
	graph->setRootVertex(vxReadFile);

	PiSDFVertex *vxInitNLoop 	= (PiSDFVertex *)graph->addVertex("InitNLoop", pisdf_vertex);
	vxInitNLoop->addParameter(paramN);
	vxInitNLoop->addParameter(paramNMAX);
	vxInitNLoop->addParameter(paramMMAX);
	vxInitNLoop->setFunction_index(2);

	PiSDFVertex *vxMLoop 		= (PiSDFVertex *)graph->addVertex("MLoop", pisdf_vertex);

	PiSDFVertex *vxEndNLoop 	= (PiSDFVertex *)graph->addVertex("EndNLoop", pisdf_vertex);
	vxEndNLoop->addParameter(paramN);
	vxEndNLoop->addParameter(paramMMAX);
	vxEndNLoop->setFunction_index(3);

	PiSDFVertex *vxWriteFile 	= (PiSDFVertex *)graph->addVertex("WriteFile", pisdf_vertex);
	vxWriteFile->addParameter(paramNMAX);
	vxWriteFile->addParameter(paramMMAX);
	vxWriteFile->setFunction_index(4);

	// Edges.
	graph->addEdge(vxReadFile, 0, "NMAX", vxInitNLoop, 0, "NMAX", "0");
	graph->addEdge(vxReadFile, 1, "NMAX*MMAX", vxInitNLoop, 1,  "NMAX*MMAX", "0");
	
	graph->addEdge(vxInitNLoop, 0, "N", vxMLoop, 0, "1", "0");
	graph->addEdge(vxInitNLoop, 1, "N*MMAX", vxMLoop, 1, "MMAX", "0");

	graph->addEdge(vxMLoop, 0, "MMAX", vxEndNLoop, 0, "N*MMAX", "0");

	graph->addEdge(vxEndNLoop, 0, "NMAX*MMAX", vxWriteFile, 0, "NMAX*MMAX", "0");

	// Timings
	scenario->setTiming(vxReadFile->getId(), 1, "100");
	scenario->setTiming(vxInitNLoop->getId(), 1, "100");
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


	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *MLoop_subGraph = &graphs[nb_graphs]; nb_graphs++;
	MLoop(MLoop_subGraph, vxMLoop, scenario, MMAX);
	vxMLoop->setSubGraph(MLoop_subGraph);
}


void top(PiSDFGraph* graph, Scenario* scenario, UINT32 NMAX, UINT32 MMAX){
	nb_graphs = 0;
	PiSDFVertex *vxDoubleLoop = (PiSDFVertex *)graph->addVertex("DoubleLoop", pisdf_vertex);

	graph->setRootVertex(vxDoubleLoop);

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *DoubleLoop_subGraph = &graphs[nb_graphs]; nb_graphs++;
	PiSDFDoubleLoop(DoubleLoop_subGraph, vxDoubleLoop, scenario, NMAX, MMAX);
	vxDoubleLoop->setSubGraph(DoubleLoop_subGraph);
}

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

void PiSDFGnuRadio_sub(PiSDFGraph* graph, PiSDFAbstractVertex* parentVertex){
	// Parameters.
	graph->setParentVertex(parentVertex);

	PiSDFParameter *paramM = graph->addParameter("M");

#if EXEC == 0
	paramM->setValue(2);
#endif
	// Interface vertices.
	PiSDFIfVertex *vxIn = (PiSDFIfVertex*)graph->addVertex("in", input_vertex);
	vxIn->setDirection(0);
	vxIn->setParentVertex(parentVertex);
	vxIn->setParentEdge(parentVertex->getInputEdge(0));
	vxIn->setFunction_index(10);

	PiSDFIfVertex *vxOut = (PiSDFIfVertex*)graph->addVertex("out", output_vertex);
	vxOut->setDirection(1);
	vxOut->addParameter(paramM);
	vxOut->setParentVertex(parentVertex);
	vxOut->setParentEdge(parentVertex->getOutputEdge(0));
	vxOut->setFunction_index(10);

	// Configure vertices.
	PiSDFConfigVertex *vxCfgM = (PiSDFConfigVertex *)graph->addVertex("cfgM", config_vertex);
	vxCfgM->setFunction_index(3);
	vxCfgM->addRelatedParam(paramM);
	graph->setRootVertex(vxCfgM);

	// Other vertices
	PiSDFVertex *vxInitM 	= (PiSDFVertex *)graph->addVertex("InitM", pisdf_vertex);
	vxInitM->addParameter(paramM);
	vxInitM->setFunction_index(4);

	PiSDFVertex *vxSwitch 	= (PiSDFVertex *)graph->addVertex("Switch", pisdf_vertex);
	vxSwitch->addParameter(paramM);
	vxSwitch->setFunction_index(5);

	PiSDFVertex *vxFir 	= (PiSDFVertex *)graph->addVertex("FIR", pisdf_vertex);
	vxSwitch->addParameter(paramM);
	vxSwitch->setFunction_index(5);

	PiSDFVertex *vxBroadCast = (PiSDFVertex *)graph->addVertex("Br", pisdf_vertex);
	vxSwitch->setFunction_index(11);

	// Edges.
	graph->addEdge(vxInitM, 	0, "M", vxSwitch, 		0, "1", "0");
	graph->addEdge(vxIn, 		0, "1", vxSwitch, 		1, "1", "0");
	graph->addEdge(vxBroadCast, 0, "1", vxSwitch, 		2, "1", "1");

	graph->addEdge(vxSwitch, 	0, "1", vxFir, 			0, "1", "0");
	graph->addEdge(vxFir, 		0, "1", vxBroadCast, 	0, "1", "0");
	graph->addEdge(vxBroadCast, 1, "1", vxOut, 			0, "1", "0");

	// Timings
	vxInitM->setTiming(0, "1");
	vxSwitch->setTiming(0, "1");
	vxFir->setTiming(0, "10");
	vxBroadCast->setTiming(0, "1");
}

void PiSDFGnuRadio(PiSDFGraph* graph, PiSDFAbstractVertex* parentVertex){
	graph->setParentVertex(parentVertex);

	// Parameters.
	PiSDFParameter *paramN = graph->addParameter("N");

#if EXEC == 0
	paramN->setValue(2);
#endif


	// Configure vertices.
	PiSDFConfigVertex *vxCfgN = (PiSDFConfigVertex *)graph->addVertex("CfgN", config_vertex);
	vxCfgN->setFunction_index(0);
	vxCfgN->addRelatedParam(paramN);
	graph->setRootVertex(vxCfgN);

	// Regular vertices
	PiSDFConfigVertex *vxSrc = (PiSDFConfigVertex *)graph->addVertex("src", config_vertex);
	vxSrc->setFunction_index(1);
	vxSrc->addParameter(paramN);

	PiSDFVertex *vxFirLine 	= (PiSDFVertex *)graph->addVertex("FirLine", pisdf_vertex);
	vxFirLine->addParameter(paramN);

	PiSDFVertex *vxSnk = (PiSDFVertex *)graph->addVertex("snk", pisdf_vertex);
	vxSnk->setFunction_index(2);
	vxSnk->addParameter(paramN);

	// Edges.
	graph->addEdge(vxSrc, 0, "N", vxFirLine, 0, "1", "0");
	graph->addEdge(vxFirLine, 0, "1", vxSnk, 0, "N", "0");

	// Timings
	vxCfgN->setTiming(0, "1");
	vxSrc->setTiming(0, "1");
	vxSnk->setTiming(0, "1");

//	// Constraints
//	scenario->setConstraints(vxReadFile->getId(), 0);
//	scenario->setConstraints(roundB_0->getId(), 0);
//	scenario->setConstraints(roundB_1->getId(), 0);
//	scenario->setConstraints(vxBroad->getId(), 0);
//	scenario->setConstraints(vxInitNLoop->getId(), 0);
//	scenario->setConstraints(vxEndNLoop->getId(), 0);
//	scenario->setConstraints(vxWriteFile->getId(), 0);

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS) exitWithCode(1054);
	PiSDFGraph *GnuRadio_subGraph = &graphs[nb_graphs++];
	GnuRadio_subGraph->setBaseId(20);
	PiSDFGnuRadio_sub(GnuRadio_subGraph, vxFirLine);
	vxFirLine->setSubGraph(GnuRadio_subGraph);
}


void top(PiSDFGraph* graph, /*Scenario* scenario,*/ UINT32 height, UINT32 width, UINT32 nbSlicesMax_median){
	nb_graphs = 0;
	PiSDFVertex *vxTop = (PiSDFVertex *)graph->addVertex("top", pisdf_vertex);
	graph->setRootVertex(vxTop);

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *GnuRadio_graph = &graphs[nb_graphs]; nb_graphs++;
	GnuRadio_graph->setBaseId(10);
	PiSDFGnuRadio(GnuRadio_graph, vxTop);
	vxTop->setSubGraph(GnuRadio_graph);
}

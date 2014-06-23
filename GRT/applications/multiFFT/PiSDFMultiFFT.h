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

static PiSDFGraph* graphs;
static int nbGraphs = 0;

void multiFFT(PiSDFGraph* graph);

PiSDFGraph* addGraph() {
	if(nbGraphs >= MAX_NB_PiSDF_GRAPHS) exitWithCode(1054);
	PiSDFGraph* graph = &(graphs[nbGraphs++]);
	graph->reset();
	return graph;
}

/**
 * This is the method you need to call to build a complete PiSDF graph.
 */
PiSDFGraph* top(PiSDFGraph* _graphs){
	graphs = _graphs;
	PiSDFGraph* top = addGraph();
	PiSDFVertex* vxTop = (PiSDFVertex *)top->addVertex("vxTop", pisdf_vertex);
	PiSDFGraph *multiFFT_subGraph = addGraph();
	vxTop->setSubGraph(multiFFT_subGraph);
	multiFFT_subGraph->setParentVertex(vxTop);
	multiFFT(multiFFT_subGraph);
	return top;
}

// Method building PiSDFGraph multiFFT
void multiFFT(PiSDFGraph* graph){
	//Parameters
	PiSDFParameter *param_NC = graph->addParameter("NC");
	param_NC->setValue(256.0);

	PiSDFParameter *param_NR = graph->addParameter("NR");
	param_NR->setValue(256.0);

	//Vertices
	PiSDFVertex *vxReOrder1 = (PiSDFVertex*)graph->addVertex("ReOrder1",pisdf_vertex);
	vxReOrder1->addParameter(param_NC);
	vxReOrder1->addParameter(param_NR);
	vxReOrder1->setTiming(0, "10000");
	vxReOrder1->setFunction_index(1);

	PiSDFVertex *vxFFT1 = (PiSDFVertex*)graph->addVertex("FFT1",pisdf_vertex);
	vxFFT1->addParameter(param_NC);
	vxFFT1->addParameter(param_NR);
	vxFFT1->setTiming(0, "1302");
	vxFFT1->setTiming(1, "488");
	vxFFT1->setFunction_index(2);

	PiSDFVertex *vxmulti = (PiSDFVertex*)graph->addVertex("multi",pisdf_vertex);
	vxmulti->addParameter(param_NC);
	vxmulti->addParameter(param_NR);
	vxmulti->setTiming(0, "38000");
	vxmulti->setFunction_index(3);

	PiSDFVertex *vxscaling = (PiSDFVertex*)graph->addVertex("scaling",pisdf_vertex);
	vxscaling->addParameter(param_NC);
	vxscaling->addParameter(param_NR);
	vxscaling->setTiming(0, "10");
	vxscaling->setFunction_index(4);

	PiSDFVertex *vxsrc = (PiSDFVertex*)graph->addVertex("src",pisdf_vertex);
	vxsrc->addParameter(param_NC);
	vxsrc->addParameter(param_NR);
	vxsrc->setTiming(0, "10");
	vxsrc->setFunction_index(5);

	PiSDFVertex *vxsnk = (PiSDFVertex*)graph->addVertex("snk",pisdf_vertex);
	vxsnk->addParameter(param_NC);
	vxsnk->addParameter(param_NR);
	vxsnk->setTiming(0, "10");
	vxsnk->setFunction_index(6);

	PiSDFVertex *vxReOrder2 = (PiSDFVertex*)graph->addVertex("ReOrder2",pisdf_vertex);
	vxReOrder2->addParameter(param_NC);
	vxReOrder2->addParameter(param_NR);
	vxReOrder2->setTiming(0, "10000");
	vxReOrder2->setFunction_index(7);

	PiSDFVertex *vxFFT2 = (PiSDFVertex*)graph->addVertex("FFT2",pisdf_vertex);
	vxFFT2->addParameter(param_NC);
	vxFFT2->addParameter(param_NR);
	vxFFT2->setTiming(0, "1302");
	vxFFT2->setTiming(1, "488");
	vxFFT2->setFunction_index(8);

	//Edges
	graph->addEdge(vxsrc, 0, "NC*NR", vxReOrder1, 0, "NC*NR", "0");

	graph->addEdge(vxReOrder1, 0, "NC*NR", vxFFT1, 0, "NR", "0");

	graph->addEdge(vxFFT1, 0, "NR", vxmulti, 0, "NR*NR", "0");

	graph->addEdge(vxmulti, 0, "NR*NC", vxscaling, 0, "NC*NR", "0");

	graph->addEdge(vxscaling, 0, "NC*NR", vxReOrder2, 0, "NR*NC", "0");

	graph->addEdge(vxReOrder2, 0, "NR*NC", vxFFT2, 0, "NC", "0");

	graph->addEdge(vxFFT2, 0, "NC", vxsnk, 0, "NC*NR", "0");

}

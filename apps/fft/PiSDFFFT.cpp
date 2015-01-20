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

#include <grt_definitions.h>

#include <spider.h>
#include "fft.h"

void fft_top(PiSDFGraph* _graphs);

static PiSDFGraph* graphs;
static int nbGraphs = 0;

PiSDFGraph* addGraph(){
	if(nbGraphs >= MAX_NB_PiSDF_GRAPHS) exitWithCode(1054);
	PiSDFGraph* graph = &(graphs[nbGraphs++]);
	graph->reset();
	return graph;
}

void resetGraph(){
	nbGraphs = 0;
}

PiSDFGraph* initPisdf_fft(PiSDFGraph* _graphs){
	graphs = _graphs;

	PiSDFGraph* top = addGraph();

	PiSDFVertex *vxTop = (PiSDFVertex *)top->addVertex("top", normal_vertex);

	PiSDFGraph* mpSchedGraph = addGraph();
	vxTop->setSubGraph(mpSchedGraph);
	mpSchedGraph->setParentVertex(vxTop);

	fft_top(mpSchedGraph);

	return top;
}

void fft_top(PiSDFGraph* graph){

	// Parameters.
	PiSDFParameter *paramNC = graph->addParameter("NC");
	PiSDFParameter *paramNR = graph->addParameter("NR");

	paramNC->setValue(16);
	paramNR->setValue(16);

	// Configure vertices.

	// Special vertices

	// Other vertices
	PiSDFVertex *vxSrc = (PiSDFVertex *)graph->addVertex("Src", normal_vertex);
	vxSrc->addParameter(paramNC);
	vxSrc->addParameter(paramNR);
	vxSrc->setFunction_index(SRC);

	PiSDFVertex *vxTrans1 = (PiSDFVertex *)graph->addVertex("Transpose_1", normal_vertex);
	vxTrans1->addParameter(paramNC);
	vxTrans1->addParameter(paramNR);
	vxTrans1->setFunction_index(TRANSPOSE);

	PiSDFVertex *vxFft2 = (PiSDFVertex *)graph->addVertex("FFT_2", normal_vertex);
	vxFft2->addParameter(paramNR);
	vxFft2->setFunction_index(FFT);

	PiSDFVertex *vxTrans3 = (PiSDFVertex *)graph->addVertex("Transpose_3", normal_vertex);
	vxTrans3->addParameter(paramNC);
	vxTrans3->addParameter(paramNR);
	vxTrans3->setFunction_index(TRANSPOSE);

	PiSDFVertex *vxTwi4 = (PiSDFVertex *)graph->addVertex("Twiddle_4", normal_vertex);
	vxTwi4->addParameter(paramNC);
	vxTwi4->addParameter(paramNR);
	vxTwi4->setFunction_index(TWIDDLE);

	PiSDFVertex *vxFft5 = (PiSDFVertex *)graph->addVertex("FFT_5", normal_vertex);
	vxFft5->addParameter(paramNC);
	vxFft5->setFunction_index(FFT);

	PiSDFVertex *vxTrans6 = (PiSDFVertex *)graph->addVertex("Transpose_6", normal_vertex);
	vxTrans6->addParameter(paramNC);
	vxTrans6->addParameter(paramNR);
	vxTrans6->setFunction_index(TRANSPOSE);

	PiSDFVertex *vxSnk = (PiSDFVertex *)graph->addVertex("Snk", normal_vertex);
	vxSnk->addParameter(paramNC);
	vxSnk->addParameter(paramNR);
	vxSnk->setFunction_index(SNK);

	// Edges.
	graph->addEdge(vxSrc, 		0, "2*2*NC*NR", vxTrans1, 	0, "2*2*NC*NR", "0");
	graph->addEdge(vxTrans1, 	0, "2*2*NC*NR", vxFft2, 	0, "2*2*NR", "0");
	graph->addEdge(vxFft2, 		0, "2*2*NR", 	vxTrans3, 	0, "2*2*NC*NR", "0");
	graph->addEdge(vxTrans3, 	0, "2*2*NC*NR", vxTwi4, 	0, "2*2*NC*NR", "0");
	graph->addEdge(vxTwi4, 		0, "2*2*NC*NR", vxFft5, 	0, "2*2*NC", "0");
	graph->addEdge(vxFft5, 		0, "2*2*NR", 	vxTrans6, 	0, "2*2*NC*NR", "0");
	graph->addEdge(vxTrans6, 	0, "2*2*NC*NR", vxSnk, 	0, "2*2*NC*NR", "0");

	// Timings
//#ifdef DSP
//	vxSrc->setTiming(0, "100");
////	vxTrans1->setTiming(0, "100");
////	vxFft2->setTiming(0, "100");
////	vxTrans3->setTiming(0, "100");
////	vxTwi4->setTiming(0, "100");
////	vxFft5->setTiming(0, "100");
////	vxTrans6->setTiming(0, "100");
//	vxSnk->setTiming(0, "100");
//
////	vxSrc->setTiming(1, "100");
//	vxTrans1->setTiming(1, "100");
//	vxFft2->setTiming(1, "100");
//	vxTrans3->setTiming(1, "100");
//	vxTwi4->setTiming(1, "100");
//	vxFft5->setTiming(1, "100");
//	vxTrans6->setTiming(1, "100");
////	vxSnk->setTiming(1, "100");
//#else
	vxSrc->setTiming(0, "100");
	vxTrans1->setTiming(0, "100");
	vxFft2->setTiming(0, "100");
	vxTrans3->setTiming(0, "100");
	vxTwi4->setTiming(0, "100");
	vxFft5->setTiming(0, "100");
	vxTrans6->setTiming(0, "100");
	vxSnk->setTiming(0, "100");
//#endif

	// Subgraphs

}


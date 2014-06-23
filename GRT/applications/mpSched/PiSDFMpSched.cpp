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

#include "PiSDFMpSched.h"

void mpSched(PiSDFGraph* graph, int NMAX, UINT32 nbSamples, UINT32 Nval);
void mpSched_sub(PiSDFGraph* graph, UINT32 nbSamples);


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

PiSDFGraph* initPisdf_mpSched(PiSDFGraph* _graphs, int NMAX, UINT32 nbSamples, UINT32 Nval){
	graphs = _graphs;

	PiSDFGraph* top = addGraph();

	PiSDFVertex *vxTop = (PiSDFVertex *)top->addVertex("top", pisdf_vertex);

	PiSDFGraph* mpSchedGraph = addGraph();
	vxTop->setSubGraph(mpSchedGraph);
	mpSchedGraph->setParentVertex(vxTop);

	mpSched(mpSchedGraph, NMAX, nbSamples, Nval);

	return top;
}

void mpSched(PiSDFGraph* graph, int NMAX, UINT32 nbSamples, UINT32 Nval){
	// Parameters.
	PiSDFParameter *paramN = graph->addParameter("N");
	PiSDFParameter *paramNVAL = graph->addParameter("N_VAL");
	PiSDFParameter *paramNMAX = graph->addParameter("NMAX");
	PiSDFParameter *paramSamples = graph->addParameter("nbSamples");

	paramNVAL->setValue(Nval);
	paramNMAX->setValue(NMAX);
	paramSamples->setValue(nbSamples);

#if EXEC == 0
	paramN->setValue(10);
#endif

	// Configure vertices.
	PiSDFConfigVertex *vxConfig = (PiSDFConfigVertex *)graph->addVertex("config", config_vertex);
	vxConfig->setFunction_index(0);
	vxConfig->addParameter(paramNMAX);
	vxConfig->addParameter(paramNVAL);
	vxConfig->addRelatedParam(paramN);

	// Other vertices
	PiSDFVertex *vxMFilter 	= (PiSDFVertex *)graph->addVertex("MFilter", pisdf_vertex);
	vxMFilter->addParameter(paramNMAX);
	vxMFilter->addParameter(paramN);
	vxMFilter->setFunction_index(1);

	PiSDFVertex *vxSrc = (PiSDFVertex *)graph->addVertex("Src", pisdf_vertex);
	vxSrc->addParameter(paramN);
	vxSrc->addParameter(paramSamples);
	vxSrc->setFunction_index(2);

	PiSDFVertex *vxSnk 	= (PiSDFVertex *)graph->addVertex("Snk", pisdf_vertex);
	vxSnk->addParameter(paramN);
	vxSnk->addParameter(paramSamples);
	vxSnk->setFunction_index(3);

	PiSDFVertex *vxUserFIRs	= (PiSDFVertex *)graph->addVertex("UserFIRs", pisdf_vertex);
	vxUserFIRs->addParameter(paramSamples);

	// Edges.
	graph->addEdge(vxConfig, 0, "NMAX", vxMFilter, 0, "NMAX", "0");
	graph->addEdge(vxMFilter, 0, "N", vxUserFIRs, 0, "1", "0");

	graph->addEdge(vxSrc, 0, "N*nbSamples*4", vxUserFIRs, 1, "nbSamples*4", "0");
	graph->addEdge(vxUserFIRs, 0, "nbSamples*4", vxSnk, 0, "N*nbSamples*4", "0");

	// Timings
	vxConfig->setTiming(0, "210");
	vxMFilter->setTiming(0, "80");
	vxSrc->setTiming(0, "70");
	vxSnk->setTiming(0, "70");

	// Subgraphs
	PiSDFGraph *MpSched_subGraph = addGraph();
	vxUserFIRs->setSubGraph(MpSched_subGraph);
	MpSched_subGraph->setParentVertex(vxUserFIRs);

	mpSched_sub(MpSched_subGraph, nbSamples);
}

void mpSched_sub(PiSDFGraph* graph, UINT32 nbSamples){
	// Parameters.
	PiSDFParameter *paramM = graph->addParameter("M");
	PiSDFParameter *paramSamples = graph->addParameter("nbSamples2");

	paramSamples->setParameterParentID(0);//->setValue(nbSamples);
#if EXEC == 0
		paramM->setValue(6);
#endif

	// Interface vertices.
	PiSDFIfVertex *vxM = (PiSDFIfVertex*)graph->addVertex("M", input_vertex);
	vxM->setDirection(0);
	vxM->setParentVertex(graph->getParentVertex());
	vxM->setParentEdge(graph->getParentVertex()->getInputEdge(0));
	vxM->setFunction_index(10);

	PiSDFIfVertex *vxIn = (PiSDFIfVertex*)graph->addVertex("In", input_vertex);
	vxIn->setDirection(0);
	vxIn->setParentVertex(graph->getParentVertex());
	vxIn->setParentEdge(graph->getParentVertex()->getInputEdge(0));
	vxIn->addParameter(paramSamples);
	vxIn->setFunction_index(10);

	PiSDFIfVertex *vxOut = (PiSDFIfVertex*)graph->addVertex("Out", output_vertex);
	vxOut->setDirection(1);
	vxOut->setParentVertex(graph->getParentVertex());
	vxOut->setParentEdge(graph->getParentVertex()->getOutputEdge(0));
	vxOut->addParameter(paramSamples);
	vxOut->setFunction_index(10);

	// Configure vertices.
	PiSDFConfigVertex *vxSetM = (PiSDFConfigVertex *)graph->addVertex("setM", config_vertex);
	vxSetM->setFunction_index(4);
	vxSetM->addRelatedParam(paramM);

	// Other vertices
	PiSDFVertex *vxInitSwitch 	= (PiSDFVertex *)graph->addVertex("InitSwitch", pisdf_vertex);
	vxInitSwitch->addParameter(paramM);
	vxInitSwitch->setFunction_index(5);

	PiSDFVertex *vxSwitch 	= (PiSDFVertex *)graph->addVertex("Switch", pisdf_vertex);
	vxSwitch->addParameter(paramSamples);
	vxSwitch->setFunction_index(SWICTH_FUNCT_IX);

	PiSDFVertex *vxFIR	= (PiSDFVertex *)graph->addVertex("FIR", pisdf_vertex);
	vxFIR->addParameter(paramSamples);
	vxFIR->setFunction_index(7);

	PiSDFVertex *vxBr	= (PiSDFVertex *)graph->addVertex("BroadCast", pisdf_vertex);
	vxBr->addParameter(paramSamples);
	vxBr->setFunction_index(BROADCAST_FUNCT_IX);

	// Edges.
	graph->addEdge(vxM, 0, "1", vxSetM, 0, "1", "0");
	graph->addEdge(vxInitSwitch, 0, "M", vxSwitch, 0, "1", "0");

	graph->addEdge(vxIn, 0, "nbSamples2*4", vxSwitch, 1, "nbSamples2*4", "0");
	graph->addEdge(vxBr, 1, "nbSamples2*4", vxSwitch, 2, "nbSamples2*4", "nbSamples2*4");
	graph->addEdge(vxSwitch, 0, "nbSamples2*4", vxFIR, 0, "nbSamples2*4", "0");
	graph->addEdge(vxFIR, 0, "nbSamples2*4", vxBr, 0, "nbSamples2*4", "0");
	graph->addEdge(vxBr, 0, "nbSamples2*4", vxOut, 0, "nbSamples2*4", "0");

	// Timings
	vxSetM->setTiming(0, "200");
	vxInitSwitch->setTiming(0, "70");
	vxSwitch->setTiming(0, "1600");
	vxFIR->setTiming(0, "5501132/60");//"570000");
	vxBr->setTiming(0, "100");
}

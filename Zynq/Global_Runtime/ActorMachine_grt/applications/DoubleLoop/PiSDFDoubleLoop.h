
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

#include <string.h>
#include <graphs/PiSDF/PiSDFGraph.h>

static PiSDFGraph graphs[MAX_NB_PiSDF_SUB_GRAPHS];
static UINT8 nb_graphs = 0;

void MLoop(PiSDFGraph* graph, BaseVertex* parentVertex){
	// Parameters.
	PiSDFParameter *paramM = graph->addParameter("M");

	// Interface vertices.
	PiSDFIfVertex *vxM_in = (PiSDFIfVertex*)graph->addVertex("M_in", input_vertex);
	vxM_in->setDirection(0);
	vxM_in->setParentVertex(parentVertex);
	vxM_in->setParentEdge(parentVertex->getInputEdge(0));
	vxM_in->setFunction_index(11);

	PiSDFIfVertex *vxLine_in = (PiSDFIfVertex*)graph->addVertex("Line_in", input_vertex);
	vxLine_in->setDirection(0);
	vxLine_in->setParentVertex(parentVertex);
	vxLine_in->setParentEdge(parentVertex->getInputEdge(1));
	vxLine_in->setFunction_index(12);

	PiSDFIfVertex *vxLine_out = (PiSDFIfVertex*)graph->addVertex("Line_out", output_vertex);
	vxLine_out->setDirection(1);
	vxLine_out->setParentVertex(parentVertex);
	vxLine_out->setParentEdge(parentVertex->getOutputEdge(0));
	vxLine_out->setFunction_index(13);


	// Configuration vertices
	PiSDFConfigVertex *vxConfigM = (PiSDFConfigVertex *)graph->addVertex("ConfigM", config_vertex);
	vxConfigM->setFunction_index(5);
	vxConfigM->addRelatedParam(paramM);


	// Others..
	PiSDFVertex *vxInitMLoop = (PiSDFVertex*)graph->addVertex("InitMLoop", pisdf_vertex);
	vxInitMLoop->setFunction_index(6);
	vxInitMLoop->addParameter(paramM);

	PiSDFVertex *vxF = (PiSDFVertex *)graph->addVertex("F", pisdf_vertex);
	vxF->setFunction_index(7);

	PiSDFVertex *vxEndMLoop = (PiSDFVertex*)graph->addVertex("EndMLoop", pisdf_vertex);
	vxEndMLoop->setFunction_index(8);
	vxEndMLoop->addParameter(paramM);


	// Edges.
	graph->addEdge(vxM_in, "1", vxConfigM, "1", "0");

	graph->addEdge(vxLine_in, "3", vxInitMLoop, "3", "0");

	graph->addEdge(vxInitMLoop, "M", vxF, "1", "0");

	graph->addEdge(vxF, "1", vxEndMLoop, "M", "0");

	graph->addEdge(vxEndMLoop, "3", vxLine_out, "3", "0");
}


void PiSDFDoubleLoop(PiSDFGraph* graph, BaseVertex* parentVertex){
	// Parameters.
	PiSDFParameter *paramN = graph->addParameter("N");

	// Configure vertices.
	PiSDFConfigVertex *vxReadFile = (PiSDFConfigVertex *)graph->addVertex("ReadFile", config_vertex);
	vxReadFile->setFunction_index(0);
	vxReadFile->addRelatedParam(paramN);
	graph->setRootVertex(vxReadFile);

	// Round buffer vertices
	BaseVertex* roundB_0 = graph->addVertex("RoundBuf_0", roundBuff_vertex);
	roundB_0->setFunction_index(9);
	BaseVertex* roundB_1 = graph->addVertex("RoundBuf_1", roundBuff_vertex);
	roundB_1->setFunction_index(10);

	// Others
	PiSDFVertex *vxBroad 		= (PiSDFVertex *)graph->addVertex("Broadcast", pisdf_vertex);
	vxBroad->setFunction_index(1);

	PiSDFVertex *vxInitNLoop 	= (PiSDFVertex *)graph->addVertex("InitNLoop", pisdf_vertex);
	vxInitNLoop->addParameter(paramN);
	vxInitNLoop->setFunction_index(2);

	PiSDFVertex *vxMLoop 		= (PiSDFVertex *)graph->addVertex("MLoop", pisdf_vertex);

	PiSDFVertex *vxEndNLoop 	= (PiSDFVertex *)graph->addVertex("EndNLoop", pisdf_vertex);
	vxEndNLoop->addParameter(paramN);
	vxEndNLoop->setFunction_index(3);

	PiSDFVertex *vxWriteFile 	= (PiSDFVertex *)graph->addVertex("WriteFile", pisdf_vertex);
	vxWriteFile->setFunction_index(4);

	// Edges.
	graph->addEdge(vxReadFile, "3", roundB_0, "3", "0");
	graph->addEdge(vxReadFile, "9", roundB_1, "9", "0");

	graph->addEdge(roundB_0, "3", vxBroad, "3", "0");

	graph->addEdge(roundB_1, "9", vxInitNLoop, "9", "0");

	graph->addEdge(vxBroad, "3", vxWriteFile, "3", "0");
	graph->addEdge(vxBroad, "3", vxInitNLoop, "3", "0");

	graph->addEdge(vxInitNLoop, "N", vxMLoop, "1", "1");
	graph->addEdge(vxInitNLoop, "N*3", vxMLoop, "3", "0");

	graph->addEdge(vxMLoop, "3", vxEndNLoop, "N*3", "0");

	graph->addEdge(vxEndNLoop, "9", vxWriteFile, "9", "0");

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *MLoop_subGraph = &graphs[nb_graphs]; nb_graphs++;
	MLoop(MLoop_subGraph, vxMLoop);
	vxMLoop->setSubGraph(MLoop_subGraph);
}


void top(PiSDFGraph* graph){
	PiSDFVertex *vxDoubleLoop = (PiSDFVertex *)graph->addVertex("DoubleLoop", pisdf_vertex);

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *DoubleLoop_subGraph = &graphs[nb_graphs]; nb_graphs++;
	PiSDFDoubleLoop(DoubleLoop_subGraph, vxDoubleLoop);
	vxDoubleLoop->setSubGraph(DoubleLoop_subGraph);
}

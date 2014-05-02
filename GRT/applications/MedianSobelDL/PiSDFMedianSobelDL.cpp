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

#include "PiSDFMedianSobelDL.h"

void medianSobel(PiSDFGraph* graph, UINT32 height, UINT32 width, UINT32 nbSlicesMax_median);
void medianSobel_sub(PiSDFGraph* graph);


static PiSDFGraph* graphs;
static int nbGraphs = 0;

PiSDFGraph* addGraph(){
	if(nbGraphs >= MAX_NB_PiSDF_GRAPHS) exitWithCode(1054);
	PiSDFGraph* graph = &(graphs[nbGraphs++]);
	graph->reset();
	return graph;
}

PiSDFGraph* initPisdf_MedianSobelDL(PiSDFGraph* _graphs, UINT32 height, UINT32 width, UINT32 nbSlicesMax_median){
	graphs = _graphs;

	PiSDFGraph* top = addGraph();
	top->setBaseId(0);

	PiSDFVertex *vxTop = (PiSDFVertex *)top->addVertex("top", pisdf_vertex);
	top->setRootVertex(vxTop);

	PiSDFGraph* medSobGraph = addGraph();
	medSobGraph->setBaseId(10);
	vxTop->setSubGraph(medSobGraph);
	medSobGraph->setParentVertex(vxTop);

	medianSobel(medSobGraph, height, width, nbSlicesMax_median);

	return top;
}

void medianSobel(PiSDFGraph* graph, UINT32 height, UINT32 width, UINT32 nbSlicesMax_median){
	// Parameters.
	PiSDFParameter *paramHEIGHT = graph->addParameter("HEIGHT");
	PiSDFParameter *paramWIDTH = graph->addParameter("WIDTH");
	PiSDFParameter *paramNBSLICES_SOBEL = graph->addParameter("NBSLICES_SOBEL");
	PiSDFParameter *paramNBSLICES_MEDIAN = graph->addParameter("NBSLICES_MEDIAN");
	PiSDFParameter *paramNBSLICESMAX_MEDIAN = graph->addParameter("NBSLICESMAX_MEDIAN");

	paramHEIGHT->setValue(height);
	paramWIDTH->setValue(width);
	paramNBSLICESMAX_MEDIAN->setValue(nbSlicesMax_median);

#if EXEC == 0
	paramNBSLICES_SOBEL->setValue(4);
	paramNBSLICES_MEDIAN->setValue(5);
#endif

	// Configure vertices.
	PiSDFConfigVertex *vxReadFile = (PiSDFConfigVertex *)graph->addVertex("ReadFile", config_vertex);
	vxReadFile->setFunction_index(0);
	vxReadFile->addParameter(paramHEIGHT);
	vxReadFile->addParameter(paramWIDTH);
	vxReadFile->addParameter(paramNBSLICESMAX_MEDIAN);
	vxReadFile->addRelatedParam(paramNBSLICES_MEDIAN);
	vxReadFile->addRelatedParam(paramNBSLICES_SOBEL);
	graph->setRootVertex(vxReadFile);

	PiSDFVertex *vxInitMedianLoop 	= (PiSDFVertex *)graph->addVertex("InitMedianLoop", pisdf_vertex);
	vxInitMedianLoop->addParameter(paramNBSLICES_MEDIAN);
	vxInitMedianLoop->addParameter(paramNBSLICESMAX_MEDIAN);
	vxInitMedianLoop->setFunction_index(1);

	PiSDFVertex *vxMedianSlice = (PiSDFVertex *)graph->addVertex("MedianSlice", pisdf_vertex);
	vxMedianSlice->addParameter(paramHEIGHT);
	vxMedianSlice->addParameter(paramWIDTH);
	vxMedianSlice->addParameter(paramNBSLICES_MEDIAN);

	PiSDFVertex *vxSobelSlice 	= (PiSDFVertex *)graph->addVertex("SobelSlice", pisdf_vertex);
	vxSobelSlice->addParameter(paramHEIGHT);
	vxSobelSlice->addParameter(paramWIDTH);
	vxSobelSlice->addParameter(paramNBSLICES_SOBEL);
	vxSobelSlice->setFunction_index(4);

	PiSDFVertex *vxWriteFile 	= (PiSDFVertex *)graph->addVertex("WriteFile", pisdf_vertex);
	vxWriteFile->addParameter(paramHEIGHT);
	vxWriteFile->addParameter(paramWIDTH);
	vxWriteFile->setFunction_index(5);

	// Edges.
	graph->addEdge(vxReadFile, 0, "NBSLICESMAX_MEDIAN", vxInitMedianLoop, 0, "NBSLICESMAX_MEDIAN", "0");
	graph->addEdge(vxInitMedianLoop, 0, "NBSLICES_MEDIAN", vxMedianSlice, 0, "1", "0");

	graph->addEdge(vxReadFile, 1, "HEIGHT*WIDTH", vxMedianSlice, 1, "HEIGHT*WIDTH/NBSLICES_MEDIAN", "0");
	graph->addEdge(vxMedianSlice, 0, "HEIGHT*WIDTH/NBSLICES_MEDIAN", vxSobelSlice, 0, "HEIGHT*WIDTH/NBSLICES_SOBEL", "0");
	graph->addEdge(vxSobelSlice, 0, "HEIGHT*WIDTH/NBSLICES_SOBEL", vxWriteFile, 0, "HEIGHT*WIDTH", "0");

	// Timings
	vxReadFile->setTiming(0, "220000");
	vxInitMedianLoop->setTiming(0, "150");
	vxSobelSlice->setTiming(0, "28000000/NBSLICES_SOBEL");
	vxWriteFile->setTiming(0, "100");

	// Subgraphs
	PiSDFGraph *MedianSlice_subGraph = addGraph();
	MedianSlice_subGraph->setBaseId(20);
	vxMedianSlice->setSubGraph(MedianSlice_subGraph);
	MedianSlice_subGraph->setParentVertex(vxMedianSlice);

	medianSobel_sub(MedianSlice_subGraph);
}

void medianSobel_sub(PiSDFGraph* graph){
	// Parameters.
	PiSDFParameter *paramHEIGHT = graph->addParameter("HEIGHT_2");
	PiSDFParameter *paramWIDTH = graph->addParameter("WIDTH_2");
	PiSDFParameter *paramNBTILES_MEDIAN = graph->addParameter("NBTILES_MEDIAN_2");
	PiSDFParameter *paramNBSLICES_MEDIAN = graph->addParameter("NBSLICES_MEDIAN_2");

	paramHEIGHT->setParameterParentID(0);
	paramWIDTH->setParameterParentID(1);
	paramNBSLICES_MEDIAN->setParameterParentID(2);

#if EXEC == 0
	paramNBTILES_MEDIAN->setValue(3);
#endif
	// Interface vertices.
	PiSDFIfVertex *vxNBTiles_in = (PiSDFIfVertex*)graph->addVertex("NBTiles_in", input_vertex);
	vxNBTiles_in->setDirection(0);
	vxNBTiles_in->setParentVertex(graph->getParentVertex());
	vxNBTiles_in->setParentEdge(graph->getParentVertex()->getInputEdge(0));
	vxNBTiles_in->setFunction_index(10);

	PiSDFIfVertex *vxSlice_in = (PiSDFIfVertex*)graph->addVertex("Slice_in", input_vertex);
	vxSlice_in->setDirection(0);
	vxSlice_in->addParameter(paramHEIGHT);
	vxSlice_in->addParameter(paramWIDTH);
	vxSlice_in->addParameter(paramNBSLICES_MEDIAN);
	vxSlice_in->setParentVertex(graph->getParentVertex());
	vxSlice_in->setParentEdge(graph->getParentVertex()->getInputEdge(1));
	vxSlice_in->setFunction_index(10);

	PiSDFIfVertex *vxSlice_out = (PiSDFIfVertex*)graph->addVertex("Slice_out", output_vertex);
	vxSlice_out->setDirection(1);
	vxSlice_out->addParameter(paramHEIGHT);
	vxSlice_out->addParameter(paramWIDTH);
	vxSlice_out->addParameter(paramNBSLICES_MEDIAN);
	vxSlice_out->setParentVertex(graph->getParentVertex());
	vxSlice_out->setParentEdge(graph->getParentVertex()->getOutputEdge(0));
	vxSlice_out->setFunction_index(10);

	// Configure vertices.
	PiSDFConfigVertex *vxConfigNBTiles = (PiSDFConfigVertex *)graph->addVertex("ConfigNBTiles", config_vertex);
	vxConfigNBTiles->setFunction_index(2);
	vxConfigNBTiles->addRelatedParam(paramNBTILES_MEDIAN);
	graph->setRootVertex(vxConfigNBTiles);

	// Other vertices
	PiSDFVertex *vxMedianTile 	= (PiSDFVertex *)graph->addVertex("MedianTile", pisdf_vertex);
	vxMedianTile->addParameter(paramHEIGHT);
	vxMedianTile->addParameter(paramWIDTH);
	vxMedianTile->addParameter(paramNBSLICES_MEDIAN);
	vxMedianTile->addParameter(paramNBTILES_MEDIAN);
	vxMedianTile->setFunction_index(3);

	// Edges.
	graph->addEdge(vxNBTiles_in, 0, "1", vxConfigNBTiles, 0, "1", "0");

	graph->addEdge(vxSlice_in, 0, "HEIGHT_2*WIDTH_2/NBSLICES_MEDIAN_2", vxMedianTile, 0, "HEIGHT_2*WIDTH_2/(NBSLICES_MEDIAN_2*NBTILES_MEDIAN_2)", "0");
	graph->addEdge(vxMedianTile, 0, "HEIGHT_2*WIDTH_2/(NBSLICES_MEDIAN_2*NBTILES_MEDIAN_2)", vxSlice_out, 0, "HEIGHT_2*WIDTH_2/NBSLICES_MEDIAN_2", "0");

	// Timings
	vxConfigNBTiles->setTiming(0, "200");
	vxMedianTile->setTiming(0, "17000000/(NBTILES_MEDIAN_2*NBSLICES_MEDIAN_2)");
}

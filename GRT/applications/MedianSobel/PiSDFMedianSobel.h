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

//void MedianSlice(PiSDFGraph* graph, PiSDFAbstractVertex* parentVertex, Scenario* scenario){
//	// Parameters.
//	graph->setParentVertex(parentVertex);
//
//	PiSDFParameter *paramHEIGHT = graph->addParameter("HEIGHT_2");
//	PiSDFParameter *paramWIDTH = graph->addParameter("WIDTH_2");
//	PiSDFParameter *paramNBTILES_MEDIAN = graph->addParameter("NBTILES_MEDIAN_2");
//	PiSDFParameter *paramNBSLICES_MEDIAN = graph->addParameter("NBSLICES_MEDIAN_2");
//
//	paramHEIGHT->setParameterParentID(0);
//	paramWIDTH->setParameterParentID(1);
//	paramNBSLICES_MEDIAN->setParameterParentID(2);
//
//#if EXEC == 0
//	paramNBTILES_MEDIAN->setValue(2);
//#endif
//	// Interface vertices.
//	PiSDFIfVertex *vxNBTiles_in = (PiSDFIfVertex*)graph->addVertex("NBTiles_in", input_vertex);
//	vxNBTiles_in->setDirection(0);
//	vxNBTiles_in->setParentVertex(parentVertex);
//	vxNBTiles_in->setParentEdge(parentVertex->getInputEdge(0));
//	vxNBTiles_in->setFunction_index(10);
//
//	PiSDFIfVertex *vxSlice_in = (PiSDFIfVertex*)graph->addVertex("Slice_in", input_vertex);
//	vxSlice_in->setDirection(0);
//	vxSlice_in->addParameter(paramHEIGHT);
//	vxSlice_in->addParameter(paramWIDTH);
//	vxSlice_in->addParameter(paramNBSLICES_MEDIAN);
//	vxSlice_in->setParentVertex(parentVertex);
//	vxSlice_in->setParentEdge(parentVertex->getInputEdge(1));
//	vxSlice_in->setFunction_index(10);
//
//	PiSDFIfVertex *vxSlice_out = (PiSDFIfVertex*)graph->addVertex("Slice_out", output_vertex);
//	vxSlice_out->setDirection(1);
//	vxSlice_out->addParameter(paramHEIGHT);
//	vxSlice_out->addParameter(paramWIDTH);
//	vxSlice_out->addParameter(paramNBSLICES_MEDIAN);
//	vxSlice_out->setParentVertex(parentVertex);
//	vxSlice_out->setParentEdge(parentVertex->getOutputEdge(0));
//	vxSlice_out->setFunction_index(10);
//
//	// Configure vertices.
//	PiSDFConfigVertex *vxConfigNBTiles = (PiSDFConfigVertex *)graph->addVertex("ConfigNBTiles", config_vertex);
//	vxConfigNBTiles->setFunction_index(2);
//	vxConfigNBTiles->addRelatedParam(paramNBTILES_MEDIAN);
//	graph->setRootVertex(vxConfigNBTiles);
//
//	// Other vertices
//	PiSDFVertex *vxMedianTileFormat = (PiSDFVertex *)graph->addVertex("MedianTileF", pisdf_vertex);
//	vxMedianTileFormat->addParameter(paramHEIGHT);
//	vxMedianTileFormat->addParameter(paramWIDTH);
//	vxMedianTileFormat->addParameter(paramNBSLICES_MEDIAN);
//	vxMedianTileFormat->addParameter(paramNBTILES_MEDIAN);
//	vxMedianTileFormat->setFunction_index(6);
//
//	PiSDFVertex *vxMedianTile 	= (PiSDFVertex *)graph->addVertex("MedianTile", pisdf_vertex);
//	vxMedianTile->addParameter(paramHEIGHT);
//	vxMedianTile->addParameter(paramWIDTH);
//	vxMedianTile->addParameter(paramNBSLICES_MEDIAN);
//	vxMedianTile->addParameter(paramNBTILES_MEDIAN);
//	vxMedianTile->setFunction_index(3);
//
//	PiSDFVertex *vxMedianTileReconstruct = (PiSDFVertex *)graph->addVertex("MedianTileR", pisdf_vertex);
//	vxMedianTileReconstruct->addParameter(paramHEIGHT);
//	vxMedianTileReconstruct->addParameter(paramWIDTH);
//	vxMedianTileReconstruct->addParameter(paramNBSLICES_MEDIAN);
//	vxMedianTileReconstruct->addParameter(paramNBTILES_MEDIAN);
//	vxMedianTileReconstruct->setFunction_index(7);
//
//	// Edges.
//	graph->addEdge(vxNBTiles_in, 0, "1", vxConfigNBTiles, 0, "1", "0");
//
//	graph->addEdge(vxSlice_in, 0, "HEIGHT_2*WIDTH_2/NBSLICES_MEDIAN_2", vxMedianTileFormat, 0, "HEIGHT_2*WIDTH_2/NBSLICES_MEDIAN_2", "0");
//	graph->addEdge(vxMedianTileFormat, 0, "HEIGHT_2*WIDTH_2/NBSLICES_MEDIAN_2", vxMedianTile, 0, "HEIGHT_2*WIDTH_2/(NBSLICES_MEDIAN_2*NBTILES_MEDIAN_2)", "0");
//	graph->addEdge(vxMedianTile, 0, "HEIGHT_2*WIDTH_2/(NBSLICES_MEDIAN_2*NBTILES_MEDIAN_2)", vxMedianTileReconstruct, 0, "HEIGHT_2*WIDTH_2/NBSLICES_MEDIAN_2", "0");
//	graph->addEdge(vxMedianTileReconstruct, 0, "HEIGHT_2*WIDTH_2/NBSLICES_MEDIAN_2", vxSlice_out, 0, "HEIGHT_2*WIDTH_2/NBSLICES_MEDIAN_2", "0");
//
//	// Timings
//	scenario->setTiming(vxConfigNBTiles->getId()+graph->getBaseId(), 1, "200");
//	scenario->setTiming(vxMedianTile->getId()+graph->getBaseId(), 1, "692000/NBTILES_MEDIAN_2");
//	scenario->setTiming(vxMedianTileFormat->getId()+graph->getBaseId(), 1, "10000");
//	scenario->setTiming(vxMedianTileReconstruct->getId()+graph->getBaseId(), 1, "10000");
//}

void PiSDFMedianSobel(PiSDFGraph* graph, PiSDFAbstractVertex* parentVertex, Scenario* scenario, UINT32 height, UINT32 width, UINT32 nbSlicesMax_median){
	graph->setParentVertex(parentVertex);

	// Parameters.
	PiSDFParameter *paramHEIGHT = graph->addParameter("HEIGHT");
	PiSDFParameter *paramWIDTH = graph->addParameter("WIDTH");
	PiSDFParameter *paramNBSLICES_SOBEL = graph->addParameter("NBSLICES_SOBEL");
	PiSDFParameter *paramNBSLICES_MEDIAN = graph->addParameter("NBSLICES_MEDIAN");
//	PiSDFParameter *paramNBSLICESMAX_MEDIAN = graph->addParameter("NBSLICESMAX_MEDIAN");

	paramHEIGHT->setValue(height);
	paramWIDTH->setValue(width);
//	paramNBSLICESMAX_MEDIAN->setValue(nbSlicesMax_median);

#if EXEC == 0
	paramNBSLICES_SOBEL->setValue(4);
	paramNBSLICES_MEDIAN->setValue(5);
#endif

	// Configure vertices.
	PiSDFConfigVertex *vxReadFile = (PiSDFConfigVertex *)graph->addVertex("ReadFile", config_vertex);
	vxReadFile->setFunction_index(0);
	vxReadFile->addParameter(paramHEIGHT);
	vxReadFile->addParameter(paramWIDTH);
//	vxReadFile->addParameter(paramNBSLICESMAX_MEDIAN);
	vxReadFile->addRelatedParam(paramNBSLICES_MEDIAN);
	vxReadFile->addRelatedParam(paramNBSLICES_SOBEL);
	graph->setRootVertex(vxReadFile);

//	PiSDFVertex *vxInitMedianLoop 	= (PiSDFVertex *)graph->addVertex("InitMedianLoop", pisdf_vertex);
//	vxInitMedianLoop->addParameter(paramNBSLICES_MEDIAN);
//	vxInitMedianLoop->addParameter(paramNBSLICESMAX_MEDIAN);
//	vxInitMedianLoop->setFunction_index(1);

	PiSDFVertex *vxMedianSlice = (PiSDFVertex *)graph->addVertex("MedianSlice", pisdf_vertex);
	vxMedianSlice->addParameter(paramHEIGHT);
	vxMedianSlice->addParameter(paramWIDTH);
	vxMedianSlice->addParameter(paramNBSLICES_MEDIAN);
	vxMedianSlice->setFunction_index(3);

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
//	graph->addEdge(vxReadFile, 0, "NBSLICESMAX_MEDIAN", vxInitMedianLoop, 0, "NBSLICESMAX_MEDIAN", "0");
//	graph->addEdge(vxInitMedianLoop, 0, "NBSLICES_MEDIAN", vxMedianSlice, 0, "1", "0");
//
//	graph->addEdge(vxReadFile, 1, "HEIGHT*WIDTH", vxMedianSlice, 1, "HEIGHT*WIDTH/NBSLICES_MEDIAN", "0");
//	graph->addEdge(vxMedianSlice, 0, "HEIGHT*WIDTH/NBSLICES_MEDIAN", vxSobelSlice, 0, "HEIGHT*WIDTH/NBSLICES_SOBEL", "0");
//	graph->addEdge(vxSobelSlice, 0, "HEIGHT*WIDTH/NBSLICES_SOBEL", vxWriteFile, 0, "HEIGHT*WIDTH", "0");

	graph->addEdge(vxReadFile, 0, "HEIGHT*WIDTH", vxMedianSlice, 0, "HEIGHT*WIDTH/NBSLICES_MEDIAN", "0");
	graph->addEdge(vxMedianSlice, 0, "HEIGHT*WIDTH/NBSLICES_MEDIAN", vxSobelSlice, 0, "HEIGHT*WIDTH/NBSLICES_SOBEL", "0");
	graph->addEdge(vxSobelSlice, 0, "HEIGHT*WIDTH/NBSLICES_SOBEL", vxWriteFile, 0, "HEIGHT*WIDTH", "0");

	// Timings
	scenario->setTiming(vxReadFile->getId()+graph->getBaseId(), 1, "220000");
//	scenario->setTiming(vxInitMedianLoop->getId()+graph->getBaseId(), 1, "150");
	scenario->setTiming(vxMedianSlice->getId()+graph->getBaseId(), 1, "18000000/NBSLICES_MEDIAN");
	scenario->setTiming(vxSobelSlice->getId()+graph->getBaseId(), 1, "34000000/NBSLICES_SOBEL");
	scenario->setTiming(vxWriteFile->getId()+graph->getBaseId(), 1, "100");

//	// Constraints
//	scenario->setConstraints(vxReadFile->getId(), 0);
//	scenario->setConstraints(roundB_0->getId(), 0);
//	scenario->setConstraints(roundB_1->getId(), 0);
//	scenario->setConstraints(vxBroad->getId(), 0);
//	scenario->setConstraints(vxInitNLoop->getId(), 0);
//	scenario->setConstraints(vxEndNLoop->getId(), 0);
//	scenario->setConstraints(vxWriteFile->getId(), 0);

	// Subgraphs
//	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS) exitWithCode(1054);
//	PiSDFGraph *MedianSlice_subGraph = &graphs[nb_graphs++];
//	MedianSlice_subGraph->setBaseId(20);
//	MedianSlice(MedianSlice_subGraph, vxMedianSlice, scenario);
//	vxMedianSlice->setSubGraph(MedianSlice_subGraph);
}


void top(PiSDFGraph* graph, Scenario* scenario, UINT32 height, UINT32 width, UINT32 nbSlicesMax_median){
	nb_graphs = 0;
	PiSDFVertex *vxMedianSobel = (PiSDFVertex *)graph->addVertex("SimpleLoop", pisdf_vertex);

	graph->setRootVertex(vxMedianSobel);

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *MedianSobel_subGraph = &graphs[nb_graphs]; nb_graphs++;
	MedianSobel_subGraph->setBaseId(10);
	PiSDFMedianSobel(MedianSobel_subGraph, vxMedianSobel, scenario, height, width, nbSlicesMax_median);
	vxMedianSobel->setSubGraph(MedianSobel_subGraph);
}

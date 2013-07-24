/*
 * DPCM.cpp
 *
 *  Created on: 5 juil. 2013
 *      Author: yoliva
 */
#include <string.h>
#include "DPCM.h"
#include "../graphs/PiCSDF/PiCSDFGraph.h"


#define H_VALUE	3
#define W_VALUE	4
#define B_VALUE	1
#define N_VALUE	1


void createPiCSDF_DPCM_1(PiCSDFGraph* graph){
	// Vertices.
	CSDAGVertex *vertexRead = graph->addVertex("Read");
	CSDAGVertex *vertexRasterToMB = graph->addVertex("RasterToMB");
	CSDAGVertex *vertexDPCM = graph->addVertex("DPCM");
	CSDAGVertex *vertexMBToRaster = graph->addVertex("MBToRaster");
	CSDAGVertex *vertexWrite = graph->addVertex("Write");

	// Configuration vertices.
	CSDAGVertex *vertexSetH = graph->addConfigVertex("SetH");
	CSDAGVertex *vertexSetW = graph->addConfigVertex("SetW");
	CSDAGVertex *vertexSetB = graph->addConfigVertex("SetB");
	CSDAGVertex *vertexSetN = graph->addConfigVertex("SetN");

	// Variables.
	globalParser.addVariable("H", 1);
	globalParser.addVariable("W", 1);
	globalParser.addVariable("B", 1);
	globalParser.addVariable("N", 1);

	// Parameters.
	PiCSDFParameter* paramH = graph->addParameter("H");
	PiCSDFParameter* paramW = graph->addParameter("W");
	PiCSDFParameter* paramB = graph->addParameter("B");
	PiCSDFParameter* paramN = graph->addParameter("N");

	// Configuration input ports.
	graph->addConfigPort(vertexRead, paramH, 0);
	graph->addConfigPort(vertexRead, paramW, 0);
	graph->addConfigPort(vertexRead, paramB, 0);
	graph->addConfigPort(vertexRasterToMB, paramH, 0);
	graph->addConfigPort(vertexRasterToMB, paramW, 0);
	graph->addConfigPort(vertexRasterToMB, paramB, 0);
	graph->addConfigPort(vertexDPCM, paramN, 0);
	graph->addConfigPort(vertexDPCM, paramB, 0);
	graph->addConfigPort(vertexMBToRaster, paramH, 0);
	graph->addConfigPort(vertexMBToRaster, paramW, 0);
	graph->addConfigPort(vertexMBToRaster, paramB, 0);
	graph->addConfigPort(vertexWrite, paramH, 0);
	graph->addConfigPort(vertexWrite, paramW, 0);
	graph->addConfigPort(vertexWrite, paramB, 0);

	// Configuration output ports.
	graph->addConfigPort(vertexSetH, paramH, 1);
	graph->addConfigPort(vertexSetW, paramW, 1);
	graph->addConfigPort(vertexSetB, paramB, 1);
	graph->addConfigPort(vertexSetN, paramN, 1);

	// Edges.
	graph->addEdge(vertexRead, "W*H*B", vertexRasterToMB, "W*H*B", "0");
	graph->addEdge(vertexRasterToMB, "W*H*B", vertexDPCM, "H*B/N", "0");
//	graph->addEdge(vertexDPCM, "H*B/N", vertexDPCM, "H*B/N", "H*B");
	graph->addEdge(vertexDPCM, "H*B/N", vertexMBToRaster, "W*H*B", "0");
	graph->addEdge(vertexMBToRaster, "W*H*B", vertexWrite, "W*H*B", "0");
}


int executeConfigActorDPCM(CSDAGVertex* vertex){
	if(strcmp(vertex->getName(), "SetH") == 0)
		return H_VALUE;
	if(strcmp(vertex->getName(), "SetW") == 0)
		return W_VALUE;
	if(strcmp(vertex->getName(), "SetB") == 0)
		return B_VALUE;
//	if(strcmp(vertex->getName(), "SetN") == 0)
	return N_VALUE;
}


void resolvePiCSDFParams(PiCSDFGraph* graph){
	for (int i = 0; i < graph->getNbConfigOutPorts(); i++) {
		// Getting the output port.
		PiCSDFConfigPort *outPort = graph->getConfigOutPort(i);

		// Retrieving the value from the actor's execution.
		int value = executeConfigActorDPCM(outPort->vertex);

		// Updating the corresponding parameter' value.
		((variable*)(outPort->parameter->expression[0].elt_union.var))->value = value;
	}
}

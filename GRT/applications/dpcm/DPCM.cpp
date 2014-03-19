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
	PiCSDFParameter* paramH = graph->addParameter("H", "H");
	PiCSDFParameter* paramW = graph->addParameter("W", "W");
	PiCSDFParameter* paramB = graph->addParameter("B", "B");
	PiCSDFParameter* paramN = graph->addParameter("N", "N");

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

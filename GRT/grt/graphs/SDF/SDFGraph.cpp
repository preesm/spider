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

#include "SDFGraph.h"

BaseVertex* SDFGraph::addVertex(BaseVertex* originalVertex){
	if(nbVertices >= MAX_NB_VERTICES){
		// Adding a vertex while the graph is already full
		exitWithCode(1000);
	}
	vertices[nbVertices] = originalVertex;

	if(((PiSDFVertex*)originalVertex)->getType() == config_vertex)
		configVertices[nbConfigVertices++] = originalVertex;

	return vertices[nbVertices++];
}

BaseEdge* SDFGraph::addEdge(BaseVertex* source, UINT32 production, BaseVertex* sink, UINT32 consumption, BaseEdge* refEdge)
{
	if(nbEdges >= MAX_NB_EDGES){
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
	BaseEdge* edge = &edges[nbEdges++];
	edge->setSource(source);
	edge->setProductionInt(production);
	edge->setSink(sink);
	edge->setConsumtionInt(consumption);
	edge->setRefEdge(refEdge);

	return edge;
}

void SDFGraph::updateRBProd(){
	UINT32 maxResult = 0;
	for (UINT32 i = 0; i < nbVertices; i++){
		BaseVertex* vertex = vertices[i];
		if(vertex->getType() == roundBuff_vertex){
			BaseEdge* outEdge = vertex->getOutputEdge(0);
			BaseEdge* inEdge = vertex->getInputEdge(0);

			//
			UINT32 prod = outEdge->getConsumptionInt() * outEdge->getSink()->getNbRepetition();
			UINT32 cons = inEdge->getConsumptionInt();

			outEdge->setProductionInt(prod);

			UINT32 result;
			if(cons > prod){
				UINT32 rest = cons % prod;
				result = cons / prod;
				if(rest != 0) result += 1;
			}
			else
				result = 1;

			if(result > maxResult) maxResult = result;
		}
	}

	for (UINT32 i = 0; i < nbVertices; i++){
		BaseVertex* vertex = vertices[i];
		if(vertex->getType() == roundBuff_vertex){
			BaseEdge* outEdge = vertex->getOutputEdge(0);
			UINT32 prod = outEdge->getProductionInt();
			outEdge->setProductionInt(prod * maxResult);
		}
	}
}

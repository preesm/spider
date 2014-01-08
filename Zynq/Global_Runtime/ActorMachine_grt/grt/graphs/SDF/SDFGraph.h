
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#ifndef SDFGRAPH_H_
#define SDFGRAPH_H_

#include <grt_definitions.h>
#include "../Base/BaseEdge.h"
#include "tools/SchedulingError.h"

class BaseVertex;

class SDFGraph {
	BaseVertex* vertices[MAX_NB_VERTICES];
	BaseEdge	edges[MAX_NB_EDGES];
	BaseVertex* configVertices[MAX_NB_PiSDF_CONFIG_VERTICES];
	UINT32 		nbVertices;
	UINT32		nbEdges;
	UINT32		nbConfigVertices;
public:
	SDFGraph():nbVertices(0), nbEdges(0), nbConfigVertices(0){};

	void reset(){
		memset(vertices, 0, sizeof(BaseVertex*)*nbVertices);
		memset(edges, 0, sizeof(BaseEdge*)*nbEdges);
		memset(configVertices, 0, sizeof(BaseVertex*)*nbConfigVertices);

		nbVertices = 0;
		nbEdges = 0;
		nbConfigVertices = 0;
	}

	void addVertex(BaseVertex* originalVertex);

	BaseVertex* getVertex(UINT32 index)
	{
		return vertices[index];
	}

	void removeVertex(UINT32 index)
	{
		vertices[index] = 0;
		nbVertices--;
	}

	BaseEdge* addEdge(BaseVertex* source, UINT32 production, BaseVertex* sink, UINT32 consumption);

	BaseEdge* getEdge(UINT32 index)
	{
		return &edges[index];
	}

	void addConfigVertex(BaseVertex* configVertex){
		configVertices[nbConfigVertices++] = configVertex;
	}

	BaseVertex* getConfigVertex(UINT32 index){
		return configVertices[index];
	}


	/*
	 * Returns true if both the source and the sink vertices of 'edge' are present in 'vertices'.
	 */
	bool checkEdge(BaseEdge* edge){
		BaseVertex* vertex;
		UINT32 index = 0;
		UINT32 cntr = 0;
		bool sourceOk = false;
		bool sinkOk = false;

		while(cntr < nbVertices && (!sourceOk || !sinkOk))
		{
			vertex = vertices[index];
			if(vertex != NULL){
				if (vertex == edge->getSource()) sourceOk = true;
				if (vertex == edge->getSink())	sinkOk = true;

				cntr++;
			}
			index++;
		}
		return (sourceOk && sinkOk);
	}



	/*
	 * Auto generated getters and setters.
	 */
    UINT32 getNbVertices() const
    {
        return nbVertices;
    }

    void setNbVertices(UINT32 nbVertices)
    {
        this->nbVertices = nbVertices;
    }

    UINT32 getNbEdges() const
    {
        return nbEdges;
    }

    void setNbEdges(UINT32 nbEdges)
    {
        this->nbEdges = nbEdges;
    }

    UINT32 getNbConfigVertices() const
    {
    	return nbConfigVertices;
    }

    void setNbConfigVertices(UINT32 nbConfigVertices)
    {
    	this->nbConfigVertices = nbConfigVertices;
    }
};

inline void SDFGraph::addVertex(BaseVertex* originalVertex){
	if(nbVertices >= MAX_NB_VERTICES){
		// Adding a vertex while the graph is already full
		exitWithCode(1000);
	}
	vertices[nbVertices++] = originalVertex;
}

inline BaseEdge* SDFGraph::addEdge(BaseVertex* source, UINT32 production, BaseVertex* sink, UINT32 consumption)
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

	return edge;
}

#endif /* SDFGRAPH_H_ */

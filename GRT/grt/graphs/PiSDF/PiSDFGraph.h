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

#ifndef PISDFGRAPH_H_
#define PISDFGRAPH_H_

#include <grt_definitions.h>
#include "PiSDFEdge.h"
#include "PiSDFParameter.h"
#include "PiSDFVertex.h"
#include "PiSDFConfigVertex.h"
#include "PiSDFIfVertex.h"
#include "../SRDAG/SRDAGGraph.h"
#include <scheduling/ListScheduler/ListScheduler.h>
#include "launcher/launcher.h"
#include <tools/ExecutionStat.h>

class PiSDFGraph {
	static UINT32 glbNbConfigVertices;

	List<PiSDFEdge,MAX_NB_PiSDF_EDGES> 			edges;
	List<PiSDFParameter,MAX_NB_PiSDF_PARAMS> 	parameters;

	List<PiSDFAbstractVertex*,MAX_NB_PiSDF_VERTICES>		vertices;
	List<PiSDFVertex,MAX_NB_PiSDF_VERTICES>					pisdf_vertices;
	List<PiSDFConfigVertex,MAX_NB_PiSDF_CONFIG_VERTICES>	config_vertices;
	List<PiSDFIfVertex,MAX_NB_PiSDF_INPUT_VERTICES>			input_vertices;
	List<PiSDFIfVertex,MAX_NB_PiSDF_OUTPUT_VERTICES>		output_vertices;

	PiSDFAbstractVertex* parentVertex;
	UINT32 nbExecVertices; 			// Counts the number of executable vertices.
	UINT32 nbDiscardVertices; 		// Counts the number of discarded vertices
									// e.g. a classic vertex with 0 production on an input edge.
						   // nbExecVertices = nb_vertices means the graph can be completely executed.
	bool executable;				// True when the graph can be completely executed.
public:
	PiSDFGraph();
	~PiSDFGraph(){}

	void reset();
	void resetRefs();

	PiSDFEdge* 	addEdge(PiSDFAbstractVertex* source, UINT32 sourcePortId, const char* production, PiSDFAbstractVertex* sink, UINT32 sinkPortId, const char* consumption, const char* delay);

	PiSDFEdge*	addEdge(PiSDFAbstractVertex* source, UINT32 sourcePortId, abstract_syntax_elt* production, PiSDFAbstractVertex* sink, UINT32 sinkPortId, abstract_syntax_elt* consumption, abstract_syntax_elt* delay);

	PiSDFAbstractVertex* addVertex(const char* vertexName, VERTEX_TYPE type);

	PiSDFParameter*	addParameter(const char* name);

	void evaluateExpressions(/*Scenario* scenario*/);

	/*
	 * Creates SrDAG graph including configure and input vertices.
	 */
	void createSrDAGInputConfigVxs(SRDAGGraph* outSrDAG, SRDAGVertex* hSrDagVx);

	void solveParameters(SRDAGGraph* dag, BaseSchedule* schedule);


	/*
	 * Auto-generated getters and setters.
	 */

    UINT32 getNb_parameters()
    {
        return parameters.getNb();
    }

    PiSDFParameter* getParameter(UINT64 index){
    	return &parameters[index];
    }

	UINT32 getNb_config_vertices()
	{
		return config_vertices.getNb();
	}

	PiSDFConfigVertex* getConfig_vertex(UINT64 index)
	{
		return &config_vertices[index];
	}

//	PiSDFConfigVertex* getListConfigVertices()
//	{
//		return &config_vertices[0];
//	}
//
	UINT32 getNb_input_vertices()
	{
		return input_vertices.getNb();
	}

    PiSDFIfVertex* getInput_vertex(UINT64 index)
    {
        return &input_vertices[index];
    }

//    PiSDFIfVertex* getInputVertex(PiSDFEdge *parentEdge)
//    {
//    	for(UINT32 i = 0; i < nb_input_vertices; i++)
//    	{
//    		if(input_vertices[i].getParentEdge() == parentEdge)
//    			return &input_vertices[i];
//    	}
//    	return NULL;
//    }
//

	UINT32 getNb_output_vertices()
	{
		return output_vertices.getNb();
	}

    PiSDFIfVertex* getOutput_vertex(UINT64 index)
    {
        return &output_vertices[index];
    }

	UINT32 getNb_pisdf_vertices()
	{
		return pisdf_vertices.getNb();
	}

    PiSDFVertex* getPiSDFVertex(UINT64 index)
    {
        return &pisdf_vertices[index];
    }

	UINT32 getNb_edges()
	{
		return edges.getNb();
	}

	PiSDFEdge* getEdge(UINT64 index)
	{
		return &edges[index];
	}

	UINT32 getNbVertices() {
		return vertices.getNb();
	}

	PiSDFAbstractVertex* getVertex(int id){
		return vertices[id];
	}

	PiSDFAbstractVertex* getParentVertex(){
		return parentVertex;
	}

	void setParentVertex(PiSDFAbstractVertex* parentVertex) {
		this->parentVertex = parentVertex;
	}
};

#endif /* PISDFGRAPH_H_ */

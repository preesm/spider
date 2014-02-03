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

#ifndef PISDFGRAPH_H_
#define PISDFGRAPH_H_

#include <grt_definitions.h>
#include "PiSDFEdge.h"
#include "PiSDFParameter.h"
#include "PiSDFVertex.h"
#include "PiSDFConfigVertex.h"
#include "PiSDFIfVertex.h"
#include "../SDF/SDFGraph.h"
#include "../SRDAG/SRDAGGraph.h"

class PiSDFGraph {
	UINT32 nb_edges;
	UINT32 nb_parameters;

	UINT32 nb_vertices;
	UINT32 nb_pisdf_vertices;
	UINT32 nb_config_vertices;
	UINT32 nb_join_vertices;
	UINT32 nb_input_vertices;
	UINT32 nb_broad_vertices;
	UINT32 nb_output_vertices;
	UINT32 nb_switch_vertices;
	UINT32 nb_select_vertices;
	UINT32 nb_roundB_vertices;

	static UINT32 glbNbConfigVertices;

	PiSDFEdge 		edges[MAX_NB_PiSDF_EDGES];
	PiSDFParameter 	parameters[MAX_NB_PiSDF_PARAMS];
//	variable* 	parameters[MAX_NB_PiSDF_PARAMS];

	BaseVertex*			vertices[MAX_NB_VERTICES];
	PiSDFVertex 		pisdf_vertices[MAX_NB_PiSDF_VERTICES];
	PiSDFConfigVertex 	config_vertices[MAX_NB_PiSDF_CONFIG_VERTICES];
	BaseVertex 			join_vertices[MAX_NB_PiSDF_JOIN_VERTICES];
	PiSDFIfVertex		input_vertices[MAX_NB_PiSDF_INPUT_VERTICES];
	BaseVertex 			broad_vertices[MAX_NB_PiSDF_BROAD_VERTICES];
	PiSDFIfVertex		output_vertices[MAX_NB_PiSDF_OUTPUT_VERTICES];
	BaseVertex 			switch_vertices[MAX_NB_PiSDF_SWITCH_VERTICES];
	BaseVertex 			select_vertices[MAX_NB_PiSDF_SELECT_VERTICES];
	BaseVertex 			roundB_vertices[MAX_NB_PiSDF_ROUNDB_VERTICES];


	BaseVertex* rootVertex; // Must be set while creating the graph.
//	static BaseVertex* ExecutableVertices[MAX_NB_VERTICES]; // Vertices which do not depend on unresolved parameters.
	static PiSDFEdge* requiredEdges[MAX_NB_EDGES]; // Edges required for an execution.
	static PiSDFIfVertex* visitedIfs[MAX_NB_VERTICES];
	static UINT32 glbNbRequiredEdges;
	static UINT32 glbNbExecConfigVertices;
	static UINT32 glbNbExecVertices;
	static UINT32 glbNbVisitedIfs;

	UINT32 nbExecVertices; 			// Counts the number of executable vertices.
	UINT32 nbDiscardVertices; 		// Counts the number of discarded vertices
									// e.g. a classic vertex with 0 production on an input edge.
						   // nbExecVertices = nb_vertices means the graph can be completely executed.
//	bool execComplete;	   // True when the graph can be completely executed.
public:
	PiSDFGraph();

	PiSDFEdge* 	addEdge(BaseVertex* source, const char* production, BaseVertex* sink, const char* consumption, const char* delay);

	PiSDFEdge*	addEdge(BaseVertex* source, abstract_syntax_elt* production, BaseVertex* sink, abstract_syntax_elt* consumption, abstract_syntax_elt* delay);

	BaseVertex* addVertex(const char* vertexName, VERTEXT_TYPE type);

	PiSDFParameter*	addParameter(const char* name);

//	BaseVertex *getExecutableVertex(UINT32 index)
//	{
//		return ExecutableVertices[index];
//	}

//	 Resets the list of executable vertices.
//	void resetExecutableVertices()
//	{
//		memset(ExecutableVertices, 0, MAX_NB_VERTICES);
//		nbExecutableVertices = 0;
//	}

	/*
	 * Update the list of executables vertices, i.e. those which do not depend on unresolved parameters.
	 * It also discards useless information from the graph, e.g. edges with zero production and/or zero consumption.
	 * Note that the list is a static member so it must be reset before calling this method.
	 * TODO: Careful, it assumes there is only one root vertex!!!
	 */
//	void setExecutableVertices(BaseVertex* vertex);

	// With no parameters, it just looks on the configuration vertices.
//	void setExecutableVertices();


	bool findVisitedIf(PiSDFIfVertex* ifVertex)
	{
		for(UINT32 i = 0; i < glbNbVisitedIfs; i++){
			if(visitedIfs[i] == ifVertex) return true;
		}
		return false;
	}

	/*
	 * Marks the vertices that can be executed.
	 */
	void copyRequiredEdges(BaseVertex* startVertex);


	// Copies vertices that can be executed to the outSDF graph.
//	void copyExecutableVertices(BaseVertex* startVertex, SDFGraph *outSDF);

	/*
	 * Runs through the graph to get executable vertices. If there are input vertices,
	 * the search is launched starting from each input vertex. If there is no input vertex,
	 * the search starts from the root vertex. In such case, it exits with an error
	 * if no root vertex has been defined.
	 */
//	void getExecutableVertices(SDFGraph *outSDF);


	// Links executable vertices that have been copied to outSDF.
//	void linkExecutableVertices(SDFGraph *outSDF);

	void connectExecVertices(SDFGraph *outSDF);

	void evaluateExpressions();

	void createSubGraph(SDFGraph* outSDF);

	void findRequiredEdges();

	void clearIntraIteration();

	/*
	 * Resets the visited vertices (the "visited" field is set to false),
	 * so that the "getSDFGraph" algorithm can re-examine the entire graph.
	 */
	void clearAfterVisit();


	/*
	 * Inserts round buffer vertices between configure vertices and normal vertices.
	 */
	void insertRoundBuffers();

	/*
	 * Creates SDF graph excluding the configure vertices.
	 */
	void createSDF(SDFGraph* outSDF);

	/*
	 * Creates SrDAG graph including only configure vertices.
	 */
	void createSrDAGConfigVertices(SRDAGGraph* outSrDAG);

	/*
	 * Auto-generated getters and setters.
	 */

    UINT32 getNb_parameters() const
    {
        return nb_parameters;
    }

    PiSDFParameter* getParameter(UINT64 index){
    	return &parameters[index];
    }

	UINT32 getNb_broad_vertices() const
	{
		return nb_broad_vertices;
	}

    BaseVertex* getBroad_vertex(UINT64 index)
    {
    	return &broad_vertices[index];
	}

	UINT32 getNb_config_vertices() const
	{
		return nb_config_vertices;
	}

	PiSDFConfigVertex* getConfig_vertex(UINT64 index)
	{
		return &config_vertices[index];
	}

	PiSDFConfigVertex* getListConfigVertices()
	{
		return &config_vertices[0];
	}

	UINT32 getNb_input_vertices() const
	{
		return nb_input_vertices;
	}

    PiSDFIfVertex* getInput_vertex(UINT64 index)
    {
        return &input_vertices[index];
    }

    PiSDFIfVertex* getInputVertex(PiSDFEdge *parentEdge)
    {
    	for(UINT32 i = 0; i < nb_input_vertices; i++)
    	{
    		if(input_vertices[i].getParentEdge() == parentEdge)
    			return &input_vertices[i];
    	}
    	return NULL;
    }

	UINT32 getNb_join_vertices() const
	{
		return nb_join_vertices;
	}

	BaseVertex* getJoin_vertex(UINT64 index)
    {
        return &join_vertices[index];
    }

	UINT32 getNb_output_vertices() const
	{
		return nb_output_vertices;
	}

    BaseVertex* getOutput_vertex(UINT64 index)
    {
        return &output_vertices[index];
    }

	UINT32 getNb_vertices() const
	{
		return nb_vertices;
	}

	UINT32 getNb_pisdf_vertices() const
	{
		return nb_pisdf_vertices;
	}

    PiSDFVertex* getPiSDFVertex(UINT64 index)
    {
        return &pisdf_vertices[index];
    }

	UINT32 getNb_edges() const
	{
		return nb_edges;
	}

	PiSDFEdge* getEdge(UINT64 index)
	{
		return &edges[index];
	}

    BaseVertex *getRootVertex() const
    {
    	if(rootVertex == NULL) exitWithCode(1056);
        return rootVertex;
    }

//    UINT32 getNbExecutableVertices() const
//	{
//		return PiSDFGraph::nbExecutableVertices;
//	}
//
//	BaseVertex** getExecutableVertices()
//	{
//		return ExecutableVertices;
//	}

    UINT32 getNb_select_vertices() const
    {
        return nb_select_vertices;
    }

    UINT32 getNb_switch_vertices() const
    {
        return nb_switch_vertices;
    }

    BaseVertex* getSelectVertex(UINT64 index)
    {
        return &select_vertices[index];
    }

    BaseVertex* getSwitchVertex(UINT64 index)
    {
        return &switch_vertices[index];
    }

//    bool getExecComplete() const
//    {
//        return execComplete;
//    }


    UINT32 getGlbNbConfigVertices() const
    {
    	return glbNbConfigVertices;
    }


    UINT32 getGlbNbExecConfigVertices() const
    {
    	return glbNbExecConfigVertices;
    }


    UINT32 getNbExecVertices() const
    {
    	return nbExecVertices;
    }


    UINT32 getNbDiscardVertices() const
    {
    	return nbDiscardVertices;
    }


    UINT32 getGlbNbExecVertices() const
    {
    	return glbNbExecVertices;
    }


    UINT32 getGlbNbRequiredEdges() const
    {
    	return glbNbRequiredEdges;
    }


//    void setExecComplete(bool execComplete)
//    {
//        this->execComplete = execComplete;
//    }

    void setNb_select_vertices(UINT32 nb_select_vertices)
    {
        this->nb_select_vertices = nb_select_vertices;
    }

    void setNb_switch_vertices(UINT32 nb_switch_vertices)
    {
        this->nb_switch_vertices = nb_switch_vertices;
    }


//        void setBroad_vertices(BaseVertex broad_vertices[32])
//        {
//            this->broad_vertices = broad_vertices;
//        }
//
//        void setConfig_vertices(PiSDFConfigVertex config_vertices[32])
//        {
//            this->config_vertices = config_vertices;
//        }
//
//        void setEdges(PiSDFEdge edges[32])
//        {
//            this->edges = edges;
//        }
//
//        void setInput_vertices(PiSDFIfVertex input_vertices[32])
//        {
//            this->input_vertices = input_vertices;
//        }
//
//        void setJoin_vertices(BaseVertex join_vertices[32])
//        {
//            this->join_vertices = join_vertices;
//        }
//
//        void setNb_broad_vertices(UINT32 nb_broad_vertices)
//        {
//            this->nb_broad_vertices = nb_broad_vertices;
//        }
//
//        void setNb_config_vertices(UINT32 nb_config_vertices)
//        {
//            this->nb_config_vertices = nb_config_vertices;
//        }
//
//        void setNb_edges(UINT32 nb_edges)
//        {
//            this->nb_edges = nb_edges;
//        }
//
//        void setNb_input_vertices(UINT32 nb_input_vertices)
//        {
//            this->nb_input_vertices = nb_input_vertices;
//        }
//
//        void setNb_join_vertices(UINT32 nb_join_vertices)
//        {
//            this->nb_join_vertices = nb_join_vertices;
//        }
//
//        void setNb_output_vertices(UINT32 nb_output_vertices)
//        {
//            this->nb_output_vertices = nb_output_vertices;
//        }
//
//        void setNb_parameters(UINT32 nb_parameters)
//        {
//            this->nb_parameters = nb_parameters;
//        }
//
//        void setNb_vertices(UINT32 nb_vertices)
//        {
//            this->nb_vertices = nb_vertices;
//        }
//
//        void setOutput_vertices(BaseVertex output_vertices[32])
//        {
//            this->output_vertices = output_vertices;
//        }
//
//        void setParameters(variable *parameters[32])
//        {
//            this->parameters = parameters;
//        }
//
//        void setVertices(PiSDFVertex vertices[32])
//        {
//            this->vertices = vertices;
//        }
//


    void setRootVertex(BaseVertex *rootVertex)
    {
        this->rootVertex = rootVertex;
    }

//    void setNbExecutableVertices(UINT32 nbExecutableVertices)
//    {
//        this->nbExecutableVertices = nbExecutableVertices;
//    }

};

#endif /* PISDFGRAPH_H_ */

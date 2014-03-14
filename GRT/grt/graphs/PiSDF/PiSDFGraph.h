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
#include <scheduling/ListScheduler/ListScheduler.h>
#include "launcher/launcher.h"
#include <tools/ExecutionStat.h>

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
	UINT32 nbExecVertices; 			// Counts the number of executable vertices.
	UINT32 nbDiscardVertices; 		// Counts the number of discarded vertices
									// e.g. a classic vertex with 0 production on an input edge.
						   // nbExecVertices = nb_vertices means the graph can be completely executed.
	bool executable;				// True when the graph can be completely executed.
public:
	PiSDFGraph();

	PiSDFEdge* 	addEdge(BaseVertex* source, const char* production, BaseVertex* sink, const char* consumption, const char* delay);

	PiSDFEdge*	addEdge(BaseVertex* source, abstract_syntax_elt* production, BaseVertex* sink, abstract_syntax_elt* consumption, abstract_syntax_elt* delay);

	BaseVertex* addVertex(const char* vertexName, VERTEX_TYPE type);

	PiSDFParameter*	addParameter(const char* name);

	void evaluateExpressions();


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
	 *  Says if Vx is an interface/round-buffer preceding a configure vx.
	 */
	bool isConfigVxPred(BaseVertex* Vx);

	/*
	 * Creates SDF graph excluding the configure vertices.
	 */
	void createSDF(SDFGraph* outSDF);

	/*
	 * Creates SrDAG graph including configure and input vertices.
	 */
	void createSrDAGInputConfigVxs(SRDAGGraph* outSrDAG, SRDAGVertex* hSrDagVx);


	void multiStepScheduling(BaseSchedule* schedule,
							ListScheduler* listScheduler,
							Architecture* arch,
							launcher* launch,
							ExecutionStat* execStat,
							SRDAGGraph* dag,
							SRDAGVertex* hSrDagVx,
							UINT32 level,
							UINT8* step);

	void solveParameters(SRDAGGraph* dag, BaseSchedule* schedule);

	void updateDAGStates(SRDAGGraph* dag);


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

//	PiSDFConfigVertex* getListConfigVertices()
//	{
//		return &config_vertices[0];
//	}
//
	UINT32 getNb_input_vertices() const
	{
		return nb_input_vertices;
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

    void setRootVertex(BaseVertex *rootVertex)
    {
        this->rootVertex = rootVertex;
    }

	UINT32 getNbRoundBVertices() const {
		return nb_roundB_vertices;
	}

	void setNbRoundBVertices(UINT32 nbRoundBVertices) {
		nb_roundB_vertices = nbRoundBVertices;
	}

	UINT32 getNbSelectVertices() const {
		return nb_select_vertices;
	}

	void setNbSelectVertices(UINT32 nbSelectVertices) {
		nb_select_vertices = nbSelectVertices;
	}

	UINT32 getNbSwitchVertices() const {
		return nb_switch_vertices;
	}

	void setNbSwitchVertices(UINT32 nbSwitchVertices) {
		nb_switch_vertices = nbSwitchVertices;
	}

	BaseVertex* getRoundBVertex(UINT32 index) {
		return &roundB_vertices[index];
	}

	BaseVertex* getSelectVertex(UINT32 index) {
		return &select_vertices[index];
	}

	BaseVertex* getSwitchVertex(UINT32 index) {
		return &switch_vertices[index];
	}

	const BaseVertex* getRootVertex() const {
		return rootVertex;
	}
};

#endif /* PISDFGRAPH_H_ */

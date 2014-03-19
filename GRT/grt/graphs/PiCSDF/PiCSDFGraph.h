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

#ifndef PICSDFGRAPH_H_
#define PICSDFGRAPH_H_

#include "../CSDAG/CSDAGGraph.h"
#include "PiCSDFEdge.h"


/*
 * This data types may become classes later.
 */
// Parameter
typedef struct PiCSDFParameter{
	char 	name[MAX_VERTEX_NAME_SIZE];
	abstract_syntax_elt expression[REVERSE_POLISH_STACK_MAX_ELEMENTS+1]; //expression defining the parameter's value.
}PiCSDFParameter;


// ConfigPort
typedef struct PiCSDFConfigPort{
	CSDAGVertex* 		vertex;
	PiCSDFParameter* 	parameter;
	int 				direction; // 0:input, 1:output.
}PiCSDFConfigPort;


class PiCSDFGraph : public CSDAGGraph{
	private:

		/*
		 * Table of configuration actors.
		 */
		CSDAGVertex configVertices[MAX_NB_PiSDF_CONFIG_VERTICES];
		int nbConfigVertices;

		/**
		 table of PiCSDF edges
		*/
		PiCSDFEdge edges[MAX_CSDAG_EDGES];

		/*
		 * Table of configuration input ports.
		 */
		PiCSDFConfigPort configInPorts[MAX_NB_PiSDF_INPUT_VERTICES];
		int nbConfigInPorts;

		/*
		 * Table of configuration output ports.
		 */
		PiCSDFConfigPort configOutPorts[MAX_NB_PiSDF_OUTPUT_VERTICES];
		int nbConfigOutPorts;

		/*
		 * Table of parameters.
		 */
		PiCSDFParameter parameters[MAX_NB_PiSDF_PARAMS];
		int nbParameters;

	public:
		PiCSDFGraph():CSDAGGraph(){nbConfigVertices	= 0;
								   nbConfigInPorts	= 0;
								   nbConfigOutPorts	= 0;
								   nbParameters		= 0;};

		/**
		 Adding an edge to the graph. Vertices and edges must be added in topological order.

		 @param source: The source vertex of the edge
		 @param production: number of tokens (chars) produced by the source
		 @param sink: The sink vertex of the edge
		 @param consumption: number of tokens (chars) consumed by the sink
		 @param delay: number of initial tokens.
		 @return the created edge
		*/
		PiCSDFEdge* addEdge(CSDAGVertex* source, const char* production, CSDAGVertex* sink, const char* consumption, const char* delay);


		/**
		 Gets the edge at the given index

		 @param index: index of the edge in the edge table
		 @return edge
		*/
		PiCSDFEdge* getEdge(int index);


		/**
		 Gets the input edges of a given vertex

		 @param vertex: input vertex
		 @param output: table to store the edges
		 @return the number of input edges
		*/
		int getInputEdges(CSDAGVertex* vertex, PiCSDFEdge** output);


		/**
		 Adding a configuration vertex to the graph.

		 @param vertexName: the name of the new vertex
		 @return the new vertex
		*/
		CSDAGVertex* addConfigVertex(const char* vertexName);


		/**
		 * Get number of configuration vertices.
		 */
		int getNbConfigVertices();


		/*
		 * Get a configuration vertex
		 */
		CSDAGVertex* getConfigVertex(const int index);

		/**
		 Adding a configuration port to the graph

		 @param vertex: pointer to the vertex connected to the port.
		 	 	param:	pointer to the parameter connected to the port.
		 	 	dir:	port direction. 0:input, 1:output.

		 @return the new configuration port.
		*/
		PiCSDFConfigPort* addConfigPort(CSDAGVertex* vertex, PiCSDFParameter* param, int dir);


		/*
		 * Get a configuration input port
		 */
		PiCSDFConfigPort* getConfigInPort(int index);


		/*
		 * Get a configuration output port
		 */
		PiCSDFConfigPort* getConfigOutPort(int index);


		/**
		 * Getter of the number of configuration input ports.
		 */
		int getNbConfigInPorts();



		/**
		 * Getter of the number of configuration output ports.
		 */
		int getNbConfigOutPorts();



		/**
		 Adding a parameter to the graph

		 @param expression: //expression defining the parameter's value.

		 @return the new parameter.
		*/
		PiCSDFParameter* addParameter(const char* name, const char* expression);

		int getNbParameters();

		PiCSDFParameter* getParameter(const int index);
};



/**
 Gets the edge at the given index

 @param index: index of the edge in the edge list
 @return edge
*/
inline PiCSDFEdge* PiCSDFGraph::getEdge(int index){
	return &edges[index];
}


/**
 Gets the input edges of a given vertex

 @param vertex: input vertex
 @param output: table to store the edges
 @return the number of input edges
*/
inline int PiCSDFGraph::getInputEdges(CSDAGVertex* vertex, PiCSDFEdge** output){
	int nbEdges = 0;
	for(int i=0; i<this->nbEdges; i++){
		PiCSDFEdge* edge = &edges[i];
		CSDAGVertex* sink = edge->getSink();
		if(sink == vertex){
			output[nbEdges] = edge;
			nbEdges++;
		}
	}
	return nbEdges;
}


/**
 Gets the configuration vertex at the given index.
*/
inline CSDAGVertex* PiCSDFGraph::getConfigVertex(const int index){
	return &configVertices[index];
}


/**
 Gets the number of parameters.
*/
inline int PiCSDFGraph::getNbParameters(){
	return nbParameters;
}


/**
 Gets the parameter at the given index.
*/
inline PiCSDFParameter* PiCSDFGraph::getParameter(const int index){
	return &parameters[index];
}

#endif /* PICSDFGRAPH_H_ */

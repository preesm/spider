/*
 * PiCSDFGraph.h
 *
 *  Created on: 12 juin 2013
 *      Author: yoliva
 */

#ifndef PICSDFGRAPH_H_
#define PICSDFGRAPH_H_

#include "../CSDAG/CSDAGGraph.h"
#include "PiCSDFEdge.h"


/*
 * This data types may become classes later.
 */
// Parameter
typedef struct PiCSDFParameter{
//	char 	name[MAX_VERTEX_NAME_SIZE];
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
		/**
		 table of PiCSDF edges
		*/
		PiCSDFEdge edges[MAX_CSDAG_EDGES];

		/*
		 * Table of configuration ports.
		 */
		PiCSDFConfigPort configPorts[MAX_PISDF_CONFIG_PORTS];
		int nbConfigPorts;

		/*
		 * Table of parameters.
		 */
		PiCSDFParameter parameters[MAX_PISDF_CONFIG_PORTS];
		int nbParameters;

	public:
		PiCSDFGraph():CSDAGGraph(){nbConfigPorts = 0; nbParameters = 0;};

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
		 Adding a configuration port to the graph

		 @param vertex: pointer to the vertex connected to the port.
		 	 	param:	pointer to the parameter connected to the port.
		 	 	dir:	port direction. 0:input, 1:output.

		 @return the new configuration port.
		*/
		PiCSDFConfigPort* addConfigPort(CSDAGVertex* vertex, PiCSDFParameter* param, int dir);


		/**
		 Adding a parameter to the graph

		 @param expression: //expression defining the parameter's value.

		 @return the new parameter.
		*/
		PiCSDFParameter* addParameter(const char* expression);
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





#endif /* PICSDFGRAPH_H_ */

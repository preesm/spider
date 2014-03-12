/*
 * PiSDFTransformer.h
 *
 *  Created on: 12 juin 2013
 *      Author: yoliva
 */

#ifndef PISDFTRANSFORMER_H_
#define PISDFTRANSFORMER_H_

#include "../CSDAGTransformer/CSDAGTransformer.h"
#include "../../graphs/PiCSDF/PiCSDFGraph.h"

class PiCSDFTransformer : public CSDAGTransformer{
	private:
		/**
		 Table of input DAG edges of an PiCSDF vertex.
		*/
		PiCSDFEdge* input_edges[MAX_CSDAG_INPUT_EDGES];

	public:

		/**
		 * Adds "nb_repetitions" SRDAGVertex vertices for the "csdag_vertex" vertex to the output SRDAG graph.
		 */
		void addVertices(PiCSDFGraph* inputGraph, CSDAGVertex* csdag_vertex, int nb_repetitions, SRDAGGraph* outputGraph);


		/**
		 Transforms a PiCSDF graph in SrSDF

		 @param input: input PiCSDF graph with pattern expressions on production, consumption and delays.
		 @param output: output SrSDD graph with variable expressions on production and consumption???
		*/
		void transform(PiCSDFGraph* input, SRDAGGraph* output, Architecture* archi);


		void linkvertices(PiCSDFGraph* inputGraph, SRDAGGraph* outputGraph);


};
#endif /* PISDFTRANSFORMER_H_ */

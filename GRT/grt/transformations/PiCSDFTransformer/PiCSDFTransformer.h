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

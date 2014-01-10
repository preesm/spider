
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#ifndef PISDFTRANSFORMER_H_
#define PISDFTRANSFORMER_H_

#include <graphs/Base/BaseVertex.h>

#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGVertex.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/SDF/SDFGraph.h"


class PiSDFTransformer {
	/**
	 Basis repetition vector for the original vertices.
	*/
	int brv[MAX_NB_VERTICES];
	/**
	 Different SRDAG repetitions of an CSDAG vertex source to generate edges
	 */
	SRDAGVertex* sourceRepetitions[MAX_VERTEX_REPETITION];
	/**
	 Different SRDAG repetitions of an CSDAG vertex sink to generate edges
	 */
	SRDAGVertex* sinkRepetitions[MAX_VERTEX_REPETITION];
public:
	PiSDFTransformer() {
		// TODO Auto-generated constructor stub

	}
	~PiSDFTransformer() {
		// TODO Auto-generated destructor stub
	}


	/**
	 * Adds "nb_repetitions" SRDAGVertex vertices for "vertex" to the output SRDAG graph.
	 */
	void addVertices(BaseVertex* vertex, int nb_repetitions, SRDAGGraph* outputGraph);


	/*
	 * Connects vertices to form a SrDAG.
	 */
	void linkvertices(BaseVertex** vertices, UINT32 nbVertices, SRDAGGraph* outputGraph);

	void linkvertices(SDFGraph* sdf, SRDAGGraph* outputGraph);

	/**
	 Transforms a PiSDF graph in Sr

	 @param input: input PiSDF graph with pattern expressions on production, consumption and delays.
	 @param output: output SrSDD graph with variable expressions on production and consumption???
	*/
//	void transform(PiCSDFGraph* input_graph, SRDAGGraph* output_graph, Architecture* archi);

	/**
	 Transforms a set of PiSDF vertices into a Sr graph.
	*/
	void transform(BaseVertex **vertices, UINT32 nbVertices, SRDAGGraph *srGraph);

	/**
	 Transforms a SDF graph into a Sr graph.
	*/
	void transform(SDFGraph *sdf, SRDAGGraph *srGraph);

};

#endif /* PISDFTRANSFORMER_H_ */

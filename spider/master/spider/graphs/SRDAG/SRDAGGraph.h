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

#ifndef SRDAG_GRAPH_H
#define SRDAG_GRAPH_H

#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/SRDAG/SRDAGCommon.h>

class SRDAGGraph {
public:
	SRDAGGraph();
	SRDAGGraph(Stack *stack);
	virtual ~SRDAGGraph();

	SRDAGVertex* addVertex(PiSDFVertex* reference);
	SRDAGVertex* addBroadcast(int nOutput);
	SRDAGVertex* addFork(int nOutput);
	SRDAGVertex* addJoin(int nInput);
	SRDAGVertex* addInit();
	SRDAGVertex* addEnd();
	SRDAGVertex* addRoundBuffer();

	SRDAGEdge* addEdge();
	SRDAGEdge* addEdge(
			SRDAGVertex* src, int srcPortIx,
			SRDAGVertex* snk, int snkPortIx,
			int rate);

	void delVertex(SRDAGVertex* vertex);
	void delEdge(SRDAGEdge* edge);

	/** Iterator getters */
	inline SRDAGEdgeIterator getEdgeIterator();
	inline SRDAGVertexIterator getVertexIterator();

	/** Element getters */
	inline SRDAGEdge* getEdge(int ix);
	inline SRDAGVertex* getVertex(int ix);

	inline int getIxOfEdge(SRDAGEdge* edge);
	inline int getIxOfVertex(SRDAGVertex* vertex);

	/** Size getters */
	inline int getNEdge();
	inline int getNVertex();
	int getNExecVertex();

	/** Print Fct */
	void print(const char *path);
	bool check();

	void updateState();

private:
	SRDAGEdgeSet edges_;
	SRDAGVertexSet vertices_;
	Stack* stack_;
};

/** Inline Fcts */
/** Iterator getters */
inline SRDAGEdgeIterator SRDAGGraph::getEdgeIterator(){
	return edges_.getIterator();
}
inline SRDAGVertexIterator SRDAGGraph::getVertexIterator(){
	return vertices_.getIterator();
}

/** Element getters */
inline SRDAGEdge* SRDAGGraph::getEdge(int ix){
	return edges_[ix];
}
inline SRDAGVertex* SRDAGGraph::getVertex(int ix){
	return vertices_[ix];
}

inline int SRDAGGraph::getIxOfEdge(SRDAGEdge* edge){
	return edges_.getIxOf(edge);
}
inline int SRDAGGraph::getIxOfVertex(SRDAGVertex* vertex){
	return vertices_.getIxOf(vertex);
}

/** Size getters */
inline int SRDAGGraph::getNEdge(){
	return edges_.getN();
}
inline int SRDAGGraph::getNVertex(){
	return vertices_.getN();
}

#endif/*SRDAG_GRAPH_H*/

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

#include "BipartiteGraph.h"
#include <tools/Stack.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>

#include <cstring>

BipartiteGraph::BipartiteGraph() {
	graph_ = 0;
	nVertices_ = 0;
	nConnections_ = 0;
	stack_ = 0;
}

BipartiteGraph::BipartiteGraph(SRDAGGraph g1, SRDAGGraph g2, Stack* stack){
	nVertices_ = g1.getNVertex();
	int nVerticesG2 = g2.getNVertex();
	graph_ = sAlloc(stack, nVertices_*nVerticesG2, int);
	nConnections_ = sAlloc(stack, nVertices_, int);
	memset(nConnections_, 0, nVertices_*sizeof(int));
	stack_ = stack;

	SRDAGVertexIterator vertexItG1 = g1.getVertexIterator();
	FOR_IT(vertexItG1){
		SRDAGVertex* vertexG1 = vertexItG1.current();
		SRDAGVertexIterator vertexItG2 = g2.getVertexIterator();
		FOR_IT(vertexItG2){
			SRDAGVertex* vertexG2 = vertexItG2.current();
			if(vertexG1->match(vertexG2)){
				int ixG1 = g1.getIxOfVertex(vertexG1);
				int ixG2 = g2.getIxOfVertex(vertexG2);
				graph_[ixG1 + nConnections_[ixG1]] = ixG2;
				nConnections_[ixG1]++;
			}
		}
	}
}

BipartiteGraph::~BipartiteGraph() {
}

int BipartiteGraph::maxMatching(BipartiteGraph* graph) {
    int* matching = sAlloc(graph->stack_, graph->nVertices_, int);
    memset(matching, -1, graph->nVertices_*sizeof(int));
    int matches = 0;
    bool* visited = sAlloc(graph->stack_, graph->nVertices_, bool);
    for (int u = 0; u < graph->nVertices_; u++) {
        memset(visited, false, graph->nVertices_*sizeof(bool));
      if (findPath(graph, u, matching, visited))
        ++matches;
    }
    return matches;
  }

 bool BipartiteGraph::findPath(BipartiteGraph* graph, int u1, int* matching, bool* vis) {
    vis[u1] = true;
    for (int i=0; i<graph->nConnections_[u1]; i++) {
      int v = graph->graph_[u1*graph->nVertices_ + i];
      int u2 = matching[v];
      if (u2 == -1 || (!vis[u2] && findPath(graph, u2, matching, vis))) {
        matching[v] = u1;
        return true;
      }
    }
    return false;
  }

bool BipartiteGraph::match(){
	return maxMatching(this) == nVertices_;
}

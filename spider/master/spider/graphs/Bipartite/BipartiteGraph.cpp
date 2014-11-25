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
#include <cstdio>
#include <platform_file.h>

BipartiteGraph::BipartiteGraph() {
	graph_ = 0;
	nVertices_ = 0;
	nConnections_ = 0;
	stack_ = 0;
}

BipartiteGraph::BipartiteGraph(SRDAGGraph* g1, SRDAGGraph* g2, Stack* stack){
	nVertices_ = g1->getNVertex();
	int nVerticesG2 = g2->getNVertex();
	graph_ = sAlloc(stack, nVertices_*nVerticesG2, int);
	nConnections_ = sAlloc(stack, nVertices_, int);
	memset(nConnections_, 0, nVertices_*sizeof(int));
	stack_ = stack;

	SRDAGVertexIterator vertexItG1 = g1->getVertexIterator();
	FOR_IT(vertexItG1){
		SRDAGVertex* vertexG1 = vertexItG1.current();
		int ixG1 = g1->getIxOfVertex(vertexG1);
		SRDAGVertexIterator vertexItG2 = g2->getVertexIterator();
		FOR_IT(vertexItG2){
			SRDAGVertex* vertexG2 = vertexItG2.current();
			int ixG2 = g2->getIxOfVertex(vertexG2);
			if(vertexG1->match(vertexG2)){
				graph_[ixG1*nVertices_ + nConnections_[ixG1]] = ixG2;
				nConnections_[ixG1]++;
			}
		}
	}
}

BipartiteGraph::~BipartiteGraph() {
}

bool BipartiteGraph::hasPerfectMatch() {
    int* matching = sAlloc(stack_, nVertices_, int);
    memset(matching, -1, nVertices_*sizeof(int));
    int matches = 0;
    bool* visited = sAlloc(stack_, nVertices_, bool);
    for (int u = 0; u < nVertices_; u++) {
        memset(visited, false, nVertices_*sizeof(bool));
      if (!findPath(this, u, matching, visited))
        return false;
    }
    return true;
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

bool BipartiteGraph::compareGraphs(SRDAGGraph* g1, SRDAGGraph* g2, Stack* stack){
	if(g1->getNVertex() != g2->getNVertex()
			|| g1->getNEdge() != g2->getNEdge())
		return false;

	BipartiteGraph* bipartite = sAlloc(stack, 1, BipartiteGraph);
	*bipartite = BipartiteGraph(g1, g2, stack);
//	bipartite->print("bipart.gv");

//	g1->print("g1.gv");
//	g2->print("g2.gv");

	return bipartite->hasPerfectMatch();
}

void BipartiteGraph::print(const char* path){
	int maxId;
	int file = platform_fopen (path);
	if(file == -1){
		printf("cannot open %s\n", path);
		return;
	}

	// Writing header
	platform_fprintf (file, "digraph csdag {\n");
	platform_fprintf (file, "\tnode [color=\"#433D63\"];\n");
	platform_fprintf (file, "\tedge [color=\"#9262B6\" arrowhead=\"empty\"];\n");
	platform_fprintf (file, "\trankdir=LR;\n\n");

	// Drawing vertices.
	platform_fprintf (file, "\t# Vertices\n");

	platform_fprintf (file, "\tsubgraph cluster_0 {\nlabel = \"Graph 1\";\n");
	for (int i=0; i<nVertices_; i++){
		platform_fprintf (file, "\t\tg1_%d [shape=ellipse,label=\"g1_%d\"];\n", i, i);
	}
	platform_fprintf (file, "\t}\n");

	platform_fprintf (file, "\tsubgraph cluster_1 {\nlabel = \"Graph 2\";\n");
	for (int i=0; i<nVertices_; i++){
		platform_fprintf (file, "\t\tg2_%d [shape=ellipse,label=\"g2_%d\"];\n", i, i);
	}
	platform_fprintf (file, "\t}\n");

	// Drawing edges.
	platform_fprintf (file, "\t# Edges\n");
	for (int i=0; i<nVertices_; i++) {
		for (int j=0; j<nConnections_[i]; j++) {
			int snkIx, srcIx;
			platform_fprintf (file, "\tg1_%d->g2_%d;\n", i, graph_[i*nVertices_+j]);
		}
	}

	platform_fprintf (file, "}\n");
	platform_fclose(file);
}

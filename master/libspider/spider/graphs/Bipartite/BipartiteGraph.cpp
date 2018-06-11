/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2015)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#ifdef _MSC_VER
#if (_MSC_VER < 1900)
#define snprintf _snprintf
#endif
#endif

#include "BipartiteGraph.h"

BipartiteGraph::BipartiteGraph(SRDAGGraph *g1, SRDAGGraph *g2, SpiderStack stackId) {
    nVerticesG1_ = g1->getNVertex();
    nVerticesG2_ = g2->getNVertex();
    graph_ = CREATE_MUL(stackId, nVerticesG1_ * nVerticesG2_, int);
    nConnections_ = CREATE_MUL(stackId, nVerticesG1_, int);
    memset(nConnections_, 0, nVerticesG1_ * sizeof(int));
    stackId_ = stackId;

    for (int ixG1 = 0; ixG1 < nVerticesG1_; ixG1++) {
        SRDAGVertex *vertexG1 = g1->getVertex(ixG1);
        for (int ixG2 = 0; ixG2 < nVerticesG2_; ixG2++) {
            SRDAGVertex *vertexG2 = g2->getVertex(ixG2);
            if (vertexG1->match(vertexG2)) {
                graph_[ixG1 * nVerticesG2_ + nConnections_[ixG1]] = ixG2;
                nConnections_[ixG1]++;
            }
        }
    }
}

BipartiteGraph::~BipartiteGraph() {
    StackMonitor::free(stackId_, graph_);
    StackMonitor::free(stackId_, nConnections_);
}

bool BipartiteGraph::hasPerfectMatch() {
    int *matching = CREATE_MUL(stackId_, nVerticesG1_, int);
    bool *visited = CREATE_MUL(stackId_, nVerticesG1_, bool);

    memset(matching, -1, nVerticesG1_ * sizeof(int));

    for (int u = 0; u < nVerticesG1_; u++) {
        memset(visited, false, nVerticesG1_ * sizeof(bool));
        if (!findPath(this, u, matching, visited)) {
            StackMonitor::free(stackId_, matching);
            StackMonitor::free(stackId_, visited);
            return false;
        }
    }
    StackMonitor::free(stackId_, matching);
    StackMonitor::free(stackId_, visited);
    return true;
}

bool BipartiteGraph::findPath(BipartiteGraph *graph, int u1, int *matching, bool *vis) {
    vis[u1] = true;
    for (int i = 0; i < graph->nConnections_[u1]; i++) {
        int v = graph->graph_[u1 * graph->nVerticesG1_ + i];
        int u2 = matching[v];
        if (u2 == -1 || (!vis[u2] && findPath(graph, u2, matching, vis))) {
            matching[v] = u1;
            return true;
        }
    }
    return false;
}

void BipartiteGraph::compareGraphs(SRDAGGraph *g1, SRDAGGraph *g2, SpiderStack stackId, const char *testName) {

    BipartiteGraph *bipartite = CREATE(stackId, BipartiteGraph)(g1, g2, stackId);

    printf("%s : ", testName);
    if (g1->getNVertex() == g2->getNVertex()
        && g1->getNEdge() == g2->getNEdge()
        && bipartite->hasPerfectMatch()) {
        printf("Ok\n");
    } else {
        printf("Failed !\n");
        char name[100];
        snprintf(name, 100, "%s_bipart.gv", testName);
        bipartite->print(name, g1, g2);

        snprintf(name, 100, "%s_get.gv", testName);
        g1->print(name);

        snprintf(name, 100, "%s_model.gv", testName);
        g2->print(name);
    }

    bipartite->~BipartiteGraph();
    StackMonitor::free(stackId, bipartite);
}

void BipartiteGraph::print(const char *path, SRDAGGraph *g1, SRDAGGraph *g2) {
    FILE *file = Platform::get()->fopen(path);
    if (file == NULL) {
        printf("cannot open %s\n", path);
        return;
    }

    // Writing header
    Platform::get()->fprintf(file, "digraph csdag {\n");
    Platform::get()->fprintf(file, "\tnode [color=\"#433D63\"];\n");
    Platform::get()->fprintf(file, "\tedge [color=\"#9262B6\" arrowhead=\"empty\"];\n");
    Platform::get()->fprintf(file, "\trankdir=LR;\n\n");

    // Drawing vertices.
    Platform::get()->fprintf(file, "\t# Vertices\n");

    Platform::get()->fprintf(file, "\tsubgraph cluster_0 {\nlabel = \"Get\";\n");
    for (int i = 0; i < nVerticesG1_; i++) {
        char name[100];
        g1->getVertex(i)->toString(name, 100);
        Platform::get()->fprintf(file, "\t\tg1_%d [shape=ellipse,label=\"%s\"];\n", i, name);
    }
    Platform::get()->fprintf(file, "\t}\n");

    Platform::get()->fprintf(file, "\tsubgraph cluster_1 {\nlabel = \"Model\";\n");
    for (int i = 0; i < nVerticesG2_; i++) {
        char name[100];
        g2->getVertex(i)->toString(name, 100);
        Platform::get()->fprintf(file, "\t\tg2_%d [shape=ellipse,label=\"%s\"];\n", i, name);
    }
    Platform::get()->fprintf(file, "\t}\n");

    // Drawing edges.
    Platform::get()->fprintf(file, "\t# Edges\n");
    for (int i = 0; i < nVerticesG1_; i++) {
        for (int j = 0; j < nConnections_[i]; j++) {
            Platform::get()->fprintf(file, "\tg1_%d->g2_%d;\n", i, graph_[i * nVerticesG2_ + j]);
        }
    }

    Platform::get()->fprintf(file, "}\n");
    Platform::get()->fclose(file);
}

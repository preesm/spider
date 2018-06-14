/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2018)
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
#include <graphs/SRDAG/SRDAGCommon.h>
#include <graphs/SRDAG/SRDAGEdge.h>
#include <graphs/SRDAG/SRDAGGraph.h>

#include <algorithm>

#define MAX_VERTEX (10000)
#define MAX_EDGE (10000)

static const char *stateStrings[3] = {
        "NOT_EXEC",
        "EXEC",
        "RUN"
};

SRDAGGraph::SRDAGGraph() :
        idEdge_(0),
        idVertex_(0),
        edges_(MAX_EDGE, SRDAG_STACK),
        vertices_(MAX_VERTEX, SRDAG_STACK) {
}

SRDAGGraph::~SRDAGGraph() {
    while (edges_.getN()) {
        delEdge(edges_[0]);
    }
    while (vertices_.getN()) {
        delVertex(vertices_[0]);
    }
}

SRDAGVertex *SRDAGGraph::addVertex(PiSDFVertex *reference, int refId, int iterId) {
    SRDAGVertex *vertex = CREATE(SRDAG_STACK, SRDAGVertex)(
            idVertex_++,
            SRDAG_NORMAL, this,
            reference, refId, iterId,
            reference->getNInEdge(),
            reference->getNOutEdge(),
            reference->getNInParam(),
            reference->getNOutParam());
    vertices_.add(vertex);
    return vertex;
}

SRDAGVertex *SRDAGGraph::addBroadcast(int nOutput, PiSDFVertex *reference) {
    SRDAGVertex *vertex = CREATE(SRDAG_STACK, SRDAGVertex)(
            idVertex_++,
            SRDAG_BROADCAST, this,
            reference /*Ref*/, 0, 0,
            1 /*nInEdge*/,
            nOutput /*nOutEdge*/,
            0 /*nInParam*/,
            0 /*nOutParam*/);
    vertices_.add(vertex);
    return vertex;
}

SRDAGVertex *SRDAGGraph::addFork(int nOutput) {
    SRDAGVertex *vertex = CREATE(SRDAG_STACK, SRDAGVertex)(
            idVertex_++,
            SRDAG_FORK, this,
            0 /*Ref*/, 0, 0,
            1 /*nInEdge*/,
            nOutput /*nOutEdge*/,
            0 /*nInParam*/,
            0 /*nOutParam*/);
    vertices_.add(vertex);
    return vertex;
}

SRDAGVertex *SRDAGGraph::addJoin(int nInput) {
    SRDAGVertex *vertex = CREATE(SRDAG_STACK, SRDAGVertex)(
            idVertex_++,
            SRDAG_JOIN, this,
            0 /*Ref*/, 0, 0,
            nInput /*nInEdge*/,
            1 /*nOutEdge*/,
            0 /*nInParam*/,
            0 /*nOutParam*/);
    vertices_.add(vertex);
    return vertex;
}

SRDAGVertex *SRDAGGraph::addInit() {
    SRDAGVertex *vertex = CREATE(SRDAG_STACK, SRDAGVertex)(
            idVertex_++,
            SRDAG_INIT, this,
            0 /*Ref*/, 0, 0,
            0 /*nInEdge*/,
            1 /*nOutEdge*/,
            2 /*nInParam*/,
            0 /*nOutParam*/);
    vertices_.add(vertex);
    return vertex;
}

SRDAGVertex *SRDAGGraph::addEnd() {
    SRDAGVertex *vertex = CREATE(SRDAG_STACK, SRDAGVertex)(
            idVertex_++,
            SRDAG_END, this,
            0 /*Ref*/, 0, 0,
            1 /*nInEdge*/,
            0 /*nOutEdge*/,
            2 /*nInParam: 0 -> isDelayPersistent, 1 -> memory address of the fifo (if any)*/,
            0 /*nOutParam*/);
    vertices_.add(vertex);
    return vertex;
}

SRDAGVertex *SRDAGGraph::addRoundBuffer() {
    SRDAGVertex *vertex = CREATE(SRDAG_STACK, SRDAGVertex)(
            idVertex_++,
            SRDAG_ROUNDBUFFER, this,
            0 /*Ref*/, 0, 0,
            1 /*nInEdge*/,
            1 /*nOutEdge*/,
            0 /*nInParam*/,
            0 /*nOutParam*/);
    vertices_.add(vertex);
    return vertex;
}

SRDAGEdge *SRDAGGraph::addEdge() {
    SRDAGEdge *edge = CREATE(SRDAG_STACK, SRDAGEdge)(this, idEdge_++);
    edges_.add(edge);
    return edge;
}

SRDAGEdge *SRDAGGraph::addEdge(
        SRDAGVertex *src, int srcPortIx,
        SRDAGVertex *snk, int snkPortIx,
        int rate) {
    SRDAGEdge *edge = CREATE(SRDAG_STACK, SRDAGEdge)(this, idEdge_++);
    edges_.add(edge);

    edge->connectSrc(src, srcPortIx);
    edge->connectSnk(snk, snkPortIx);
    edge->setRate(rate);

    return edge;
}

void SRDAGGraph::delVertex(SRDAGVertex *vertex) {
    int i = 0;
    while (vertex->getNConnectedInEdge() > 0) {
        if (vertex->getInEdge(i) != 0)
            vertex->getInEdge(i)->disconnectSnk();
        i++;
    }

    i = 0;
    while (vertex->getNConnectedOutEdge() > 0) {
        if (vertex->getOutEdge(i) != 0)
            vertex->getOutEdge(i)->disconnectSrc();
        i++;
    }

    vertices_.del(vertex);
    vertex->~SRDAGVertex();
    StackMonitor::free(SRDAG_STACK, vertex);
}

void SRDAGGraph::delEdge(SRDAGEdge *edge) {
    if (edge->getSrc() != 0) {
        edge->disconnectSrc();
    }
    if (edge->getSnk() != 0) {
        edge->disconnectSnk();
    }
    edges_.del(edge);
    edge->~SRDAGEdge();
    StackMonitor::free(SRDAG_STACK, edge);
}

int SRDAGGraph::getNExecVertex() {
    int n = 0;
    for (int i = 0; i < vertices_.getN(); i++) {
        if (vertices_[i]->getState() == SRDAG_EXEC)
            n++;
    }
    return n;
}

void SRDAGGraph::updateState() {
    for (int i = 0; i < vertices_.getN(); i++) {
        vertices_[i]->updateState();
    }
}

/** Print Fct */
void SRDAGGraph::print(const char *path) {

    if (!check())
        printf("Errors in the SRDAG Graph\n");

    int maxId = -1;
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
    for (int i = 0; i < vertices_.getN(); i++) {
        char name[100];
        SRDAGVertex *vertex = vertices_[i];
        vertex->toString(name, 100);
        Platform::get()->fprintf(file, "\t%d [shape=ellipse,label=\"%d\\n%s (%d)\n%s",
                                 vertex->getId(),
                                 vertex->getId(),
                                 name,
                                 vertex->getFctId(),
                                 stateStrings[vertex->getState()]);
        Platform::get()->fprintf(file, "\",color=");
        switch (vertex->getState()) {
            case SRDAG_EXEC:
                Platform::get()->fprintf(file, "blue");
                break;
            case SRDAG_RUN:
                Platform::get()->fprintf(file, "gray");
                break;
            case SRDAG_NEXEC:
                if (vertex->isHierarchical())
                    Platform::get()->fprintf(file, "red");
                else
                    Platform::get()->fprintf(file, "black");
                break;
        }
        Platform::get()->fprintf(file, "];\n");

        maxId = std::max(vertex->getId(), maxId);
    }

    // Drawing edges.
    Platform::get()->fprintf(file, "\t# Edges\n");
    for (int i = 0; i < edges_.getN(); i++) {
        SRDAGEdge *edge = edges_[i];
        int snkIx, srcIx;

        if (edge->getSrc())
            srcIx = edge->getSrc()->getId();
        else {
            maxId++;
            Platform::get()->fprintf(file, "\t%d [shape=point];\n", maxId);
            srcIx = maxId;
        }
        if (edge->getSnk())
            snkIx = edge->getSnk()->getId();
        else {
            maxId++;
            Platform::get()->fprintf(file, "\t%d [shape=point];\n", maxId);
            snkIx = maxId;
        }

//		switch(mode){
//		case DataRates:
        Platform::get()->fprintf(file,
                                 "\t%d->%d [label=\"%d (ID%d)\n%#x\",taillabel=\"%d\",headlabel=\"%d\"];\n",
                                 srcIx, snkIx,
                                 edge->getRate(),
                                 edge->getId(),
                                 edge->getAlloc(),
                                 edge->getSrcPortIx(),
                                 edge->getSnkPortIx());
//			break;
//		case Allocation:
//			Platform::get()->fprintf(file, "\t%d->%d [label=\"%d: %#x (%#x)\"];\n",
//					srcIx, snkIx,
//					edge->fifo.id,
//					edge->fifo.add,
//					edge->fifo.size);
//			break;
//		}

    }


    Platform::get()->fprintf(file, "}\n");
    Platform::get()->fclose(file);
}

bool SRDAGGraph::check() {
    bool result = true;

    /* Check vertices */
    for (int i = 0; i < vertices_.getN(); i++) {
        SRDAGVertex *vertex = vertices_[i];
        int j;

        // Check input edges
        for (j = 0; j < vertex->getNConnectedInEdge(); j++) {
            const SRDAGEdge *edge = vertex->getInEdge(j);
            if (vertex->getType() == SRDAG_JOIN
                && edge == NULL) {
                printf("Warning V%d Input%d: not connected\n", vertex->getId(), j);
            } else if (edge == NULL) {
                printf("Error V%d Input%d: not connected\n", vertex->getId(), j);
                result = false;
            } else if (edge->getSnk() != vertex) {
                printf("Error V%d E%d: connection mismatch\n", vertex->getId(), edge->getId());
                result = false;
            }
        }

        // Check output edges
        for (j = 0; j < vertex->getNConnectedOutEdge(); j++) {
            const SRDAGEdge *edge = vertex->getOutEdge(j);
            if (vertex->getType() == SRDAG_FORK
                && edge == NULL) {
                printf("Warning V%d Output%d: not connected\n", vertex->getId(), j);
            } else if (edge == NULL) {
                printf("Error V%d Output%d: not connected\n", vertex->getId(), j);
                result = false;
            } else if (edge->getSrc() != vertex) {
                printf("Error V%d E%d: connection mismatch\n", vertex->getId(), edge->getId());
                result = false;
            }
        }
    }

    /* Check edges */


    return result;
}

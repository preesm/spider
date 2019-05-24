/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
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
#ifdef _WIN32
#define getpagesize() 4096
#else

#endif


#include "SpecialActorMemAlloc.h"

void SpecialActorMemAlloc::allocFork(SRDAGVertex *fork) {
    /** Try to find a judicious allocation */
    SRDAGEdge *inEdge = fork->getInEdge(0);
    int alloc = inEdge->getAlloc();
    /** Look if one output is allocated */
    for (int j = 0; alloc == -1 && j < fork->getNConnectedOutEdge(); j++) {
        SRDAGEdge *outEdge = fork->getOutEdge(j);
        if (outEdge->getAlloc() != -1)
            alloc = outEdge->getAlloc();
    }
    /** Else allocate input */
    if (alloc == -1) {
        allocEdge(inEdge);
        alloc = inEdge->getAlloc();
    }

    /** Set allocation of all in/out edges */
    inEdge->setAlloc(alloc);

    int offset = 0;
    for (int j = 0; j < fork->getNConnectedOutEdge(); j++) {
        SRDAGEdge *outEdge = fork->getOutEdge(j);
        if (outEdge->getAlloc() == -1)
            outEdge->setAlloc(alloc + offset);

        offset += outEdge->getRate();
    }
}

void SpecialActorMemAlloc::allocJoin(SRDAGVertex *join) {
    /** Try to find a judicious allocation */
    SRDAGEdge *outEdge = join->getOutEdge(0);
    int alloc = outEdge->getAlloc();
    if (alloc == -1) {
        /** Look if one output is allocated */
        for (int j = 0; j < join->getNConnectedInEdge(); j++) {
            SRDAGEdge *inEdge = join->getInEdge(j);
            if (inEdge->getAlloc() != -1) {
                alloc = inEdge->getAlloc();
                break;
            }
        }
        /** Else allocate output */
        if (alloc == -1) {
            allocEdge(outEdge);
            alloc = outEdge->getAlloc();
        }
    }

    /** Set allocation of all in/out edges */
    outEdge->setAlloc(alloc);

    int offset = 0;
    for (int j = 0; j < join->getNConnectedInEdge(); j++) {
        SRDAGEdge *inEdge = join->getInEdge(j);
        if (inEdge->getAlloc() == -1)
            inEdge->setAlloc(alloc + offset);

        offset += inEdge->getRate();
    }
}

void SpecialActorMemAlloc::allocBroadcast(SRDAGVertex *br) {
    SRDAGEdge *inEdge = br->getInEdge(0);

    /** Try to find a judicious allocation */
    int alloc = inEdge->getAlloc();
    if (alloc == -1) {
        /** Look if one output is allocated */
        for (int j = 0; j < br->getNConnectedOutEdge(); j++) {
            SRDAGEdge *outEdge = br->getOutEdge(j);
            if (outEdge->getAlloc() != -1) {
                alloc = outEdge->getAlloc();
                break;
            }
        }
        /** Else allocate input */
        if (alloc == -1) {
            allocEdge(inEdge);
            alloc = br->getInEdge(0)->getAlloc();
        }
    }

    /** Set allocation of all in/out edges */
    inEdge->setAlloc(alloc);

    for (int j = 0; j < br->getNConnectedOutEdge(); j++) {
        SRDAGEdge *outEdge = br->getOutEdge(j);
        if (outEdge->getAlloc() == -1)
            outEdge->setAlloc(alloc);
    }
}

// TODO For more perfs, try to not execute optimized special actors
void SpecialActorMemAlloc::alloc(List<SRDAGVertex *> *listOfVertices) {
    /** Look for Fork **/
    for (int i = 0; i < listOfVertices->size(); i++) {
        SRDAGVertex *fork = listOfVertices->operator[](i);
        if (fork->getState() == SRDAG_EXEC
            && fork->getType() == SRDAG_FORK)
            allocFork(fork);
    }

    /** Look for Join **/
    for (int i = 0; i < listOfVertices->size(); i++) {
        SRDAGVertex *join = listOfVertices->operator[](i);
        if (join->getState() == SRDAG_EXEC
            && join->getType() == SRDAG_JOIN)
            allocJoin(join);
    }

    /** Look for Broadcast **/
    for (int i = 0; i < listOfVertices->size(); i++) {
        SRDAGVertex *br = listOfVertices->operator[](i);
        if (br->getState() == SRDAG_EXEC
            && br->getType() == SRDAG_BROADCAST) {
            allocBroadcast(br);
        }
    }

    for (int i = 0; i < listOfVertices->size(); i++) {
        SRDAGVertex *vertex = listOfVertices->operator[](i);
        if (vertex->getState() == SRDAG_EXEC) {
            for (int j = 0; j < vertex->getNConnectedOutEdge(); j++) {
                SRDAGEdge *edge = vertex->getOutEdge(j);
                if (edge->getAlloc() == -1) {
                    allocEdge(edge);
                }
            }
        }
    }
}

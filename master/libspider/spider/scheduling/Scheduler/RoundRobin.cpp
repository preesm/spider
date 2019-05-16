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
#include "RoundRobin.h"

#include <graphs/SRDAG/SRDAGGraph.h>
#include <launcher/Launcher.h>

RoundRobin::RoundRobin() {
    srdag_ = nullptr;
    schedule_ = nullptr;
    archi_ = nullptr;
    list_ = nullptr;
}

RoundRobin::~RoundRobin() = default;

#if 0
int RoundRobin::computeSchedLevel(SRDAGVertex* vertex){
    int lvl = 0;
    if(vertex->getSchedLvl() == -1){
        for(int i=0; i<vertex->getNConnectedOutEdge(); i++){
            SRDAGVertex* succ = vertex->getOutEdge(i)->getSnk();
            if(succ && succ->getState() != SRDAG_NEXEC){
                Time minExecTime = (Time)-1;
                for(int j=0; j<archi_->getNPE(); j++){
                    if(succ->isExecutableOn(j)){
                        Time execTime = succ->executionTimeOn(archi_->getPEType(j));
                        if (execTime == 0) {
                            if (succ->getReference()) {
                                throwSpiderException("Vertex: %s -- NULL execution time.", succ->getReference()->getName());
                            }
                            throwSpiderException("Vertex has NULL execution time.", succ->getReference()->getName());
                        }
                        minExecTime = std::min(minExecTime, execTime);
                    }
                }
                lvl = std::max(lvl, computeSchedLevel(succ)+(int)minExecTime);
            }
        }
        vertex->setSchedLvl(lvl);
        return lvl;
    }
    return vertex->getSchedLvl();
}
#endif

void RoundRobin::mapVertex(SRDAGVertex *vertex) {
    Time minimumStartTime = computeMinimumStartTime(vertex);

    if (vertex->getState() == SRDAG_RUN) {
        vertex->setStartTime(minimumStartTime);
        vertex->setEndTime(minimumStartTime);
        return;
    }

    int bestSlave = -1;
    Time bestStartTime = 0;
    auto bestEndTime = (Time) -1; // Very high value.


    //Getting alloc size to determine if PE can handle it
    int vertexAllocSize = 0;
    for (int i = 0; i < vertex->getNConnectedInEdge(); i++) {
        vertexAllocSize += vertex->getInEdge(i)->getRate();
    }

    for (int i = 0; i < vertex->getNConnectedOutEdge(); i++) {
        vertexAllocSize += vertex->getOutEdge(i)->getRate();
    }

    //if (vertex->getType() == SRDAG_NORMAL) printf("%s requires %d bytes\n",vertex->getReference()->getName(),vertexAllocSize);


    static int peIx = 0;
    int npe = archi_->getNPE();

    while (bestSlave == -1) {
        peIx++;

        if (vertex->getType() != SRDAG_NORMAL) {
            bestSlave = 0;
            break;
        }

        if (!archi_->getPEFromSpiderID(peIx % npe)->isEnabled()) {
            continue;
        }

        if (vertex->isExecutableOn(peIx % npe) && vertexAllocSize < Platform::get()->getMaxActorAllocSize(peIx % npe)) {
            bestSlave = peIx % npe;
        }
    }

    bestStartTime = std::max(schedule_->getReadyTime(bestSlave), minimumStartTime);
    bestEndTime = bestStartTime + vertex->executionTimeOn(archi_->getPEFromSpiderID(bestSlave)->getHardwareType());

    if (bestSlave < 0) {
        throwSpiderException("No slave found to execute one instance of vertex [%s].", vertex->toString());
    }
    Scheduler::addJobToSchedule(schedule_, vertex, bestSlave, &bestStartTime, &bestEndTime);
}

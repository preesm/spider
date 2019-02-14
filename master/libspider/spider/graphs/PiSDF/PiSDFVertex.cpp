/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013)
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
#include <graphs/PiSDF/PiSDFVertex.h>

/** Static Var def */
int PiSDFVertex::globalId = 0;

/** Constructor */
PiSDFVertex::PiSDFVertex(
        const char *name, int fctId,
        int typeId,
        PiSDFType type, PiSDFSubType subType,
        PiSDFGraph *graph, PiSDFGraph *subGraph,
        int nInEdge, int nOutEdge,
        int nInParam, int nOutParam) {

    id_ = globalId++;
    typeId_ = typeId;
    fctId_ = fctId;
    type_ = type;
    name_ = name;

    subType_ = subType;
    graph_ = graph;
    subGraph_ = subGraph;

    nInEdge_ = nInEdge;
    nOutEdge_ = nOutEdge;
    nEdge_ = nInEdge_ + nOutEdge_;
    allEdges_ = CREATE_MUL(PISDF_STACK, nEdge_, PiSDFEdge *);
    memset(allEdges_, 0, nEdge_ * sizeof(PiSDFEdge *));
    inEdges_ = allEdges_;
    outEdges_ = allEdges_ + nInEdge_;

    nInParam_ = nInParam;
    inParams_ = CREATE_MUL(PISDF_STACK, nInParam, PiSDFParam*);
    memset(inParams_, 0, nInParam * sizeof(PiSDFParam *));

    nOutParam_ = nOutParam;
    outParams_ = CREATE_MUL(PISDF_STACK, nOutParam, PiSDFParam*);
    memset(outParams_, 0, nOutParam * sizeof(PiSDFParam *));

    nPeMax_ = Spider::getArchi()->getNPE();
    nPeTypeMax_ = Spider::getArchi()->getNPETypes();

    constraints_ = CREATE_MUL(PISDF_STACK, nPeMax_, bool);
    memset(constraints_, false, nPeMax_ * sizeof(bool));

    timings_ = CREATE_MUL(PISDF_STACK, nPeTypeMax_, Expression*);
    memset(timings_, 0, nPeTypeMax_ * sizeof(Expression *));
}

PiSDFVertex::~PiSDFVertex() {
    inEdges_ = nullptr;
    outEdges_ = nullptr;
    StackMonitor::free(PISDF_STACK, allEdges_);
    StackMonitor::free(PISDF_STACK, inParams_);
    StackMonitor::free(PISDF_STACK, outParams_);
    StackMonitor::free(PISDF_STACK, constraints_);

    for (int i = 0; i < nPeTypeMax_; i++) {
        if (timings_[i]) {
            timings_[i]->~Expression();
            StackMonitor::free(PISDF_STACK, timings_[i]);
            timings_[i] = nullptr;
        }
    }
    StackMonitor::free(PISDF_STACK, timings_);
    if (scheduleJob_) {
        scheduleJob_->~ScheduleJob();
        StackMonitor::free(TRANSFO_STACK, scheduleJob_);
    }
}

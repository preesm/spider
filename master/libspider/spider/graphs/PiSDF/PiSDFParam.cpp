/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
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
#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFParam.h>
#include <parser/Expression.h>

/** Static Var def */
int PiSDFParam::globalIx = 0;

PiSDFParam::PiSDFParam(
        const char *name,
        int typeIx,
        PiSDFGraph *graph,
        PiSDFParamType type,
        const char *expr) {
    id_ = globalIx++;
    typeIx_ = typeIx;
    name_ = name;
    graph_ = graph;
    type_ = type;
    value_ = -1;
    parentId_ = -1;
    setter_ = nullptr;
    portIx_ = -1;

    switch (type) {
        case PISDF_PARAM_DEPENDENT_STATIC:
        case PISDF_PARAM_DEPENDENT_DYNAMIC:
            expr_ = CREATE(PISDF_STACK, Expression)(
                    expr,
                    graph->getParams(),
                    graph->getNParam());
            break;
        case PISDF_PARAM_STATIC:
        case PISDF_PARAM_HERITED:
        case PISDF_PARAM_DYNAMIC:
        default:
            expr_ = nullptr;
            break;
    }
}

PiSDFParam::~PiSDFParam() {
    switch (this->type_) {
        case PISDF_PARAM_DEPENDENT_STATIC:
        case PISDF_PARAM_DEPENDENT_DYNAMIC:
            expr_->~Expression();
            StackMonitor::free(PISDF_STACK, expr_);
            break;
        case PISDF_PARAM_STATIC:
        case PISDF_PARAM_HERITED:
        case PISDF_PARAM_DYNAMIC:
        default:
            break;
    }
}

/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
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
#ifndef PISDF_COMMON_H
#define PISDF_COMMON_H

#include <tools/Set.h>

class PiSDFEdge;

class PiSDFParam;

class PiSDFGraph;

class PiSDFVertex;

typedef enum PiSDFParamType {
    PISDF_PARAM_STATIC,
    PISDF_PARAM_HERITED,
    PISDF_PARAM_DYNAMIC,
} PiSDFParamType;

typedef enum PiSDFType {
    PISDF_TYPE_BODY, PISDF_TYPE_CONFIG, PISDF_TYPE_IF
} PiSDFType;

typedef enum PiSDFDelayType {
    PISDF_DELAY_PERSISTENT = 1,
    PISDF_DELAY_NONPERSISTENT = 0
} PiSDFDelayType;

/** Set types */
using PiSDFEdgeSet = Set<PiSDFEdge *>;
using PiSDFParamSet = Set<PiSDFParam *>;
using PiSDFVertexSet = Set<PiSDFVertex *>;

#endif/*PISDF_COMMON_H*/

/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2016) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
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
#include <graphs/PiSDF/PiSDFEdge.h>
#include <string.h>

#include <monitor/StackMonitor.h>

/** Static Var def */
int PiSDFEdge::globalId = 0;

PiSDFEdge::PiSDFEdge(PiSDFGraph* graph){
	id_ = globalId++;
	graph_ = graph;

	src_ = 0; srcPortIx_ = -1;
	snk_ = 0; snkPortIx_ = -1;

	prod_ = cons_ = delay_ = 0;
	setter_ = 0;
	getter_ = 0;
}

PiSDFEdge::~PiSDFEdge(){
	if(delay_ != 0){
		delay_->~Expression();
		StackMonitor::free(PISDF_STACK, delay_);
		delay_ = 0;
	}
	if(prod_ != 0){
		prod_->~Expression();
		StackMonitor::free(PISDF_STACK, prod_);
		prod_ = 0;
	}
	if(cons_ != 0){
		cons_->~Expression();
		StackMonitor::free(PISDF_STACK, cons_);
		cons_ = 0;
	}

}

void PiSDFEdge::connectSrc(PiSDFVertex *src, int srcPortId, const char *prod){
	if(src_ != 0)
		throw "PiSDFEdge: try to connect to an already connected edge";
	src_ = src;
	srcPortIx_ = srcPortId;

	if(prod_ != 0){
		prod_->~Expression();
		StackMonitor::free(PISDF_STACK, prod_);
		prod_ = 0;
	}
	prod_ = CREATE(PISDF_STACK, Expression)(prod, src->getInParams(), src->getNInParam());
}

void PiSDFEdge::connectSnk(PiSDFVertex *snk, int snkPortId, const char *cons){
	if(snk_ != 0)
		throw "PiSDFEdge: try to connect to an already connected edge";
	snk_ = snk;
	snkPortIx_ = snkPortId;

	if(cons_ != 0){
		cons_->~Expression();
		StackMonitor::free(PISDF_STACK, cons_);
		cons_ = 0;
	}
	cons_ = CREATE(PISDF_STACK, Expression)(cons, snk->getInParams(), snk->getNInParam());
}

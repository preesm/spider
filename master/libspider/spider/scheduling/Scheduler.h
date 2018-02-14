/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2016) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
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
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Schedule.h"
#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/Archi/Archi.h>
#include "MemAlloc.h"
#include <spider.h>

class Scheduler {
public:
	Scheduler(){}
	virtual ~Scheduler(){}

	virtual void schedule(SRDAGGraph* graph, MemAlloc* memAlloc, Schedule* schedule, Archi* archi) = 0;
	virtual void scheduleOnlyConfig(SRDAGGraph* graph, MemAlloc* memAlloc, Schedule* schedule, Archi* archi) = 0;

};

#endif/*SCHEDULER_H*/

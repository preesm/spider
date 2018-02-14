/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
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
#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <cstdlib>

#include "Stack.h"
#include <monitor/StackMonitor.h>

template<typename TYPE> class Queue{
public:
	Queue(){
		stackId_ 	= 0;
		first_ 	= 0;
		last_ 	= 0;
	}
	Queue(SpiderStack stackId){
		stackId_ 	= stackId;
		first_ 	= 0;
		last_ 	= 0;
	}

	inline bool isEmpty() const;

	inline void push(TYPE value);
	inline TYPE pop();


private:
	struct QueueItem{
		TYPE cur;
		struct QueueItem* next;
	};

	SpiderStack stackId_;
	struct QueueItem* first_;
	struct QueueItem* last_;
};


template<typename TYPE>
inline bool Queue<TYPE>::isEmpty() const{
	return first_ == 0;
}

template<typename TYPE>
inline void Queue<TYPE>::push(TYPE value){
	struct QueueItem* newItem = CREATE(stackId_, struct QueueItem);
	newItem->cur 	= value;
	newItem->next 	= 0;

	if(last_ == 0)
		first_ = last_ = newItem;
	else{
		last_->next = newItem;
		last_ = newItem;
	}
}

template<typename TYPE>
inline TYPE Queue<TYPE>::pop(){
	if(first_ == 0)
		throw "Try to pop an empty Queue\n";

	struct QueueItem* old = first_;
	TYPE val = first_->cur;
	first_ = first_->next;
	StackMonitor::free(stackId_, old);
	if(first_ == 0)
		last_ = 0;
	return val;
}

#endif/*QUEUE_H*/

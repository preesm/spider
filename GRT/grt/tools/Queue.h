/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#ifndef QUEUE_H_
#define QUEUE_H_

#include "SchedulingError.h"
#include <platform_types.h>

/**
 * Generic Queue Class
 */
template <class T, int SIZE> class Queue {
private:
	/**
	 * Array of values.
	 */
	T buffer[SIZE];

	/**
	 * Read/Write Index
	 */
	int wIx, rIx;

public:
	/**
	 * Default Constructor.
	 */
	Queue();

	/**
	 * Reset Array.
	 */
	void reset();

	void push(T& e);
	T pop();

	BOOL isEmpty();
};

/**
 * Default Constructor.
 */
template <class T, int SIZE>
inline Queue<T,SIZE>::Queue(){
	rIx = wIx = 0;
}

/**
 * Reset Queue.
 */
template <class T, int SIZE>
inline void Queue<T,SIZE>::reset(){
	rIx = wIx = 0;
}

/**
 *
 */
template <class T, int SIZE>
inline void Queue<T,SIZE>::push(T& e){
	buffer[wIx] = e;
	wIx = (wIx+1)%SIZE;
	if(isEmpty()){
		printf("Queue: impossible to push in queue\n");
		abort();
	}
}

/**
 *
 */
template <class T, int SIZE>
inline T Queue<T,SIZE>::pop(){
	if(!isEmpty()){
		T res = buffer[rIx];
		rIx = (rIx+1)%SIZE;
		return res;
	}else{
		printf("Queue: pop empty queue\n");
		abort();
		return buffer[0];
	}
}

/**
 *
 */
template <class T, int SIZE>
inline BOOL Queue<T,SIZE>::isEmpty(){
	return wIx == rIx;
}

#endif /* QUEUE_H_ */

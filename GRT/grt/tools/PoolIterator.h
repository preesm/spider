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

#ifndef POOL_ITERATOR_H_
#define POOL_ITERATOR_H_

#include <grt_definitions.h>
#include "SchedulingError.h"
#include <platform_types.h>

#include <tools/Pool.h>
#include <tools/Iterator.h>

template <class T, int SIZE> class PoolIterator : public Iterator<T,SIZE> {
private:
	Pool<T,SIZE> * pool;

	int currentIx;

public:
	/** Default Constructor. */
	PoolIterator(Pool<T,SIZE> * p);

	PoolIterator();

	/** Default Destructor. */
	virtual ~PoolIterator();

	/** Reset Set. */
	void reset();

	/**
	 * Get next element.
	 * @return next element.
	 */
	T* next();

};

/** Default Constructor. */
template <class T, int SIZE>
inline PoolIterator<T, SIZE>::PoolIterator(Pool<T,SIZE> * p){
	pool = p;
	currentIx = 0;
}

template <class T, int SIZE>
inline PoolIterator<T, SIZE>::PoolIterator(){
}

/** Default Destructor. */
template <class T, int SIZE>
inline PoolIterator<T, SIZE>::~PoolIterator(){
}

/** Reset Set. */
template <class T, int SIZE>
inline void PoolIterator<T, SIZE>::reset(){
	currentIx = 0;
}

template <class T, int SIZE>
inline T* PoolIterator<T, SIZE>::next(){
	do{
		if(currentIx>=SIZE)
			return (T*)NULL;
		if(pool->valid[currentIx])
			return &(pool->array[currentIx++]);
		currentIx++;
	}while(1);
//	return set->array[currentIx++];
}


#endif /* POOL_ITERATOR_H_ */

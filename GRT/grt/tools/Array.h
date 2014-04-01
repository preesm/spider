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

#ifndef ARRAY_H_
#define ARRAY_H_

#include "SchedulingError.h"
#include <platform_types.h>

/**
 * Generic Array Class
 */
template <class T, int SIZE> class Array {
private:
	/**
	 * Array of values.
	 */
	T array[SIZE];

	/**
	 * Validity of array values.
	 */
	BOOL valid[SIZE];

	/**
	 * Number of valid values.
	 */
	UINT32 nb;

public:
	/**
	 * Default Constructor.
	 */
	Array();

	/**
	 * Reset Array.
	 */
	void reset();

	/**
	 * Overloading operator[].
	 * @param n Rank of the element.
	 * @return Element of rank n.
	 */
	T& operator [](int n);

	void add(T& e, int n);
	void remove(T& e);

	/**
	 * Get current number of element in the list.
	 * @return number of element.
	 */
	UINT32 getNb();

	/**
	 * Get the Id from an element
	 */
	UINT32 getIdOf(T& t);
};

/**
 * Default Constructor.
 */
template <class T, int SIZE>
inline Array<T,SIZE>::Array(){
	nb = 0;
	memset(valid, FALSE, SIZE*sizeof(BOOL));
}

/**
 * Reset List.
 */
template <class T, int SIZE>
inline void Array<T,SIZE>::reset(){
	nb = 0;
	memset(valid, FALSE, SIZE*sizeof(BOOL));
}

/**
 * Overloading operator[].
 * @param n Rank of the element.
 * @return Element of rank n.
 */
template <class T, int SIZE>
inline T& Array<T,SIZE>::operator [](int n){
	if(valid[n]){
		return array[n];
	}
	printf("Array: Error get uninitialized var\n");
	abort();
	return array[0];
}

/**
 * Get current number of element in the list.
 * @return number of element.
 */
template <class T, int SIZE>
inline UINT32 Array<T,SIZE>::getNb(){
	return nb;
}

template <class T, int SIZE>
inline void Array<T,SIZE>::add(T& e, int n){
	if(!valid[n]){
		valid[n] = TRUE;
		array[n] = e;
		nb++;
	}else{
		printf("Array: Adding element on initialized element\n");
		abort();
	}
}

template <class T, int SIZE>
inline void Array<T,SIZE>::remove(T& t){
	int n = getIdOf(t);
	if(valid[n]){
		valid[n] = FALSE;
		nb--;
	}else{
		printf("Array: Removing uninitialized element\n");
		abort();
	}
}

/**
 *
 */
template <class T, int SIZE>
inline UINT32 Array<T,SIZE>::getIdOf(T& t){
	int nbSeen=0;
	for(int i=0; i<SIZE; i++){
		if(valid[i]){
			if(array[i] == t)
				return i;
			if(++nbSeen == nb)
				return -1;
		}
	}
	return -1;
}

#endif /* ARRAY_H_ */

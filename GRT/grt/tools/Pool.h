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

#ifndef POOL_H_
#define POOL_H_

#include <grt_definitions.h>
#include <tools/Set.h>

template <class T, int SIZE> class Pool {
private:
	/** Name for debug */
	char name[MAX_TOOL_NAME];

	/** Array of values. */
	T array[SIZE];

	/** Number of value in the Pool. */
	int nb;

public:
	/** Default Constructor. */
	Pool();

	/** Default Destructor. */
	virtual ~Pool();

	/** Reset Pool. */
	void reset();

	/**
	 * Set the IndexedArray name.
	 * @param str the new name.
	 */
	void setName(const char* str);

	/**
	 * Get the IndexedArray name
	 * @return its name.
	 */
	const char * getName();

	/**
	 * Get current number of element in the Pool.
	 * @return number of element.
	 */
	int getNb();

	/**
	 * Allocate one element to the Pool.
	 * @return e Allocated element .
	 */
	T* 	alloc();

	/**
	 * Remove an element of the Pool.
	 * @param e Element to remove.
	 */
	void free(T* e);
};

/** Default Constructor. */
template <class T, int SIZE>
inline Pool<T,SIZE>::Pool(){
	nb = 0;
}

/** Default Destructor. */
template <class T, int SIZE>
inline Pool<T,SIZE>::~Pool(){
	printf("[%s]: alloc %d elemts\n", name, nb);
}

/** Reset Pool. */
template <class T, int SIZE>
inline void Pool<T,SIZE>::reset(){
	nb = 0;
}

/**
 * Set the IndexedArray name.
 * @param name the new name.
 */
template <class T, int SIZE>
void Pool<T,SIZE>::setName(const char* str){
	if(strlen(str) >= MAX_TOOL_NAME)
		exitWithCode(2000, str);
	strncpy(name, str, MAX_TOOL_NAME);
}

/**
 * Get the IndexedArray name
 * @return its name.
 */
template <class T, int SIZE>
const char * Pool<T,SIZE>::getName(){
	return name;
}

/**
 * Get current number of element in the Pool.
 * @return number of element.
 */
template <class T, int SIZE>
inline int Pool<T,SIZE>::getNb(){
	return nb;
}

/**
 * Allocate one element to the Pool.
 * @return e Allocated element .
 */
template <class T, int SIZE>
inline T* Pool<T,SIZE>::alloc(){
	// TODO: Reuse of deleted element
	if(nb<SIZE){
		nb++;
		return &(array[nb-1]);
	}else{
		exitWithCode(2001, name);
		return (T*)0;
	}
}

/**
 * Remove an element of the Pool.
 * @param e Element to remove.
 */
template <class T, int SIZE>
inline void Pool<T,SIZE>::free(T* e){
	// TODO: Reuse of deleted element
//	int ix = (e-array)/sizeof(T);
//	array[ix] = array[--nb];
}

#endif /* POOL_H_ */

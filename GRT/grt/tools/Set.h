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

#ifndef SET_H_
#define SET_H_

#include <grt_definitions.h>
#include "SchedulingError.h"
#include <platform_types.h>

//#include <tools/SetIterator.h>

template <class T, int SIZE> class SetIterator;

/**
 * Generic Set Class
 */
template <class T, int SIZE> class Set {
private:
	/** Name for debug */
	char name[MAX_TOOL_NAME];

	/** Array of values. */
	T* array[SIZE];

	/** Number of value in the Set. */
	int nb;
	int max;

public:
	/** Default Constructor. */
	Set();

	/** Default Destructor. */
	virtual ~Set();

	/** Reset Set. */
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
	 * Get current number of element in the Set.
	 * @return number of element.
	 */
	int getNb();

	SetIterator<T,SIZE> getIterator();

	/**
	 * Add one element to the set.
	 * @param e Element to add.
	 */
	void add(T* e);

	/**
	 * Remove an element of the set.
	 * @param e Element to remove.
	 */
	void remove(T* e);

	friend class SetIterator<T,SIZE>;
};

/** Default Constructor. */
template <class T, int SIZE>
inline Set<T,SIZE>::Set(){
	nb = 0;
	max = 0;
}

/** Default Destructor. */
template <class T, int SIZE>
inline Set<T,SIZE>::~Set(){
#if STAT
	printf("[%s]: Max alloc %d elemts\n", name, max);
#endif
}

/** Reset Set. */
template <class T, int SIZE>
inline void Set<T,SIZE>::reset(){
	nb = 0;
}

/**
 * Set the IndexedArray name.
 * @param name the new name.
 */
template <class T, int SIZE>
void Set<T,SIZE>::setName(const char* str){
	if(strlen(str) >= MAX_TOOL_NAME)
		exitWithCode(2000, str);
	strncpy(name, str, MAX_TOOL_NAME);
}

/**
 * Get the IndexedArray name
 * @return its name.
 */
template <class T, int SIZE>
const char * Set<T,SIZE>::getName(){
	return name;
}


/**
 * Get current number of element in the Set.
 * @return number of element.
 */
template <class T, int SIZE>
inline int Set<T,SIZE>::getNb(){
	return nb;
}

template <class T, int SIZE>
SetIterator<T,SIZE> Set<T,SIZE>::getIterator(){
	return SetIterator<T,SIZE>(this);
}

/**
 * Add one element to the set.
 * @param e Element to add.
 */
template <class T, int SIZE>
inline void Set<T,SIZE>::add(T* e){
	if(nb<SIZE){
		array[nb]=e;
		nb++;
		if(nb>max)max = nb;
	}else{
		exitWithCode(2001, name);
	}
}

/**
 * Remove an element of the set.
 * @param e Element to remove.
 */
template <class T, int SIZE>
inline void Set<T,SIZE>::remove(T* e){
	int ix = -1;
	for(int i=0; i<nb && ix == -1; i++){
		if(array[i] == e){
			ix = i;
		}
	}
	if(ix != -1){
		nb--;
		array[ix] = array[nb];
	}else{
		exitWithCode(2002, name);
	}
}

#endif /* SET_H_ */

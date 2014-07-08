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

#ifndef INDEXED_ARRAY_H_
#define INDEXED_ARRAY_H_

#include <grt_definitions.h>
#include "SchedulingError.h"
#include <platform_types.h>

/**
 * Indexed array generic class.
 * An indexed array is an array with a validity check of accesses.
 */
template <class T, int SIZE> class IndexedArray {
private:
	/** Name for debug */
	char name[MAX_TOOL_NAME];

	/** Array of values. */
	T array[SIZE];

	/** Validity of array values. */
	bool valid[SIZE];

	/** Number of valid values. */
	int nb;

public:
	/** Default Constructor */
	IndexedArray();

	/** Default Destructor */
	virtual ~IndexedArray();

	/** Reset the array */
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
	 * Access n-th element.
	 * @param n Index of the element.
	 * @return the element.
	 */
	T& operator [](int n);

	/**
	 * Get the number of initialized element of the IndexedArray.
	 * @return the number of element.
	 */
	int getNb();

	/**
	 * Set the value of the n-th element.
	 * @param n Index of the element.
	 * @param e Value of the element.
	 */
	void setValue(int n, T e);

	/**
	 * Reset the value of the n-th element.
	 * @param n Index of the element.
	 */
	void resetValue(int n);
};

/** Default Constructor */
template <class T, int SIZE>
inline IndexedArray<T,SIZE>::IndexedArray(){
	nb = 0;
	memset(valid, false, SIZE*sizeof(bool));
}

/** Default Destructor */
template <class T, int SIZE>
inline IndexedArray<T,SIZE>::~IndexedArray(){
}

/** Reset the array */
template <class T, int SIZE>
inline void IndexedArray<T,SIZE>::reset(){
	nb = 0;
	memset(valid, false, SIZE*sizeof(bool));
}

/**
 * Set the IndexedArray name.
 * @param name the new name.
 */
template <class T, int SIZE>
void IndexedArray<T,SIZE>::setName(const char* str){
	if(strlen(str) >= MAX_TOOL_NAME)
		exitWithCode(2000, str);
	strncpy(name, str, MAX_TOOL_NAME);
}

/**
 * Get the IndexedArray name
 * @return its name.
 */
template <class T, int SIZE>
const char * IndexedArray<T,SIZE>::getName(){
	return name;
}

/**
 * Overloading operator[].
 * @param n Rank of the element.
 * @return Element of rank n.
 */
template <class T, int SIZE>
inline T& IndexedArray<T,SIZE>::operator [](int n){
	if(n >= SIZE)
		exitWithCode(2003, name);
	if(!valid[n])
		exitWithCode(2004, name);
	return array[n];
}

/**
 * Get current number of element in the list.
 * @return number of element.
 */
template <class T, int SIZE>
inline int IndexedArray<T,SIZE>::getNb(){
	return nb;
}

/**
 * Set the value of the n-th element.
 * @param n Index of the element.
 * @param e Value of the element.
 */
template <class T, int SIZE>
inline void IndexedArray<T,SIZE>::setValue(int n, T e){
	if(n >= SIZE)
		exitWithCode(2003, name);
	if(valid[n])
		exitWithCode(2005, name);

	valid[n] = true;
	array[n] = e;
	nb++;
}

/**
 * Reset the value of the n-th element.
 * @param n Index of the element.
 */
template <class T, int SIZE>
inline void IndexedArray<T,SIZE>::resetValue(int n){
	if(n >= SIZE)
		exitWithCode(2003, name);
	if(!valid[n])
		exitWithCode(2006, name);

	valid[n] = false;
	nb--;
}

#endif /* INDEXED_ARRAY_H_ */

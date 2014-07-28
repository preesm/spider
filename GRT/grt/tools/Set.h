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
#include <typeinfo>
#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/SRDAG/SRDAGEdge.h>

//#include <tools/SetIterator.h>

template <class T, int SIZE> class SetIterator;

typedef enum {
	ABSTRACT_VERTICES,
	EDGES,
	DEFAULT
}SetType;

/**
 * Generic Set Class
 */
template <class T, int SIZE> class Set {
private:
#if DEBUG
	/** Name for debug */
	char name[MAX_TOOL_NAME];
#endif

	/** Array of values. */
	T* array[SIZE];

	/** Number of value in the Set. */
	int nb;
	int max;

	SetType type;

public:
	/** Default Constructor. */
	Set(SetType type);

	/** Default Destructor. */
	virtual ~Set();

	/** Reset Set. */
	void reset();

#if DEBUG
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
#endif

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
inline Set<T,SIZE>::Set(SetType _type){
	nb = 0;
	max = 0;
	type = _type;
}

/** Default Destructor. */
template <class T, int SIZE>
inline Set<T,SIZE>::~Set(){
#if STAT && DEBUG
	printf("[%s]: Max alloc %d elemts\n", name, max);
#endif
}

/** Reset Set. */
template <class T, int SIZE>
inline void Set<T,SIZE>::reset(){
	nb = 0;
}

#if DEBUG
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
#endif


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
#if DEBUG
	if(nb>=SIZE){
		exitWithCode(2001, name);
	}else{
#endif
		array[nb]=e;
		nb++;
		if(nb>max)max = nb;

		if(type == ABSTRACT_VERTICES){
			((SRDAGVertex*)e)->setSetIx(nb-1);
		}else if(type == EDGES){
			((SRDAGEdge*)e)->setSetIx(nb-1);
		}
#if DEBUG
	}
#endif
}

/**
 * Remove an element of the set.
 * @param e Element to remove.
 */
template <class T, int SIZE>
inline void Set<T,SIZE>::remove(T* e){
	if(type == ABSTRACT_VERTICES){
		SRDAGVertex*  vertex 		= (SRDAGVertex*) (e);
		SRDAGVertex** vertex_array 	= (SRDAGVertex**)(array);

		int ix = vertex->getSetIx();
#if DEBUG
		if(ix == -1)
			exitWithCode(2002, name);
#endif

		nb--;
		array[ix] = array[nb];
		if(nb!=0)
			vertex_array[ix]->setSetIx(ix);
	}else if(type == EDGES){
		SRDAGEdge*  edge 		= (SRDAGEdge*) (e);
		SRDAGEdge** edge_array 	= (SRDAGEdge**)(array);

		int ix = edge->getSetIx();
#if DEBUG
		if(ix == -1)
			exitWithCode(2002, name);
#endif

		nb--;
		array[ix] = array[nb];
		if(nb!=0)
			edge_array[ix]->setSetIx(ix);
	}else{
		int ix = -1;
		for(int i=0; i<nb && ix == -1; i++){
			if(array[i] == e){
				ix = i;
			}
		}
#if DEBUG
		if(ix == -1)
			exitWithCode(2002, name);
#endif

		nb--;
		array[ix] = array[nb];
	}
}

#endif /* SET_H_ */

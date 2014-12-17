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

#ifndef SET_H
#define SET_H

#include <tools/Stack.h>

template<typename TYPE> class Set {
public:
	Set(){
		array = 0;
		nbMax = 0;
		nb    = 0;
	}

	Set(int nbmax, Stack *stack){
		array = CREATE_MUL(stack, nbmax, TYPE);
		nbMax = nbmax;
		nb    = 0;
	}

	~Set(){
	}

	inline void add(TYPE value);
	inline void del(TYPE value);

	inline TYPE operator[](int ix);

	inline int getN() const;
	inline TYPE const * const getArray() const;

	inline int getIxOf(TYPE val);

private:
	TYPE* array;
	int nb;
	int nbMax;

//	friend class SetIterator;
};

template<typename TYPE>
inline int Set<TYPE>::getN() const{
	return nb;
}

template <typename TYPE>
inline void Set<TYPE>::add(TYPE value){
	if(nb >= nbMax)
		throw "Not enough space in Set\n";
	array[nb++] = value;
}

template <typename TYPE>
inline void Set<TYPE>::del(TYPE value){
	for(int i=0; i<nb; i++){
		if(array[i] == value){
			array[i] = array[--nb];
			return;
		}
	}
	/* TODO: do not search item */
	throw "Deleting Element not present in Set\n";
}

template <typename TYPE>
inline TYPE Set<TYPE>::operator[] (int ix){
	if(ix < 0 || ix >= nb)
		throw "Set: operator[] get bad ix";
	else
		return array[ix];
}

template <typename TYPE>
inline TYPE const * const Set<TYPE>::getArray() const{
	return array;
}

template <typename TYPE>
inline int Set<TYPE>::getIxOf(TYPE val){
	int ix = -1;
	for(int i=0; i<this->nb; i++){
		if(val == array[i]){
			ix = i;
			break;
		}
	}
	return ix;
}

#endif // SET_H

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

#ifndef LIST_H_
#define LIST_H_

#include <typeinfo>
#include "SchedulingError.h"
#include <platform_types.h>

/**
 * Generic List Class
 */
template <class T, int SIZE> class List {
private:
	/**
	 * Array of values.
	 */
	T array[SIZE];

	BOOL valid[SIZE];

	/**
	 * Number of value in the list.
	 */
	int nb;

	int wrNb;

	void myqsort(int p, int r, int (*comp)(T,T));
	int myqsort_part(int p, int r, int (*comp)(T,T));
public:
	/**
	 * Default Constructor.
	 */
	List();

	/**
	 * Reset List.
	 */
	void reset();

	/**
	 * Overloading operator[].
	 * @param n Rank of the element.
	 * @return Element of rank n.
	 */
	T& operator [](int n);

	/**
	 * Get current number of element in the list.
	 * @return number of element.
	 */
	int getNb();

	/**
	 * Add one element to the list.
	 * @param e Element to add.
	 * @return Returns the rank of the recently added element.
	 */
	T* add(T e);

	/**
	 * Reserve one element to the list.
	 * @return Returns the rank of the recently added element.
	 */
	T* add();

	UINT32 getId(T* e);
	void remove(T* e);
	void remove(UINT32 id);

	void sort(int (*comp)(T,T));

	int getWrNb() const {
		return wrNb;
	}
};

/**
 * Default Constructor.
 */
template <class T, int SIZE>
inline List<T,SIZE>::List(){
	nb = 0;
	wrNb = 0;
	memset(valid, FALSE, SIZE*sizeof(BOOL));
}

/**
 * Reset List.
 */
template <class T, int SIZE>
inline void List<T,SIZE>::reset(){
	nb = 0;
	wrNb = 0;
	memset(valid, FALSE, SIZE*sizeof(BOOL));
}

/**
 * Overloading operator[].
 * @param n Rank of the element.
 * @return Element of rank n.
 */
template <class T, int SIZE>
inline T& List<T,SIZE>::operator [](int n){
	if(n>=0 && n<wrNb)
		return array[n];
	else{
		printf("List: Accesing unitialized element\n");
		exitWithCode(1099);
		return array[n];
	}
}

/**
 * Get current number of element in the list.
 * @return number of element.
 */
template <class T, int SIZE>
inline int List<T,SIZE>::getNb(){
	return nb;
}

/**
 * Add one element to the list.
 * @param e Element to add.
 */
template <class T, int SIZE>
inline T* List<T,SIZE>::add(T e){
	if(nb<SIZE){
		array[wrNb]=e;
		valid[wrNb]=TRUE;
		nb++;
		return &(array[wrNb++]);
	}else{
		exitWithCode(1058);
		return (T*)0;
	}
}

/**
 * Reserve one element to the list.
 * @return Returns the rank of the recently added element.
 */
template <class T, int SIZE>
inline T* List<T,SIZE>::add(){
	if(wrNb<SIZE){
		valid[wrNb]=TRUE;
		nb++;
		return &(array[wrNb++]);
	}else{
		exitWithCode(1058);
		return (T*)0;
	}
}

template <class T, int SIZE>
inline UINT32 List<T,SIZE>::getId(T* e){
	return (e-array)/(&array[1]-&array[0]);
}

template <class T, int SIZE>
inline void List<T,SIZE>::remove(T* e){
	valid[getId(e)] = FALSE;
	nb--;
}

template <class T, int SIZE>
inline void List<T,SIZE>::remove(UINT32 id){
	valid[id] = FALSE;
	nb--;
}

template <class T, int SIZE>
inline void List<T,SIZE>::sort(int (*comp)(T,T)){
	myqsort(0,nb-1, comp);
}

template <class T, int SIZE>
inline void List<T,SIZE>::myqsort(int p, int r, int (*comp)(T,T)){
	int q;
	if(p<r){
		q = myqsort_part(p, r, comp);
		myqsort(p, q, comp);
		myqsort(q+1, r, comp);
	}
}

template <class T, int SIZE>
inline int List<T,SIZE>::myqsort_part(int p, int r, int (*comp)(T,T)){
    T pivot = array[p];
    int i = p-1, j = r+1;
    T temp;
    while (1) {
        do
            j--;
        while (comp (array[j],pivot) > 0);
        do
            i++;
        while (comp (array[i],pivot) < 0);
        if (i < j) {
            temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
        else
            return j;
    }
}

#endif /* LIST_H_ */

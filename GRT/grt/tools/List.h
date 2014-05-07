/*
 * List.h
 *
 *  Created on: Sep 18, 2013
 *      Author: jheulot
 */

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

/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2015)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2014)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#ifndef LIST_H
#define LIST_H

#include <monitor/StackMonitor.h>
#include <stdexcept>

template<class T>
class List {
public:
    List(SpiderStack stackId, int size);

    ~List();

    T &operator[](int n);

    int getNb();

    void add(T e);

    void sort(int (*comp)(T, T));

    bool isPresent(T item);

private:
    SpiderStack stackId_;
    T *array;
    int nb;
    int nbMax;

    void myqsort(int p, int r, int (*comp)(T, T));

    int myqsort_part(int p, int r, int (*comp)(T, T));
};

template<class T>
inline List<T>::List(SpiderStack stackId, int size) {
    stackId_ = stackId;
    nb = 0;
    nbMax = size;
    if (size == 0)
        array = nullptr;
    else
        array = CREATE_MUL(stackId_, size, T);
}

template<class T>
inline List<T>::~List() {
    if (array != nullptr)
        StackMonitor::free(stackId_, array);
}

template<class T>
inline T &List<T>::operator[](int ix) {
    if (ix < 0 || ix >= nb)
        throw std::runtime_error("List: Accesing unitialized element\n");
    return array[ix];
}

template<class T>
inline int List<T>::getNb() {
    return nb;
}

template<class T>
inline void List<T>::add(T e) {
    if (nb >= nbMax)
        throw std::runtime_error("List: Full !\n");
    array[nb] = e;
    nb++;
}

template<class T>
inline void List<T>::sort(int (*comp)(T, T)) {
    myqsort(0, nb - 1, comp);
}

template<class T>
inline void List<T>::myqsort(int p, int r, int (*comp)(T, T)) {
    int q;
    if (p < r) {
        q = myqsort_part(p, r, comp);
        myqsort(p, q, comp);
        myqsort(q + 1, r, comp);
    }
}

template<class T>
inline int List<T>::myqsort_part(int p, int r, int (*comp)(T, T)) {
    T pivot = array[p];
    int i = p - 1, j = r + 1;
    T temp;
    while (true) {
        do
            j--;
        while (comp(array[j], pivot) > 0);
        do
            i++;
        while (comp(array[i], pivot) < 0);
        if (i < j) {
            temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        } else
            return j;
    }
}

template<class T>
inline bool List<T>::isPresent(T item) {
    for (int i = 0; i < nb; i++) {
        if (item == array[i])
            return true;
    }
    return false;
}

#endif/*LIST_H*/

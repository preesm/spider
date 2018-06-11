/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
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
#ifndef SET_H
#define SET_H

#include <monitor/StackMonitor.h>
#include <tools/SetElement.h>
#include <stdexcept>

template<typename TYPE>
class Set {
public:
    Set(int nbmax, SpiderStack stackId) {
        if (nbmax > 0) {
            array = CREATE_MUL(stackId, nbmax, TYPE);

            /* Test if TYPE is a SetElement */
            SetElement *elt = dynamic_cast<SetElement *>(array[0]);
        } else {
            array = 0;
        }
        nb = 0;
        nbMax = nbmax;
        stackId_ = stackId;
    }

    ~Set() {
        if (nbMax != 0)
            StackMonitor::free(stackId_, array);
    }

    inline void add(TYPE value);

    inline void del(TYPE value);

    inline TYPE operator[](int ix);

    inline int getN() const;

    inline TYPE const *const getArray() const;

private:
    SpiderStack stackId_;
    TYPE *array;
    int nb;
    int nbMax;

//	friend class SetIterator;
};

template<typename TYPE>
inline int Set<TYPE>::getN() const {
    return nb;
}

template<typename TYPE>
inline void Set<TYPE>::add(TYPE value) {
    if (nb >= nbMax)
        throw std::runtime_error("Not enough space in Set\n");

    ((SetElement *) value)->setSetIx(nb);
    array[nb++] = value;
}

template<typename TYPE>
inline void Set<TYPE>::del(TYPE value) {
    int ix = ((SetElement *) value)->getSetIx();
    array[ix] = array[--nb];
    ((SetElement *) array[ix])->setSetIx(ix);
}

template<typename TYPE>
inline TYPE Set<TYPE>::operator[](int ix) {
    if (ix < 0 || ix >= nb)
        throw std::runtime_error("Set: operator[] get bad ix");
    else
        return array[ix];
}

template<typename TYPE>
inline TYPE const *const Set<TYPE>::getArray() const {
    return array;
}

#endif // SET_H

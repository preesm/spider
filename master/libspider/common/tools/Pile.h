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

#ifndef PILE_H
#define PILE_H

#include <cstdio>
#include <cstdlib>

#include "Stack.h"

template<typename TYPE> class Pile{
public:
	Pile(){
		stack_ 	= 0;
		first_ 	= 0;
	}
	Pile(Stack *stack){
		stack_ 	= stack;
		first_ 	= 0;
	}

	inline bool isEmpty() const;

	inline void push(TYPE value);
	inline TYPE pop();


private:
	struct PileItem{
		TYPE cur;
		struct PileItem* next;
	};

	Stack* stack_;
	struct PileItem* first_;
};


template<typename TYPE>
inline bool Pile<TYPE>::isEmpty() const{
	return first_ == 0;
}

template<typename TYPE>
inline void Pile<TYPE>::push(TYPE value){
	struct PileItem* newItem = CREATE(stack_, struct QueueItem);
	newItem->cur 	= value;
	newItem->next 	= first_;
	first_ = newItem;
}

template<typename TYPE>
inline TYPE Pile<TYPE>::pop(){
	if(first_ == 0)
		throw "Try to pop an empty Pile\n";

	TYPE val = first_->cur;
	first_ = first_->next;
	return val;
}

#endif/*PILE_H*/

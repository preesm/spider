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

#ifndef PICSDFEDGE_H_
#define PICSDFEDGE_H_

#include "../CSDAG/CSDAGEdge.h"

class PiCSDFEdge : public CSDAGEdge{
	private:
		/**
		 Expression defining the delay (in abstract_syntax_elt)
		*/
		abstract_syntax_elt delay[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];

		// Production, consumption and delay after pattern resolution.
		int productionInt; int consumtionInt; int delayInt;

	public:


		/**
		 delay getter.

		 @return delay
		*/
		abstract_syntax_elt* getDelay();

		/**
		 delay setter

		 @param expression defining the delay
		*/
		void setDelay(const char* delay);

		// Getter/setter for productionInt/consumptionInt/delayInt.
		int getProductionInt();

		void setProductionInt(const int prod);

		int getConsumptionInt();

		void setConsumptionInt(const int cons);

		int getDelayInt();

		void setDelayInt(const int delay);
};


/**
 delay getter

 @return delay expression
*/
inline abstract_syntax_elt* PiCSDFEdge::getDelay(){
	return(this->delay);
}

/**
 delay setter. Careful! Not made to set it more than once!

 @param delay: expression defining the initial tokens (in char)
 @return delay after resolving the expression
*/
inline void PiCSDFEdge::setDelay(const char* delay){
	// Parsing the expression
	globalParser.parse(delay, this->delay);
}


// Getter/setter for productionInt/consumptionInt.
inline int PiCSDFEdge::getProductionInt(){
	return this->productionInt;
}

inline void PiCSDFEdge::setProductionInt(const int prod){
	this->productionInt = prod;
}

inline int PiCSDFEdge::getConsumptionInt(){
	return this->consumtionInt;
}

inline void PiCSDFEdge::setConsumptionInt(const int cons){
	this->consumtionInt = cons;
}

inline int PiCSDFEdge::getDelayInt(){
	return this->delayInt;
}

inline void PiCSDFEdge::setDelayInt(const int delay){
	this->delayInt = delay;
}
#endif /* PICSDFEDGE_H_ */

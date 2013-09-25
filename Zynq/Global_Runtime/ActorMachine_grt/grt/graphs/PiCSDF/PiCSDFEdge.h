/*
 * PiSDFEdge.h
 *
 *  Created on: 12 juin 2013
 *      Author: yoliva
 */

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

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
		 Expression defining the initial tokens (in abstract_syntax_elt)
		*/
		abstract_syntax_elt initial_tokens[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];

	public:


		/**
		 initial_tokens getter.

		 @return initial_tokens
		*/
		abstract_syntax_elt* getInitialTokens();

		/**
		 initial_tokens setter

		 @param expression defining the initial tokens
		*/
		void setInitialTokens(const char* initial_tokens);

};


/**
 initial_tokens getter

 @return initial_tokens expression
*/
inline abstract_syntax_elt* PiCSDFEdge::getInitialTokens()
{
	return(this->initial_tokens);
}

/**
 initial_tokens setter. Careful! Not made to set it more than once!

 @param initial_tokens: expression defining the initial tokens (in char)
 @return initial_tokens after resolving the expression
*/
inline void PiCSDFEdge::setInitialTokens(const char* initial_tokens)
{
	// Parsing the expression
	globalParser.parse(initial_tokens,this->initial_tokens);
}


#endif /* PICSDFEDGE_H_ */

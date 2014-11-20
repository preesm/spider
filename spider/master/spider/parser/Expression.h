/*
 * Expression.h
 *
 *  Created on: Nov 7, 2014
 *      Author: jheulot
 */

#ifndef PARSER_EXPRESSION_H
#define PARSER_EXPRESSION_H

#include <graphs/PiSDF/PiSDFParam.h>
#include <tools/Stack.h>
#include "Token.h"

namespace Parser {

class Expression {
public:
	Expression();
	Expression(const char* expr, const PiSDFParam* const * params, int nParam, Stack* stack);
	virtual ~Expression();

	int evaluate(const int* paramValues, int nParam) const;
	void toString(
			const PiSDFParam * const * params, int nParam,
			char* out, int outSizeMax);

private:
	Token* stack_;
	int nElt_;

	bool getNextToken(
			Token* token,
			const char** expr,
			const PiSDFParam* const * params, int nParam);

	int evaluateNTokens(const char* expr);
};

} /* namespace Parser */

#endif/*PARSER_EXPRESSION_H*/

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

#include <parser/Expression.h>
#include <tools/Stack.h>
#include <graphTransfo/GraphTransfo.h>

#include <cctype>
#include <cstring>
#include <cstdio>

namespace Parser {

#define MAX_NVAR_ELEMENTS 100
#define REVERSE_POLISH_STACK_MAX_ELEMENTS 100
#define EXPR_LEN_MAX 1000

static int precedence[4] = {
	2, //OP_ADD
	2, //OP_SUB
	3, //OP_MUL
	3  //OP_DIV
};

static const char* operatorSign[4] = {
	"+", //OP_ADD
	"-", //OP_SUB
	"*", //OP_MUL
	"/" //OP_DIV
};

Expression::Expression() {
	nElt_ = 0;
	stack_ = 0;
}

Expression::Expression(
		const char* expr,
		const PiSDFParam* const * params, int nParam,
		Stack* stackAlloc){
	nElt_ = evaluateNTokens(expr);
	stack_ = sAlloc(stackAlloc, nElt_, Token);

	Token stack[MAX_NVAR_ELEMENTS];

	const char *ptr = expr;
	Token t;
	Token* outputPtr = stack_;
	Token* stackPtr = stack;
	while(getNextToken(&t, &ptr, params, nParam)){
		switch(t.getType()){
		case Token::VALUE:
		case Token::PARAMETER:
			*outputPtr = t;
			outputPtr++;
			break;
		case Token::OPERATOR:
			while((stackPtr-1)->getType() == Token::OPERATOR
					&& precedence[t.getOpType()] <= precedence[(stackPtr-1)->getOpType()]){
				stackPtr--;
				*outputPtr = *stackPtr;
				outputPtr++;
			}
			*stackPtr = t;
			stackPtr++;
			break;
		case Token::LEFT_PAR:
			*stackPtr = t;
			stackPtr++;
			break;
		case Token::RIGHT_PAR:
			while(stackPtr >= stack && (stackPtr-1)->getType() != Token::LEFT_PAR){
				stackPtr--;
				*outputPtr = *stackPtr;
				outputPtr++;
			}
			if(stackPtr < stack){
				throw "Parsing Error missing left parenthesis\n";
			}
			stackPtr--;
			break;
		}
	}

	while(stackPtr >= stack){
		stackPtr--;
		*outputPtr = *stackPtr;
		outputPtr++;
	}
}

Expression::~Expression() {
}

int Expression::evaluate(const PiSDFParam* const * paramList, transfoJob* job) const{
	int stack[MAX_NVAR_ELEMENTS];
	int* stackPtr = stack;
	const Token* inputPtr = stack_;

	while(stack_+nElt_ > inputPtr) {
		switch(inputPtr->getType()){
		case Token::OPERATOR:
			switch(inputPtr->getOpType()){
			case Token::ADD:
				if(stackPtr-stack >= 2){
					stackPtr--;
					*(stackPtr-1) += *stackPtr;
				}
				break;
			case Token::SUB:
				if(stackPtr-stack >= 2){
					stackPtr--;
					*(stackPtr-1) -= *stackPtr;
				}
				break;
			case Token::MUL:
				if(stackPtr-stack >= 2){
					stackPtr--;
					*(stackPtr-1) *= *stackPtr;
				}
				break;
			case Token::DIV:
				if(stackPtr-stack >= 2){
					stackPtr--;
					*(stackPtr-1) /= *stackPtr;
				}
				break;
			}
			break;
		case Token::VALUE:
			*stackPtr = inputPtr->getValue();
			stackPtr++;
			break;
		case Token::PARAMETER:
			*stackPtr = job->paramValues[paramList[inputPtr->getParamIx()]->getTypeIx()];
			stackPtr++;
			break;
		default:
			throw "Error: Parenthesis in evaluated var\n";
		}
		inputPtr++;
	}
	return stack[0];
}

int Expression::evaluate(const int* vertexParamValues, int nParam) const{
	int stack[MAX_NVAR_ELEMENTS];
	int* stackPtr = stack;
	const Token* inputPtr = stack_;

	while(stack_+nElt_ > inputPtr) {
		switch(inputPtr->getType()){
		case Token::OPERATOR:
			switch(inputPtr->getOpType()){
			case Token::ADD:
				if(stackPtr-stack >= 2){
					stackPtr--;
					*(stackPtr-1) += *stackPtr;
				}
				break;
			case Token::SUB:
				if(stackPtr-stack >= 2){
					stackPtr--;
					*(stackPtr-1) -= *stackPtr;
				}
				break;
			case Token::MUL:
				if(stackPtr-stack >= 2){
					stackPtr--;
					*(stackPtr-1) *= *stackPtr;
				}
				break;
			case Token::DIV:
				if(stackPtr-stack >= 2){
					stackPtr--;
					*(stackPtr-1) /= *stackPtr;
				}
				break;
			}
			break;
		case Token::VALUE:
			*stackPtr = inputPtr->getValue();
			stackPtr++;
			break;
		case Token::PARAMETER:
			*stackPtr = vertexParamValues[inputPtr->getParamIx()];
			stackPtr++;
			break;
		default:
			throw "Error: Parenthesis in evaluated var\n";
		}
		inputPtr++;
	}
	return stack[0];
}

void Expression::toString(
			const PiSDFParam * const * params, int nParam,
			char* out, int outSizeMax){
	static char outputStack[REVERSE_POLISH_STACK_MAX_ELEMENTS+1][EXPR_LEN_MAX];
	int outputStackSize = 0;
//	Token* varStackPtr = stack_;

	//While there are input tokens left
	int i;
	for(i=0; i<nElt_; i++){
		switch(stack_[i].getType()){
		case Token::OPERATOR:
			// pop out 2
			sprintf(out, "( %s %s %s )",
					outputStack[outputStackSize-1],
					operatorSign[stack_[i].getOpType()],
					outputStack[outputStackSize-2]);
			strcpy(outputStack[outputStackSize-2], out);
			outputStackSize--;
			break;
		case Token::VALUE:
			// Push value to output stack
			sprintf(outputStack[outputStackSize++], "%d", stack_[i].getValue());
			break;
		case Token::PARAMETER:
			// Push parameter name to output stack
			sprintf(outputStack[outputStackSize++], "%s", params[stack_[i].getParamIx()]->getName());
			break;
		default:
			throw "Error: Parenthesis in evaluated var\n";
		}
	}
	strcpy(out,outputStack[0]);
}

int Expression::evaluateNTokens(const char* expr){
	const char **ptr = &expr;
	int i=0;
	while(getNextToken(0, ptr, 0, 0)){
		i++;
	}
	return i;
}

bool Expression::getNextToken(
		Token* token,
		const char** ptr,
		const PiSDFParam* const * params, int nParam){
	// skip over whitespaces
	while (**ptr == ' ' || **ptr == '\t')
		(*ptr)++;


	// check for end of expression
	if (**ptr == '\0')
		return false;

	// check for minus
	if (**ptr == '-'){
		if(token != 0){
			token->setType(Token::OPERATOR);
			token->setOpType(Token::SUB);
		}
		(*ptr)++;
		return true;
	}

	// check for plus
	if (**ptr == '+'){
		if(token != 0){
			token->setType(Token::OPERATOR);
			token->setOpType(Token::ADD);
		}
		(*ptr)++;
		return true;
	}

	// check for mult
	if (**ptr == '*'){
		if(token != 0){
			token->setType(Token::OPERATOR);
			token->setOpType(Token::MUL);
		}
		(*ptr)++;
		return true;
	}

	// check for div
	if (**ptr == '/'){
		if(token != 0){
			token->setType(Token::OPERATOR);
			token->setOpType(Token::DIV);
		}
		(*ptr)++;
		return true;
	}

	// check for parentheses
	if (**ptr == '('){
		if(token != 0){
			token->setType(Token::LEFT_PAR);
		}
		(*ptr)++;
		return true;
	}
	if (**ptr == ')'){
		if(token != 0){
			token->setType(Token::RIGHT_PAR);
		}
		(*ptr)++;
		return true;
	}

	// check for a value
	if (isdigit(**ptr)){
		int value = 0;
		while (isdigit(**ptr)){
			value *= 10;
			value = **ptr - '0';
			(*ptr)++;
		}
		if(token != 0){
			token->setType(Token::VALUE);
			token->setValue(value);
		}
		return true;
	}

	// check for param
	if (isalnum(**ptr)){
		const char *name = *ptr;
		int nb=0;
//		int id=0;
		while (isalnum(**ptr)){
			nb++;
			(*ptr)++;
		}

		if(token != 0){
			token->setType(Token::PARAMETER);
			for(int i=0; i<nParam; i++){
				if(strncmp(params[i]->getName(), name, nb) == 0){
					token->setParamIx(i/*params[i]->getTypeIx()*/);
					return true;
				}
			}
			throw "Error parsing: param not found\n";
			return false;
		}else return true;
	}

	throw "Error during parsing \n";
	return false;
}

} /* namespace Parser */

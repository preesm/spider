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

#include "ReversePolishNotationGenerator.h"
#include "../tools/SchedulingError.h"
#include <cstdio>
#include <cstdlib>

/**
 * Generator of a Reverse Polish Notation queue from the parsing of an expression. 
 * The algorithm used is the Shunting-Yard algorithm.
 * 
 * @author mpelcat
 */

/**
 Constructor
*/
ReversePolishNotationGenerator::ReversePolishNotationGenerator()
{
	// Resetting the stack and queue
	flush();
}

/**
 Destructor
*/
ReversePolishNotationGenerator::~ReversePolishNotationGenerator()
{
}

/**
 Getting the queue of abstract elements. The internal queue is copied in the
 external queue. Time consuming.

 @param extQueue: Queue of the executable abstract elements
*/
void ReversePolishNotationGenerator::getQueue(abstract_syntax_elt* extQueue){
	int tempTailIndex = tailIndex;
	// Retrieving elements form the internal queue into the external queue
	for(int i=0; i<queueSize; i++){
		abstract_syntax_elt* elt = &outputQueue[tempTailIndex];
		memcpy(&extQueue[i],elt,sizeof(abstract_syntax_elt));
		tempTailIndex++;
		if(tempTailIndex==REVERSE_POLISH_STACK_MAX_ELEMENTS){
			tempTailIndex = 0;
		}
	}
	// setting the type of the unused queue elements to VOID_RPN in the external queue
	for(int i=queueSize; i<REVERSE_POLISH_STACK_MAX_ELEMENTS+1; i++){
		extQueue[i].type = VOID_RPN;
	}
}

/**
 Pushing an operator with its type in the stack
 
 @param eltType: type: operator or value (distinguishes unary and binary operators
 @param op: operator
*/
void ReversePolishNotationGenerator::push(syntax_elt_type eltType, operator_id op){
	if(stackSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full stack
		exitWithCode(1026);
	}

	abstract_syntax_elt* elt = &stack[stackSize];
	elt->type = eltType;
	elt->elt_union.op = op;
	stackSize++;

}

/**
 Pushing a variable reference in the stack
 
 @param eltType: type: variable for assignment or for evaluation
 @param var: reference to a variable
*/
void ReversePolishNotationGenerator::push(syntax_elt_type eltType, variable* var){
	if(stackSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full stack
		exitWithCode(1026);
	}

	abstract_syntax_elt* elt = &stack[stackSize];
	elt->type = eltType;
	elt->elt_union.var = var;
	stackSize++;
}

/**
 Pushing a numerical value in the stack
 
 @param num: numerical value of an element
*/
void ReversePolishNotationGenerator::push(int num){
	if(stackSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full stack
		exitWithCode(1026);
	}

	abstract_syntax_elt* elt = &stack[stackSize];
	elt->type = NUM_RPN;
	elt->elt_union.num = num;
	stackSize++;
}

/**
 Pushing a Left Parenthesis in the stack
*/
void ReversePolishNotationGenerator::pushLeftParenthesis(){
	if(stackSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full stack
		exitWithCode(1026);
	}

	abstract_syntax_elt* elt = &stack[stackSize];
	elt->type = LEFTPAR_RPN;
	stackSize++;
}

/**
 Pushing a Right Parenthesis in the stack
*/
void ReversePolishNotationGenerator::pushRightParenthesis(){
	if(stackSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full stack
		exitWithCode(1026);
	}

	abstract_syntax_elt* elt = &stack[stackSize];
	elt->type = RIGHTPAR_RPN;
	stackSize++;
}

/**
 Popping an element from the stack

 @return: element
*/
abstract_syntax_elt* ReversePolishNotationGenerator::pop(){
	if(stackSize <= 0){
		// Popping an expression element from an empty stack
		exitWithCode(1031);
	}

	stackSize--;
	abstract_syntax_elt* elt = &stack[stackSize];
	return elt;
}

/**
 Peeking the element at the top of the stack

 @return: element
*/
abstract_syntax_elt* ReversePolishNotationGenerator::peek(){
	if(stackSize <= 0){
		// Peeking an expression element from an empty stack
		exitWithCode(1035);
	}

	abstract_syntax_elt* elt = &stack[stackSize-1];
	return elt;
}

/**
 Enqueueing an operator in the output queue
 
 @param eltType: type: operator or value (distinguishes unary and binary operators
 @param op: operator
*/
void ReversePolishNotationGenerator::enqueue(syntax_elt_type eltType, operator_id op){
	if(queueSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full queue
		exitWithCode(1029);
	}

	abstract_syntax_elt* elt = &outputQueue[headIndex];
	elt->type = eltType;
	elt->elt_union.op = op;
	headIndex++;
	if(headIndex==REVERSE_POLISH_STACK_MAX_ELEMENTS){
		headIndex = 0;
	}
	queueSize++;
}

/**
 Enqueueing a variable in the output queue
 
 @param eltType: type: variable for assignment or for evaluation
 @param var: reference to a variable
*/
void ReversePolishNotationGenerator::enqueue(syntax_elt_type eltType, variable* var){
	if(queueSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full queue
		exitWithCode(1029);
	}

	abstract_syntax_elt* elt = &outputQueue[headIndex];
	elt->type = eltType;
	elt->elt_union.var = var;
	headIndex++;
	if(headIndex==REVERSE_POLISH_STACK_MAX_ELEMENTS){
		headIndex = 0;
	}
	queueSize++;
}

/**
 Enqueueing a numerical value in the output queue
 
 @param num: numerical value of an element
*/
void ReversePolishNotationGenerator::enqueue(int num){
	if(queueSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full queue
		exitWithCode(1029);
	}

	abstract_syntax_elt* elt = &outputQueue[headIndex];
	elt->type = NUM_RPN;
	elt->elt_union.num = num;
	headIndex++;
	if(headIndex==REVERSE_POLISH_STACK_MAX_ELEMENTS){
		headIndex = 0;
	}
	queueSize++;
}

/**
 Enqueueing an element

 @param inelt: the element
*/
void ReversePolishNotationGenerator::enqueue(abstract_syntax_elt* inelt){
	if(queueSize >= REVERSE_POLISH_STACK_MAX_ELEMENTS){
		// Adding an expression element in a full queue
		exitWithCode(1029);
	}

	abstract_syntax_elt* elt = &outputQueue[headIndex];
	memcpy(elt,inelt,sizeof(abstract_syntax_elt));
	headIndex++;
	if(headIndex==REVERSE_POLISH_STACK_MAX_ELEMENTS){
		headIndex = 0;
	}
	queueSize++;
}

/**
 Dequeuing an element from the output queue
 
 @return: the element
*/
abstract_syntax_elt* ReversePolishNotationGenerator::dequeue(){
	if(queueSize <= 0){
		// Removing an expression element from an empty queue
		exitWithCode(1030);
	}
	abstract_syntax_elt* result = &outputQueue[tailIndex];
	tailIndex++;
	if(tailIndex==REVERSE_POLISH_STACK_MAX_ELEMENTS){
		tailIndex = 0;
	}
	queueSize--;
	return result;
}


/**
 Removing all elements from queue and stack
*/
void ReversePolishNotationGenerator::flush(){
	stackSize = 0;
	headIndex = tailIndex = queueSize = 0;
}

/**
 Getting the queue size
 
 @return: the queue size
*/
int ReversePolishNotationGenerator::getQueueSize(){
	return queueSize;
}

/**
 Getting the stack size
 
 @return: the stack size
*/
int ReversePolishNotationGenerator::getStackSize(){
	return stackSize;
}


#ifdef DISPLAY

/**
 Prints a RPN abstract element in the stdio. Useful for debug

 @param elt: the element to print
*/
void printElement(abstract_syntax_elt* elt){
	static char temp[40];

	if(elt->type == LEFTPAR_RPN){
		printf("(\t");
	}
	else if(elt->type == RIGHTPAR_RPN){
		printf(")\t");
	}
	else if(elt->type == BINARY_RPN){
		if(elt->elt_union.op == ASSIGN_RPN) printf("=\t");
		else if(elt->elt_union.op == AND_RPN) printf("&\t");
		else if(elt->elt_union.op == OR_RPN) printf("|\t");
		else if(elt->elt_union.op == BITSHIFTLEFT_RPN) printf("<<\t");
		else if(elt->elt_union.op == BITSHIFTRIGHT_RPN) printf(">>\t");
		else if(elt->elt_union.op == EQUAL_RPN) printf("==\t");
		else if(elt->elt_union.op == UNEQUAL_RPN) printf("!=\t");
		else if(elt->elt_union.op == SMALLER_RPN) printf("<\t");
		else if(elt->elt_union.op == LARGER_RPN) printf(">\t");
		else if(elt->elt_union.op == SMALLEREQ_RPN) printf("<=\t");
		else if(elt->elt_union.op == LARGEREQ_RPN) printf(">=\t");
		else if(elt->elt_union.op == PLUS_RPN) printf("+\t");
		else if(elt->elt_union.op == MINUS_RPN) printf("-\t");
		else if(elt->elt_union.op == MULTIPLY_RPN) printf("*\t");
		else if(elt->elt_union.op == DIVIDE_RPN) printf("/\t");
		else if(elt->elt_union.op == MODULUS_RPN) printf("%\t");
		else if(elt->elt_union.op == XOR_RPN) printf("^\t");
	}
	else if(elt->type == VARIABLE_ASSIGN_RPN || elt->type == VARIABLE_EVAL_RPN){
		sprintf(temp,"%s\t",elt->elt_union.var->name);
		printf("%s",temp);
	}
	else if(elt->type == NUM_RPN){
		sprintf(temp,"%d\t",elt->elt_union.num);
		printf("%s",temp);
	}
}
/**
 Prints the stack and the queue in the stdio. Useful for debug
*/
void ReversePolishNotationGenerator::print(){/*
	printf("\nRPN queue:\t");
	int tempTailIndex = tailIndex;
	// Printing the queue elements
	for(int i=0; i<queueSize; i++){
		abstract_syntax_elt* elt = &outputQueue[tempTailIndex];
		printElement(elt);
		tempTailIndex++;
		if(tempTailIndex==REVERSE_POLISH_STACK_MAX_ELEMENTS){
			tempTailIndex = 0;
		}
	}
	printf("\nRPN stack:\t");
	// Printing the stack elements
	for(int i=0; i<stackSize; i++){
		abstract_syntax_elt* elt = &stack[i];
		printElement(elt);
	}
	printf("\n");*/
}
#endif

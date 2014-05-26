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

#ifndef ABSTRACT_SYNTAX_TREE
#define ABSTRACT_SYNTAX_TREE

#include <grt_definitions.h>
#include "variablelist.h"

/**
 Types of literal values in expressions: numerical value or variable reference
*/
typedef enum {
  variable_e,			// address of a referenced variable
  integer_e,			// value for numeric constant
  void_e				// There is no value in the element
} value_type;

/**
 Expression operator types. All operators are left-to-right and binary
*/
typedef enum operator_id {
	ASSIGN_RPN,			
	AND_RPN,			
	OR_RPN,				
	BITSHIFTLEFT_RPN,	
	BITSHIFTRIGHT_RPN,	
    EQUAL_RPN,			
	UNEQUAL_RPN,		
	SMALLER_RPN,		
	LARGER_RPN,			
	SMALLEREQ_RPN,		
	LARGEREQ_RPN,		
    PLUS_RPN,			
	MINUS_RPN,			
	MULTIPLY_RPN,		
	DIVIDE_RPN,			
	MODULUS_RPN,		
	XOR_RPN			
	} operator_id;

/**
 Syntax element type to distinguish the elements of a reverse polish notation stack
*/
typedef enum syntax_elt_type {
  LEFTPAR_RPN,				// Left parenthesis
  RIGHTPAR_RPN,				// Right parenthesis
  BINARY_RPN,				// binary expression
  VARIABLE_EVAL_RPN,		// variable reference for evaluation
  VARIABLE_ASSIGN_RPN,		// variable reference for assignment
  NUM_RPN,					// number
  VOID_RPN,					// empty element showing that the element is not used (end of an expression)
  PATTERN_DELIMITER_RPN		// End of a pattern
} syntax_elt_type;

/**
 Syntax element to be used in a Reverse Polish Notation
*/
typedef struct abstract_syntax_elt{
    syntax_elt_type type;		// type of element stored
	union {
		operator_id op; // operator (must be a binary operator)
		variable* var;	// address of a referenced variable
		int num;		// value for numeric constant
	} elt_union;
} abstract_syntax_elt;

/**
 * Generator of a Reverse Polish Notation queue from the parsing of an expression. 
 * The algorithm used is the Shunting-Yard algorithm. The algorithm necessitates
 * a temporary stack.
 * 
 * @author mpelcat
 */

class ReversePolishNotationGenerator {

	private :
	/**
	  Statically allocated table of syntax elements in the stack
	 */
    abstract_syntax_elt stack[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
	/**
	  current size of the stack
	 */
	int stackSize;

	/**
	  Output queue for the Shunting-yard algorithm with its head and tail indexes
	 */
    abstract_syntax_elt outputQueue[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
	int headIndex;
	int tailIndex;
	int queueSize;

	public : 
		/**
		 Constructor
		*/
		ReversePolishNotationGenerator();

		/**
		 Destructor
		*/
		~ReversePolishNotationGenerator();

		/**
		 Getting the queue of abstract elements

		 @param extQueue: Queue of the executable abstract elements
		*/
		void getQueue(abstract_syntax_elt* extQueue);




		//Stack
		/**
		 Pushing an operator with its type in the stack
		 
		 @param eltType: type: operator or value (distinguishes unary and binary operators
		 @param op: operator
		*/
		void push(syntax_elt_type eltType, operator_id op);

		/**
		 Pushing a variable reference in the stack
		 
		 @param eltType: type: variable for assignment or for evaluation
		 @param var: reference to a variable
		*/
		void push(syntax_elt_type eltType, variable* var);

		/**
		 Pushing a numerical value in the stack
		 
		 @param num: numerical value of an element
		*/
		void push(int num);

		/**
		 Pushing a Left Parenthesis in the stack
		*/
		void pushLeftParenthesis();

		/**
		 Pushing a Right Parenthesis in the stack
		*/
		void pushRightParenthesis();

		/**
		 Popping an element from the stack

		 @return: element
		*/
		abstract_syntax_elt* pop();

		/**
		 Peeking the element at the top of the stack

		 @return: element
		*/
		abstract_syntax_elt* peek();

		/**
		 Getting the stack size
		 
		 @return: the stack size
		*/
		int getStackSize();




		//Queue
		/**
		 Enqueueing an operator in the output queue
		 
		 @param eltType: type: operator or value (distinguishes unary and binary operators
		 @param op: operator
		*/
		void enqueue(syntax_elt_type eltType, operator_id op);

		/**
		 Enqueueing a variable in the output queue
		 
		 @param eltType: type: variable for assignment or for evaluation
		 @param var: reference to a variable
		*/
		void enqueue(syntax_elt_type eltType, variable* var);
		
		/**
		 Enqueueing a numerical value in the output queue
		 
		 @param num: numerical value of an element
		*/
		void enqueue(int num);

		/**
		 Enqueueing an element

		 @param elt: the element
		*/
		void enqueue(abstract_syntax_elt* elt);
		
		/**
		 Dequeuing an element from the output queue
		 
		 @return: the element
		*/
		abstract_syntax_elt* dequeue();

		/**
		 Getting the queue size
		 
		 @return: the queue size
		*/
		int getQueueSize();
		
		/**
		 Flushes the stack and the queue
		*/
		void flush();
		
#ifdef DISPLAY
		/**
		 Prints the stack and the queue in the stdio
		*/
		void print();
#endif
};

#endif

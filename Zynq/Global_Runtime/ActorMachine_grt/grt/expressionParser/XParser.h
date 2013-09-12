/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef XPARSER_H
#define XPARSER_H

// declarations

#include "variablelist.h"
#include "ReversePolishNotationGenerator.h"
#include "../SchedulerDimensions.h"
/**
 Expression parser

 @author: mpelcat
*/
class XParser
{
    // public functions
    public:

		/**
		 Preparsing an expression to transform it into an expression stack

		 @param expr: string expression
		 @param stack: expression stack

		 @return the size of the expression in abstract_syntax_elt
		*/
        int parse(const char expr[], abstract_syntax_elt* stack);

		/**
		 Interpreting an expression queue to transform it into an integer

		 @param queue: expression queue
		 @param value: the return value
		 @return the number of interpreted abstract elements
		*/
		int interpret(const abstract_syntax_elt* queue, int* value);
		
		/**
		 Parsing an expression directly into an integer

		 @param expr: string expression
		 @return the output integer
		*/
        int parse(const char expr[]);

		void addVariable(const char* name, int value);

		/**
		 Singleton constructor
		*/
        XParser();

		/**
		 Resolving a pattern, i.e resolving each pattern element.
		 
		 @param stringPattern: the pattern to solve
		 @param intPattern: the solved pattern

		 @return the returned pattern size
		*/
		int parsePattern(const char* stringPattern, abstract_syntax_elt* eltPattern);

    private:
		ReversePolishNotationGenerator rpnGenerator;

		// For tokenizer
        enum TOKENTYPE {NOTHING = -1, DELIMITER, NUMBER, VARIABLE, FUNCTION, UNKNOWN};

		// current data of the parser
        char expr[EXPR_LEN_MAX+1];				// holds the expression
        char* e;								// points to a character in expr
        char token[VARIABLE_NAME_LEN_MAX+1];	// holds the token
        TOKENTYPE token_type;					// type of the token

        Variablelist user_var;					// list with variables defined by user

        void getToken();

        int getOperatorId(const char op_name[]);
        int evalOperator(const int op_id, const int lhs, const int rhs);
		
		/*
		 * Getting the precedence level of an operator

		 @return: level
		 */
		int getPrecedenceLevel(const int op_id);

		/**
		 Finds the first expression in the given pattern
		 
		 @param stringPattern: the pattern to solve
		 @param token: the found token
		 @return the size of the token
		*/
		int tokenizePattern(const char* stringPattern, char* token);

    public:
		/**
		 Interpreting an expression queue to pretty print it

		 @param stack: expression queue
		 @param string: output pretty printing
		*/
		void prettyPrint(abstract_syntax_elt* queue, char* string);

    private:
		/*
		 * display an operator with given strings
		 */
		void displayOperator(int op_id, char* lhs, char* rhs, char* output);


};

// TODO: Careful allocation
extern XParser globalParser;


#endif


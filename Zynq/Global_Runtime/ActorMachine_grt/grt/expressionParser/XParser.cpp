/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 Expression parser

 @author: mpelcat
*/

// declarations
#include "XParser.h"
#include "../tools/SchedulingError.h"
#include <cstdio>
#include <cstdlib>

// TODO: Careful allocation
// The parser object
XParser globalParser;


/*
 * checks if the given char c is a minus
 */
bool isMinus(const char c)
{
    return c == '-';
}

/*
 * checks if the given char c is whitespace
 * whitespace when space chr(32) or tab chr(9)
 */
bool isWhiteSpace(const char c)
{
    return c == 32 || c == 9;  // space or tab
}

/*
 * checks if the given char c is a delimeter
 * minus is checked apart, can be unary minus
 */
bool isOperator(const char c)
{
	return c == '&' || c == '|' || c == '=' || c == '+' || c == '-' || c == '/' || c == '*' || c == '%';
    //return strchr("&|<>=+-/*%^!", c) != 0;
	
	// Limited operators
	// return c == 37 || c == 38 || c == 42 || c == 43 || c == 47 || c == 61 || c == 124;
    //return strchr("&|=+/*%", c) != 0;
}

/*
 * checks if the given char c is a letter or underscore (maj or min)
 */
bool isAlpha(const char c)
{
    return ((c >= 97) && (c <= 122)) || ((c >= 65) && (c <= 90)) || (c == 95);
}

/*
 * checks if the given char c is a digit
 */
bool isDigit(const char c)
{
    return ((c >= 48) && (c <= 57));
}

/*
 * constructor. 
 * Initializes all data with zeros and empty strings
 */
XParser::XParser()
{
    expr[0] = '\0';
    e = NULL;

    token[0] = '\0';
    token_type = NOTHING;
}

/**
 Interpreting an expression queue to transform it into an integer

 @param queue: expression queue
 @param value: the return value
 @return the number of interpreted abstract elements
*/
int XParser::interpret(const abstract_syntax_elt* queue, int* value){
	static int parseStack[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
	int parseStackSize = 0;
	variable* assignVariable = NULL;
	int index=0;

    //While there are input tokens left
	for(; queue[index].type != VOID_RPN; index++){
        // Read the next token from input.
		const abstract_syntax_elt* elt = &queue[index];
		switch(elt->type){
			case VARIABLE_EVAL_RPN:{
				// globalParserIf the token is a value Push it onto the stack.
				parseStack[parseStackSize] = elt->elt_union.var->value;
				parseStackSize++;
				break;
			}
			case VARIABLE_ASSIGN_RPN:{
				// If the expression is assigned  in a variable, its corresponding variable is kept
				assignVariable = elt->elt_union.var;
				break;
			}
			case NUM_RPN:{
				// If the token is a value Push it onto the stack.
				parseStack[parseStackSize] = elt->elt_union.num;
				parseStackSize++;
				break;
			}
			case BINARY_RPN:{
				// Otherwise, the token is an operator (operator here includes both operators, and functions).
				// It is known a priori that the operator takes n arguments.
					// If there are fewer than n values on the stack
						// (Error not checked) The user has not input sufficient values in the expression.
				
				// Else, Pop the top n values from the stack.
				// Evaluate the operator, with the values as arguments.
				// Push the returned results, if any, back onto the stack.
				operator_id op = elt->elt_union.op;
				if(op == ASSIGN_RPN){
#ifdef _DEBUG
					if(assignVariable == NULL || parseStackSize != 1){
						// Problem with variable assignment
						exitWithCode(1038);
					}
#endif
					assignVariable->value = parseStack[0];
				}
				else{
					int val2 = parseStack[parseStackSize-1];
					int val1 = parseStack[parseStackSize-2];
					int val = evalOperator(elt->elt_union.op, val1, val2);
					
					parseStack[parseStackSize-2] = val;
					parseStackSize--;
				}
				break;
			}
			default:
				break;
		}
	}

	// If there is only one value in the stack
    // That value is the result of the calculation.
    // If there are more values in the stack
    // (Error) The user input has too many values.

#ifdef _DEBUG
	if(parseStackSize != 1){
		// Failure in RPN parsing
		exitWithCode(1036);
	}
#endif

	*value = parseStack[0];
	return index;
}

/**
 Parsing an expression to transform it into an expression queue.
 We use the Shunting-yard algorithm.

 @param expr: string expression
 @param stack: expression queue

 @return the size of the expression in abstract_syntax_elt
*/
int XParser::parse(const char new_expr[], abstract_syntax_elt* queue){

	int firstToken = 1;
	rpnGenerator.flush();

	// check the length of expr
    if (strlen(new_expr) > EXPR_LEN_MAX)
    {
		// Too long expression
		exitWithCode(1013);
    }

    // initialize all variables
    strcpy(expr, new_expr);     // copy the given expression to expr
    e = expr;                   // let e point to the start of the expression

	do{
		getToken(); // Read a token.

		switch (token_type)
		{
			case NUMBER:
				// If the token is a number, then add it to the output queue.
				rpnGenerator.enqueue(atoi(token));
				break;

			case VARIABLE:
				if(firstToken){
					char name[VARIABLE_NAME_LEN_MAX];
					strcpy(name, token);
					getToken();
					if(token[0] == '='&& token[1] == '\0'){
						// If the token is an assign variable, add it with the assignment to the output queue.
						user_var.add(name,0);
						rpnGenerator.push(BINARY_RPN,ASSIGN_RPN);
						rpnGenerator.push(VARIABLE_ASSIGN_RPN,user_var.getVariable(name));
					}
					else{
						// From the top
						e = expr;
						getToken();
						// If the token is a variable, then add it to the output queue.
						rpnGenerator.enqueue(VARIABLE_EVAL_RPN,user_var.getVariable(token));
					}
				}
				else{
					// If the token is a variable, then add it to the output queue.
					rpnGenerator.enqueue(VARIABLE_EVAL_RPN,user_var.getVariable(token));
				}
				break;

			case DELIMITER: // operator
				if(isOperator(token[0])){
					// If the token is an operator, o1, then:
					//  while there is an operator, o2, at the top of the stack and
					//   o1 is left-associative and its precedence is more than or equal (precedence is in degressive order) 
					//   to that of o2, pop o2 off the stack, onto the output queue;

					operator_id o1id = (operator_id)getOperatorId(token);

					while(rpnGenerator.getStackSize()>0 && rpnGenerator.peek()->type == BINARY_RPN){
						abstract_syntax_elt* o2 = rpnGenerator.peek();
						if(getPrecedenceLevel(o1id) >= getPrecedenceLevel(o2->elt_union.op)){
							rpnGenerator.pop();
							rpnGenerator.enqueue(o2);
						}
						else{
							break;
						}
					}
					//  push o1 onto the stack.
					rpnGenerator.push(BINARY_RPN,o1id);
				}
				else if(token[0] == '('){
					// If the token is a left parenthesis, then push it onto the stack
					rpnGenerator.pushLeftParenthesis();
				}
				else if(token[0] == ')'){
					// Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue.
					// Pop the left parenthesis from the stack, but not onto the output queue.
					// If the token at the top of the stack is a function token, pop it onto the output queue.
					// If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.

					while(1){
						abstract_syntax_elt* elt = rpnGenerator.pop();
						if(elt->type == LEFTPAR_RPN){
							break;
						}
						else if(rpnGenerator.getStackSize() == 0){
							// Parser parenthesis mismatch. Left parenthesis missing
							exitWithCode(1033);
						}
						else{
							rpnGenerator.enqueue(elt);
						}
					}
				}
				else if(token[0] == '\0'){
					// While there are still operator tokens in the stack:
					//  If the operator token on the top of the stack is a parenthesis, then there are mismatched parentheses.
					//  Pop the operator onto the output queue.

					while(!rpnGenerator.getStackSize()==0){
						abstract_syntax_elt* elt = rpnGenerator.pop();
						if(elt->type == LEFTPAR_RPN || elt->type == RIGHTPAR_RPN){
							// Parser parenthesis mismatch
							exitWithCode(1032);
						}
						else if(elt->type == BINARY_RPN || elt->type == VARIABLE_ASSIGN_RPN){
							rpnGenerator.enqueue(elt);
						}
					}
				}
				break;
	            
			default:
				// Unrecognized token
				if (token[0] == '\0')
				{
					exitWithCode(1028);
				}
				break;
		}

#ifdef DISPLAY
		rpnGenerator.print();
#endif
		firstToken = 0;

	}while(token[0] != '\0');

	rpnGenerator.getQueue(queue);
	return rpnGenerator.getQueueSize();
}

/**
 Parsing a pattern, i.e generating RPN for each pattern element.
 
 @param stringPattern: the pattern to solve
 @param eltPattern: the solved pattern

 @return the pattern size in abstract_syntax_elt
*/
int XParser::parsePattern(const char* stringPattern, abstract_syntax_elt* eltPattern){
	// We parse the { , and } characters to tokenize the string pattern.
	// Each token is an expression parsed by the XParser
	char currentExpr[EXPR_LEN_MAX];
	int patternSize = 0; // size of the pattern in abstract elements
	int patternNb = 0;
	char c = *(stringPattern);

	// Pattern case
	if(c == '{'){
		while(c != '}'){
			stringPattern++; // removing the separator
			int size = tokenizePattern(stringPattern, currentExpr);
			if(size>0){
				patternSize += (parse(currentExpr,&eltPattern[patternSize]) + 1);
				stringPattern += size;
			}
			patternNb++;
			c = *(stringPattern);
		}
	}
	// Variable case
	else{
		parse(stringPattern,eltPattern);
	}
	eltPattern[patternSize].type = PATTERN_DELIMITER_RPN;
	patternSize++;

//	return patternSize;
	return patternNb;
}

/**
 Finds the first expression in the given pattern
 
 @param stringPattern: the pattern to solve
 @param token: the found token
 @return the size of the token
*/
int XParser::tokenizePattern(const char* stringPattern, char* token){
	char c = *(stringPattern++);
	int size = 0;
	while((c != ',') && (c != '}') && (c != '\0')){
		*(token++) = c;
		c = *(stringPattern++);
		size++;
	}
	*token = '\0';

	return size;
}

/**
 * parses and evaluates the given expression
 * On error, an error of type Error is thrown
 */
int XParser::parse(const char new_expr[])
{
	static abstract_syntax_elt queue[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
	parse(new_expr, queue);
	int value;
	interpret(queue,&value);
	return value;
}

/**
 * Adding explicitely a variable to the parser
 */
void XParser::addVariable(const char* name, int value){
	user_var.add(name, value);
}

/**
 * Get next token in the current string expr.
 * Uses the Parser data expr, e, token, t, token_type and err
 */
void XParser::getToken()
{
    token_type = NOTHING;
    char* t;           // points to a character in token
    t = token;         // let t point to the first character in token
    *t = '\0';         // set token empty

    //printf("\tgetToken e:{%c}, ascii=%i, col=%i\n", *e, *e, e-expr);

    // skip over whitespaces
    while (*e == ' ' || *e == '\t')     // space or tab
    {
        e++;
    }

    // check for end of expression
    if (*e == '\0')
    {
        // token is still empty
        token_type = DELIMITER;
        return;
    }

    // check for minus
    if (*e == '-')
    {
        token_type = DELIMITER;
        *t = *e;
        e++;
        t++;
        *t = '\0';  // add a null character at the end of token
        return;
    }

    // check for parentheses
    if (*e == '(' || *e == ')')
    {
        token_type = DELIMITER;
        *t = *e;
        e++;
        t++;
        *t = '\0';
        return;
    }

    // check for operators (delimeters)
    if (isOperator(*e))
    {
        token_type = DELIMITER;
        while (isOperator(*e))
        {
            *t = *e;
            e++;
            t++;
        }
        *t = '\0';  // add a null character at the end of token
        return;
    }
    
    // check for a value
    if (isDigit(*e))
    {
        token_type = NUMBER;
        while (isDigit(*e))
        {
            *t = *e;
            e++;
            t++;
        }

        *t = '\0';
        return;
    }

    // check for variables or functions
    if (isAlpha(*e))
    {
        while (isAlpha(*e) || isDigit(*e))
        {
            *t = *e;
            e++;
            t++;
        }
        *t = '\0';  // add a null character at the end of token

        // check if this is a variable or a function.
        // a function has a parentesis '(' open after the name 
        char* e2 = NULL;
        e2 = e;

        // skip whitespaces
        while (*e2 == ' ' || *e2 == '\t')     // space or tab
        {
            e2++;
        }
        
        if (*e2 == '(') 
        {
            token_type = FUNCTION;
        }
        else
        {
            token_type = VARIABLE;
        }
        return;
    }

    // something unknown is found, wrong characters -> a syntax error
    token_type = UNKNOWN;
    while (*e != '\0')
    {
        *t = *e;
        e++;
        t++;
    }
    *t = '\0';

    return;
}

/*
 * returns the id of the given operator
 * treturns -1 if the operator is not recognized
 */
int XParser::getOperatorId(const char op_name[])
{
    // level 2
    if (!strcmp(op_name, "&")) {return AND_RPN;}
    if (!strcmp(op_name, "|")) {return OR_RPN;}
    if (!strcmp(op_name, "<<")) {return BITSHIFTLEFT_RPN;}
    if (!strcmp(op_name, ">>")) {return BITSHIFTRIGHT_RPN;}

    // level 3
    if (!strcmp(op_name, "=")) {return EQUAL_RPN;}
    if (!strcmp(op_name, "<>")) {return UNEQUAL_RPN;}
    if (!strcmp(op_name, "<")) {return SMALLER_RPN;}
    if (!strcmp(op_name, ">")) {return LARGER_RPN;}
    if (!strcmp(op_name, "<=")) {return SMALLEREQ_RPN;}
    if (!strcmp(op_name, ">=")) {return LARGEREQ_RPN;}

    // level 4
    if (!strcmp(op_name, "+")) {return PLUS_RPN;}
    if (!strcmp(op_name, "-")) {return MINUS_RPN;}

    // level 5
    if (!strcmp(op_name, "*")) {return MULTIPLY_RPN;}
    if (!strcmp(op_name, "/")) {return DIVIDE_RPN;}
    if (!strcmp(op_name, "%")) {return MODULUS_RPN;}
    if (!strcmp(op_name, "||")) {return XOR_RPN;}

    // level 6
    //if (!strcmp(op_name, "^")) {return POW;}

    // level 7
    //if (!strcmp(op_name, "!")) {return FACTORIAL;}

	// Operator is not recognized
    return -1;
}

/*
 * Getting the precedence level of an operator

 @return: level
 */
int XParser::getPrecedenceLevel(const int op_id){

/*/////////////////////////////////////////////////
Using C-like precedence levels
///////////////////////////////////////////////////
1 	()   []   ->   .   :: 	Grouping, scope, array/member access
2 	 !   ~   -   +   *   &   sizeof   type cast ++x   --x   	(most) unary operations, sizeof and type casts
3 	*   /   % 	Multiplication, division, modulo
4 	+   - 	Addition and subtraction
5 	<<   >> 	Bitwise shift left and right
6 	<   <=   >   >= 	Comparisons: less-than, ...
7 	==   != 	Comparisons: equal and not equal
8 	& 	Bitwise AND_RPN
9 	^ 	Bitwise exclusive OR_RPN
10 	| 	Bitwise inclusive (normal) OR_RPN
11 	&& 	Logical AND_RPN
12 	|| 	Logical OR_RPN
13 	 ?: 	Conditional expression (ternary operator)
14 	=   +=   -=   *=   /=   %=   &=   |=   ^=   <<=   >>= 	Assignment operators
*///////////////////////////////////////////////////

    switch (op_id)
    {
        case AND_RPN:           return 8;
        case OR_RPN:            return 10;
        case BITSHIFTLEFT_RPN:  return 5;
        case BITSHIFTRIGHT_RPN: return 5;

        case EQUAL_RPN:     return 7;
        case UNEQUAL_RPN:   return 7;
        case SMALLER_RPN:   return 6;
        case LARGER_RPN:    return 6;
        case SMALLEREQ_RPN: return 6;
        case LARGEREQ_RPN:  return 6;
        
        case PLUS_RPN:      return 4;
        case MINUS_RPN:     return 4;
        
        case MULTIPLY_RPN:  return 3;
        case DIVIDE_RPN:    return 3;
        case MODULUS_RPN:   return 3;
        case XOR_RPN:       return 9;

        case ASSIGN_RPN:    return 14;
    }
   
	// Operator is not recognized
	exitWithCode(1022);
    return -1;
}

/*
 * evaluate an operator for given valuess
 */
int XParser::evalOperator(const int op_id, const int lhs, const int rhs)
{
    switch (op_id)
    {
        case AND_RPN:           return (int)(lhs) & (int)(rhs);
        case OR_RPN:            return (int)(lhs) | (int)(rhs);
        case BITSHIFTLEFT_RPN:  return (int)(lhs) << (int)(rhs);
        case BITSHIFTRIGHT_RPN: return (int)(lhs) >> (int)(rhs);

        case EQUAL_RPN:     return lhs == rhs;
        case UNEQUAL_RPN:   return lhs != rhs;
        case SMALLER_RPN:   return lhs < rhs;
        case LARGER_RPN:    return lhs > rhs;
        case SMALLEREQ_RPN: return lhs <= rhs;
        case LARGEREQ_RPN:  return lhs >= rhs;
        
        case PLUS_RPN:      return lhs + rhs;
        case MINUS_RPN:     return lhs - rhs;
        
        case MULTIPLY_RPN:  return lhs * rhs;
        case DIVIDE_RPN:    if(rhs==0)return 0; 
							else return lhs / rhs;
        case MODULUS_RPN:   return (int)(lhs) % (int)(rhs);
        case XOR_RPN:       return (int)(lhs) ^ (int)(rhs);
    }
   
	// Operator is not recognized
	exitWithCode(1022);
    return -1;
}

/**
 Interpreting an expression queue to pretty print it

 @param stack: expression queue
 @param string: output pretty printing
*/
void XParser::prettyPrint(abstract_syntax_elt* queue, char* string){
	static char parseStack[REVERSE_POLISH_STACK_MAX_ELEMENTS+1][EXPR_LEN_MAX];
	int parseStackSize = 0;
	static char assignVariable[EXPR_LEN_MAX];

    //While there are input tokens left
	for(int i=0; (queue[i].type != VOID_RPN); i++){
        // Read the next token from input.
		abstract_syntax_elt* elt = &queue[i];
		switch(elt->type){
			case VARIABLE_EVAL_RPN:{
				// If the token is a value Push it onto the stack.
				strcpy(parseStack[parseStackSize],elt->elt_union.var->name);
				parseStackSize++;
				break;
			}
			case VARIABLE_ASSIGN_RPN:{
				// If the expression is assigned  in a variable, its corresponding variable is kept
				strcpy(assignVariable,elt->elt_union.var->name);
				break;
			}
			case NUM_RPN:{
				// If the token is a value Push it onto the stack.
				sprintf(parseStack[parseStackSize],"%d",elt->elt_union.num);
				parseStackSize++;
				break;
			}
			case BINARY_RPN:{
				// Otherwise, the token is an operator (operator here includes both operators, and functions).
				// It is known a priori that the operator takes n arguments.
					// If there are fewer than n values on the stack
						// (Error not checked) The user has not input sufficient values in the expression.
				
				// Else, Pop the top n values from the stack.
				// Evaluate the operator, with the values as arguments.
				// Push the returned results, if any, back onto the stack.
				operator_id op = elt->elt_union.op;
				if(op == ASSIGN_RPN){
					if(strlen(assignVariable) != 0 && parseStackSize == 1){
						sprintf(parseStack[0],"%s=%s",assignVariable,parseStack[0]);
					}
					else{
						// Problem with variable assignment
						exitWithCode(1038);
					}
				}
				else{
					char* val2 = parseStack[parseStackSize-1];
					char* val1 = parseStack[parseStackSize-2];
					strcpy(string,val1);
					displayOperator(elt->elt_union.op, string, val2,parseStack[parseStackSize-2]);
					parseStackSize--;
				}
				break;
			}
			default:
				break;
		}
	}

	// If there is only one value in the stack
    // That value is the result of the calculation.
    // If there are more values in the stack
    // (Error) The user input has too many values.

	if(parseStackSize != 1){
		// Failure in RPN parsing
		exitWithCode(1036);
	}

	strcpy(string,parseStack[0]);
}

/*
 * display an operator with given strings
 */
void XParser::displayOperator(int op_id, char* lhs, char* rhs, char* output)
{
	char op[3] = "\0\0";
    switch (op_id)
    {
        case AND_RPN:           op[0]='&';
								break;
        case OR_RPN:            op[0]='|';
								break;
        case BITSHIFTLEFT_RPN:  op[0]='<';
								op[1]='<';
								break;
        case BITSHIFTRIGHT_RPN: op[0]='>';
								op[1]='>';
								break;

        case EQUAL_RPN:			op[0]='=';
								op[1]='=';
								break;
        case UNEQUAL_RPN:		op[0]='!';
								op[1]='=';
								break;
        case SMALLER_RPN:		op[0]='<';
								break;
        case LARGER_RPN:		op[0]='>';
								break;
        case SMALLEREQ_RPN:		op[0]='<';
								op[1]='=';
								break;
        case LARGEREQ_RPN:		op[0]='>';
								op[1]='=';
								break;
        
        case PLUS_RPN:			op[0]='+';
								break;
        case MINUS_RPN:			op[0]='-';
								break;
        
        case MULTIPLY_RPN:		op[0]='*';
								break;
        case DIVIDE_RPN:		op[0]='/';
								break;
        case MODULUS_RPN:		op[0]='%';
								break;
        case XOR_RPN:			op[0]='^';
								break;
    }

	sprintf(output,"(%s %s %s)",lhs,op,rhs);
}

/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 * An edge in a delay-less CSDAG (cyclo-static dataflow) graph (no initial token).
 * The base unit of production and consumption is a char (8 bits).
 * The space reserved for tokens can be filled with a structure defining buffers.
 * 
 * @author mpelcat
 */
#include "CSDAGEdge.h"
#include "CSDAGGraph.h"
#include "../../tools/SchedulingError.h"

/**
 Constructor

 @param base: graph in which the edge is added
*/
CSDAGEdge::CSDAGEdge(){
	base = NULL;
	source = NULL;
	sink = NULL;
}

/**
 Destructor
*/
CSDAGEdge::~CSDAGEdge()
{
}

/**
 Resolving a pattern from a vertex parameters. A CSDAG edge has patterns of production/consumption.
 A SRDAG edge has a unique value of production=consumption. The resolution generates an integer
 pattern from the source and sink parameters and the pattern. The source and sink parameters
 are solvedimmediately before the pattern is resolved because they can influence the way it is solved.
 
 @param pattern: the input expression
 @param vertex: the vertex which parameters must be solved to determine the output pattern
 @param intPattern: the solved pattern
 @param setVertexParameters: 1 if we need to set the parameters of the instances of vertex

 @return the returned pattern size
*/
unsigned short CSDAGEdge::resolvePattern(const abstract_syntax_elt* expression, CSDAGVertex* vertex, int* intPattern, char setVertexParameters){
	// Resolving the vertex patterns

	static int patternIndex[MAX_PARAM];
	unsigned short intPatternIndex=0;
	static int values[MAX_PARAM];	// parameter values
	int value=0;	// Currentpattern value
	abstract_syntax_elt* currentParamExpr = NULL;
	int nbPatternsReachCompletion = 0;

	int paramNb = vertex->getParamNb();
	if(paramNb > 0){
		for(int i=0; i<MAX_PARAM; i++){
			patternIndex[i] = 0;
		}

		while(nbPatternsReachCompletion < paramNb){
			// Evaluating the patterns to have the right variable values at the right time
			for(int i=0;i<paramNb;i++){
				currentParamExpr = vertex->getParamPattern(i);
				patternIndex[i] += globalParser.interpret(&currentParamExpr[patternIndex[i]], &values[i]) + 1;
				if(currentParamExpr[patternIndex[i]].type == PATTERN_DELIMITER_RPN){
					patternIndex[i] = 0;
					nbPatternsReachCompletion++;
				}
			}

			// Resolving the current expression
			globalParser.interpret(expression, &value);
			if(value != 0){
				intPattern[intPatternIndex] = value;

				// The parameters are taken into account and a parameter integer pattern is stored in the csdag vertex
				if(setVertexParameters){
					for(int i=0;i<paramNb;i++){
						vertex->setParamValue(intPatternIndex,i,values[i]);
					}
				}
				intPatternIndex++;
			}
		}
	}
	else{ // No parameters: the pattern is a simple expression
		globalParser.interpret(expression, &value);
		if(value != 0){
			intPattern[0] = value;
			intPatternIndex = 1;
		}
		else{
			intPatternIndex = 0;
		}
	}

	return intPatternIndex;

}

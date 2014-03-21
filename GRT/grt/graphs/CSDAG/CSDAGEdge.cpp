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

#include "CSDAGEdge.h"
#include "CSDAGGraph.h"
#include "../../tools/SchedulingError.h"

/**
 * An edge in a delay-less CSDAG (cyclo-static dataflow) graph (no initial token).
 * The base unit of production and consumption is a char (8 bits).
 * The space reserved for tokens can be filled with a structure defining buffers.
 * 
 * @author mpelcat
 */

/**
 Constructor

 @param base: graph in which the edge is added
*/
CSDAGEdge::CSDAGEdge(){
	base = (CSDAGGraph*) 	NULL;
	source = (CSDAGVertex*)	NULL;
	sink = (CSDAGVertex*)	NULL;
	consumptionPatternNb = 0;
	productionPatternNb = 0;
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
 are solved immediately before the pattern is resolved because they can influence the way it is solved.
 
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
	abstract_syntax_elt* currentParamExpr = (abstract_syntax_elt*)NULL;
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

void CSDAGEdge::resolveProdCons(){
	// Resolving the edge consumption production
	globalParser.interpret(production,  productionValues);
	globalParser.interpret(consumption, consumptionValues);
}

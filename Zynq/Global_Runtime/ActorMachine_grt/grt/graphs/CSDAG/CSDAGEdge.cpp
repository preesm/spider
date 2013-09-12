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
 are solvedimmediately before the pattern is resolved because they can influence the way it is solved.
 
 @param pattern: the input expression
 @param vertex: the vertex which parameters must be solved to determine the output pattern
 @param intPattern: the solved pattern
 @param setVertexParameters: 1 if we need to set the parameters of the instances of vertex

 @return the returned pattern size
*/


void CSDAGEdge::resolveProdCons(){
	// Resolving the edge consumption production
	globalParser.interpret(production,  productionValues);
	globalParser.interpret(consumption, consumptionValues);
}

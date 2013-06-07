/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef _EDGE
#define _EDGE

#include "../../SchedulerDimensions.h"
#include "../../expressionParser/XParser.h"
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>

#include "CSDAGVertex.h"
class CSDAGGraph;

/**
 * An edge in a delay-less CSDAG (cyclo-static dataflow) graph (no initial token).
 * The base unit of production and consumption is a char (8 bits).
 * The space reserved for tokens can be filled with a structure defining buffers.
 * 
 * @author mpelcat
 */
class CSDAGEdge {

	private :
		/**
		 Expression defining the token production of the edge source (in abstract_syntax_elt)
		*/
		abstract_syntax_elt production[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
		/**
		 Expression defining the token consumption of the edge sink (in abstract_syntax_elt)
		*/
		abstract_syntax_elt consumption[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
		/**
		 Expression defining the delay (in abstract_syntax_elt)
		*/
		abstract_syntax_elt delay[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
		/**
		 Edge source
		*/
		CSDAGVertex* source;
		/**
		 Edge sink
		*/
		CSDAGVertex* sink;

		/**
		 Graphe
		*/
		CSDAGGraph* base;
	public : 
		/**
		 Constructor
		*/
		CSDAGEdge();

		/**
		 Destructor
		*/
		~CSDAGEdge();

		/**
		 setting the base graph

		 @param base: graph in which the edge is added
		*/
		void setBase(CSDAGGraph* base);

		/**
		 Production getter. 

		 @return production 
		*/
		abstract_syntax_elt* getProduction();

		/**
		 Production setter

		 @param expression defining the token production rate
		*/
		void setProduction(const char* production);

		
		/**
		 Consumption getter. 

		 @return consumption
		*/
		abstract_syntax_elt* getConsumption();

		/**
		 Consumption setter

		 @param expression defining the token consumption rate
		*/
		void setConsumption(const char* consumption);

		
		/**
		 Delay getter.

		 @return delay
		*/
		abstract_syntax_elt* getDelay();

		/**
		 Delay setter

		 @param expression defining the delay
		*/
		void setDelay(const char* delay);


		/**
		 Source getter
		*/
		CSDAGVertex* getSource();
		
		/**
		 Source setter
		*/
		void setSource(CSDAGVertex* source);

		/**
		 Sink getter
		*/
		CSDAGVertex* getSink();

		/**
		 Sink setter
		*/
		void setSink(CSDAGVertex* sink);
		
		/**
		 Resolving the source pattern
		 
		 @param intPattern: the solved pattern

		 @return the returned pattern size
		*/
		unsigned short resolveSourcePattern(int* intPattern);
		
		/**
		 Resolving the sink pattern
		 
		 @param intPattern: the solved pattern

		 @return the returned pattern size
		*/
		unsigned short resolveSinkPattern(int* intPattern);

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
		unsigned short resolvePattern(const abstract_syntax_elt* expression, CSDAGVertex* vertex, int* intPattern, char setVertexParameters);

};

/**
 setting the base graph

 @param base: graph in which the edge is added
*/
inline void CSDAGEdge::setBase(CSDAGGraph* base){
	this->base = base;
}

/**
 production pattern getter

 @return production expression
*/
inline abstract_syntax_elt* CSDAGEdge::getProduction()
{
	return(this->production);
}

/**
 production setter. Careful! Not made to set it more than once!

 @param production: expression defining the token production of the edge source (in char)
 @return production after resolving the expression
*/
inline void CSDAGEdge::setProduction(const char* prod){
	
	// Parsing the expression
	globalParser.parse(prod,this->production);
}

/**
 consumption getter

 @return consumption expression
*/
inline abstract_syntax_elt* CSDAGEdge::getConsumption()
{
	return(this->consumption);
}

/**
 consumption setter. Careful! Not made to set it more than once!

 @param consumption: expression defining the token consumption of the edge sink (in char)
 @return consumption after resolving the expression
*/
inline void CSDAGEdge::setConsumption(const char* cons)
{
	// Parsing the expression
	globalParser.parse(cons,this->consumption);
}

/**
 delay getter

 @return delay expression
*/
inline abstract_syntax_elt* CSDAGEdge::getDelay()
{
	return(this->delay);
}

/**
 delay setter. Careful! Not made to set it more than once!

 @param delay: expression defining the delay (in char)
 @return delay after resolving the expression
*/
inline void CSDAGEdge::setDelay(const char* cons)
{
	// Parsing the expression
	globalParser.parse(cons,this->delay);
}



/**
 Source getter
*/
inline CSDAGVertex* CSDAGEdge::getSource(){
	return this->source;
}

/**
 Source setter
*/
inline void CSDAGEdge::setSource(CSDAGVertex* source){
	this->source = source;
}

/**
 Sink getter
*/
inline CSDAGVertex* CSDAGEdge::getSink(){
	return this->sink;
}

/**
 Sink setter
*/
inline void CSDAGEdge::setSink(CSDAGVertex* sink){
	this->sink = sink;
}
		
/**
 Resolving the source pattern from source vertex parameters and production pattern
 
 @param intPattern: the solved pattern

 @return the returned pattern size
*/
inline unsigned short CSDAGEdge::resolveSourcePattern(int* intPattern){
	return resolvePattern(production, source,intPattern, 0);
}

/**
 Resolving the sink pattern from sink vertex parameters and consumption pattern
 
 @param intPattern: the solved pattern

 @return the returned pattern size
*/
inline unsigned short CSDAGEdge::resolveSinkPattern(int* intPattern){
	return resolvePattern(consumption, sink,intPattern, 1);
}

#endif

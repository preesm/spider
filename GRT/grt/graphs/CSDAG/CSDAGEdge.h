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

#ifndef CSDAG_EDGE
#define CSDAG_EDGE

#include <grt_definitions.h>
#include <expressionParser/XParser.h>
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
		 expression defining the token production of the edge source (in abstract_syntax_elt)
		*/
		abstract_syntax_elt production[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
		int productionValues[MAX_CSDAG_VERTEX_REPETITION];
		int productionPatternNb;

		/**
		 expression defining the token consumption of the edge sink (in abstract_syntax_elt)
		*/
		abstract_syntax_elt consumption[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
		int consumptionValues[MAX_CSDAG_VERTEX_REPETITION];
		int consumptionPatternNb;

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
		int getResolvedProduction(int patternId);
		int* getResolvedProductionPattern();
		int getResolvedProductionNb();

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
		int getResolvedConsumption(int patternId);
		int* getResolvedConsumptionPattern();
		int getResolvedConsumptionNb();

		/**
		 Consumption setter

		 @param expression defining the token consumption rate
		*/
		void setConsumption(const char* consumption);

		
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
		 Resolving the production and consumption patterns
		*/
		void resolveProdCons();

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
		 are solvedimmediately before the pattern is resolved because they can influence the way it is solved.
		 
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

inline int CSDAGEdge::getResolvedProduction(int patternId){
	if(patternId > this->productionPatternNb){
		exitWithCode(105);
		return 0;
	}
	return this->productionValues[patternId];
}

inline int* CSDAGEdge::getResolvedProductionPattern(){
	return this->productionValues;
}

inline int CSDAGEdge::getResolvedProductionNb(){
	return this->productionPatternNb;
}

/**
 production setter. Careful! Not made to set it more than once!

 @param production: expression defining the token production of the edge source (in char)
 @return production after resolving the expression
*/
inline void CSDAGEdge::setProduction(const char* prod){
	
	// Parsing the expression
//	productionPatternNb = globalParser.parsePattern(prod,this->production);
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

inline int CSDAGEdge::getResolvedConsumption(int patternId){
	if(patternId > this->consumptionPatternNb){
		exitWithCode(105);
		return 0;
	}
	return this->consumptionValues[patternId];
}

inline int* CSDAGEdge::getResolvedConsumptionPattern(){
	return this->consumptionValues;
}

inline int CSDAGEdge::getResolvedConsumptionNb(){
	return this->consumptionPatternNb;
}

/**
 consumption setter. Careful! Not made to set it more than once!

 @param consumption: expression defining the token consumption of the edge sink (in char)
 @return consumption after resolving the expression
*/
inline void CSDAGEdge::setConsumption(const char* cons)
{
	// Parsing the expression
//	consumptionPatternNb = globalParser.parsePattern(cons,this->consumption);
	globalParser.parse(cons,this->consumption);
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

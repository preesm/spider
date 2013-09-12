/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef CSDAG_EDGE
#define CSDAG_EDGE

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
		 expression defining the token production of the edge source (in abstract_syntax_elt)
		*/
		abstract_syntax_elt production[REVERSE_POLISH_STACK_MAX_ELEMENTS+1];
		int productionValues[MAX_CSDAG_VERTEX_REPETITION];
		int productionPatternNb;

		/**
		 ecpression defining the token consumption of the edge sink (in abstract_syntax_elt)
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
inline abstract_syntax_elt* CSDAGEdge::getProduction(){
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
	productionPatternNb = globalParser.parsePattern(prod,this->production);
}

/**
 consumption getter

 @return consumption expression
*/
inline abstract_syntax_elt* CSDAGEdge::getConsumption(){
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
	consumptionPatternNb = globalParser.parsePattern(cons,this->consumption);
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

#endif

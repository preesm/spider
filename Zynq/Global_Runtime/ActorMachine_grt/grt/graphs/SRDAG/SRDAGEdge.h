/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef SRDAG_EDGE
#define SRDAG_EDGE

#include "SRDAGVertex.h"

#ifdef LINUX
#define inline inline
#endif

/**
 * An edge in a delay-less SRDAG graph (no initial token). 
 * The base unit of production and consumption is a char (8 bits).
 * The space reserved for tokens can be filled with a structure.
 * 
 * @author mpelcat
 */
class SRDAGEdge {

	private :
		/**
		 token rate (solved and not depending on an expression). 
		 tokenRate = -1 means that the edge only represents a precedence
		*/
		int tokenRate;

		/**
		 Edge source
		*/
		SRDAGVertex* source;
		/**
		 Edge sink
		*/
		SRDAGVertex* sink;

	public : 
		/**
		 Constructor
		*/
		SRDAGEdge();

		/**
		 Destructor
		*/
		~SRDAGEdge();

		/**
		 TokenRate getter

		 @return production after resolving the expression
		*/
		int getTokenRate();

		/**
		 TokenRate setter

		 @param integer defining the token rate
		*/
		void setTokenRate(int rate);

		/**
		 Source getter

		 @return the source
		*/
		SRDAGVertex* getSource();

		/**
		 Sink getter

		 @return the Sink
		*/
		SRDAGVertex* getSink();

		/**
		 Source setter

		 @param vertex: the source
		*/
		void setSource(SRDAGVertex* vertex);

		/**
		 Sink setter

		 @param vertex: the sink
		*/
		void setSink(SRDAGVertex* vertex);

		// Public for performance sake

		/**
		 In order to fast access the input edges, a linked list of edges is done. The edges
		 can then be scanned in the linked list order to get the input edges corresponding
		 to the vertices in direct order.
		*/
		static SRDAGEdge* firstInSinkOrder;
		SRDAGEdge* prevInSinkOrder;
		SRDAGEdge* nextInSinkOrder;
		static SRDAGEdge* lastInSinkOrder;
};


/**
 Source getter

 @return the source
*/
inline
SRDAGVertex* SRDAGEdge::getSource(){
	return source;
}

/**
 Sink getter

 @return the Sink
*/
inline
SRDAGVertex* SRDAGEdge::getSink(){
	return sink;
}

/**
 Source setter

 @param vertex: the source
*/
inline
void SRDAGEdge::setSource(SRDAGVertex* vertex){
	source = vertex;
}

/**
 Sink setter

 @param vertex: the sink
*/
inline
void SRDAGEdge::setSink(SRDAGVertex* vertex){
	sink = vertex;
}

/**
 TokenRate getter (token rate = production = consumption in SRDAG)

 @return production after resolving the expression
*/
inline
int SRDAGEdge::getTokenRate()
{
	return(this->tokenRate);
}

/**
 TokenRate setter

 @param production: expression defining the token production of the edge source (in char)
 @return production after resolving the expression
*/
inline
void SRDAGEdge::setTokenRate(int rate)
{
	this->tokenRate = rate;
}

#endif

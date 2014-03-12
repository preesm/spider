
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#ifndef SRDAG_EDGE
#define SRDAG_EDGE

//#include "SRDAGVertex.h"
class SRDAGVertex;
class BaseEdge;

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

	protected :
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

		BaseEdge* refEdge;		// Reference to PiSDF edge.

		int delay;				// Delay or number of initial tokens.

		int fifoId;			// Id of the implemented FIFO.

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

		/**
		 delay getter.

		 @return delay
		*/
		int getDelay();

		/**
		 delay setter
		*/
		void setDelay(const int delay);

		// Public for performance sake

	    BaseEdge *getRefEdge() const
	    {
	        return refEdge;
	    }

	    void setRefEdge(BaseEdge *refEdge)
	    {
	        this->refEdge = refEdge;
	    }

		int getFifoId() const {
			return fifoId;
		}

		void setFifoId(int fifoId) {
			this->fifoId = fifoId;
		}

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

/**
 delay getter.
 */
inline int SRDAGEdge::getDelay(){
	return(this->delay);
}

/**
 delay setter.
 */
inline void SRDAGEdge::setDelay(const int delay){
	this->delay = delay;
}

#endif

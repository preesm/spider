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

#ifndef SRDAG_EDGE
#define SRDAG_EDGE

#include <platform_types.h>
#include <grt_definitions.h>

class SRDAGVertexAbstract;
class SRDAGGraph;
class PiSDFEdge;

typedef struct{
	int id;
	UINT32 add;
	UINT32 size;
} FIFO;

/**
 * An edge in a delay-less SRDAG graph (no initial token). 
 * The base unit of production and consumption is a char (8 bits).
 * The space reserved for tokens can be filled with a structure.
 * 
 * @author mpelcat
 */
class SRDAGEdge {
protected :
	int id;
	SRDAGVertexAbstract *source, *sink;
	int sourcePortIx, sinkPortIx;
	SRDAGGraph *graph;

	int delay;				// Delay or number of initial tokens.

	FIFO fifo;

	static int creationIx;

public :
	SRDAGEdge();
	SRDAGEdge(SRDAGGraph *graph);
	~SRDAGEdge();
	void reset();

	UINT32 getTokenRate() const;
	void setTokenRate(UINT32 rate);

	int getDelay() const;
	void setDelay(int delay);

	int getFifoId() const;
	void setFifoId(int fifoId);

	UINT32 getFifoAddress() const;
	void setFifoAddress(UINT32 fifoAddress);

	FIFO* getFifo();

	PiSDFEdge *getRefEdge() const;
	void setRefEdge(PiSDFEdge *refEdge);

	void connectSink(SRDAGVertexAbstract* nSink, int nSinkPortIx);
	void disconnectSink();
	void connectSource(SRDAGVertexAbstract* nSource, int nSourcePortIx);
	void disconnectSource();

	int getId() const;

	int getSinkPortIx() const;
	int getSourcePortIx() const;

	SRDAGVertexAbstract* getSource() const;
	SRDAGVertexAbstract* getSink() const;
};

/**
 Source getter

 @return the source
*/
inline
SRDAGVertexAbstract* SRDAGEdge::getSource() const{
	return source;
}

/**
 Sink getter

 @return the Sink
*/
inline
SRDAGVertexAbstract* SRDAGEdge::getSink() const{
	return sink;
}

/**
 TokenRate getter (token rate = production = consumption in SRDAG)

 @return production after resolving the expression
*/
inline
UINT32 SRDAGEdge::getTokenRate() const {
	return(this->fifo.size);
}

/**
 TokenRate setter

 @param production: expression defining the token production of the edge source (in char)
 @return production after resolving the expression
*/
inline
void SRDAGEdge::setTokenRate(const UINT32 rate){
	this->fifo.size = rate;
}

/**
 delay getter.
 */
inline
int SRDAGEdge::getDelay() const {
	return delay;
}

/**
 delay setter.
 */
inline
void SRDAGEdge::setDelay(const int d){
	delay = d;
}

inline
FIFO* SRDAGEdge::getFifo(){
	return &fifo;
}

inline
int SRDAGEdge::getFifoId() const {
	return fifo.id;
}

inline
void SRDAGEdge::setFifoId(int fifoId) {
	this->fifo.id = fifoId;
}

inline
UINT32 SRDAGEdge::getFifoAddress() const {
	return fifo.add;
}

inline
void SRDAGEdge::setFifoAddress(UINT32 fifoAddress) {
	this->fifo.add = fifoAddress;
}

inline int SRDAGEdge::getId() const {
	return id;
}

inline int SRDAGEdge::getSinkPortIx() const {
	return sinkPortIx;
}

inline int SRDAGEdge::getSourcePortIx() const {
	return sourcePortIx;
}

#endif

/*
 * SRDAGEdgeX.h
 *
 *  Created on: 2 juil. 2013
 *      Author: yoliva
 */

#ifndef SRDAGEDGEX_H_
#define SRDAGEDGEX_H_

class SRDAGEdgeX: public SRDAGEdge{
protected:
	// Delay or number of initial tokens.
	int delay;
public:

	/**
	 delay getter.

	 @return delay
	*/
	int getDelay();

	/**
	 delay setter
	*/
	void setDelay(const int delay);
};

inline int SRDAGEdgeX::getDelay(){
	return(this->delay);
}


inline void SRDAGEdgeX::setDelay(const int delay){
	this->delay = delay;
}

#endif /* SRDAGEDGEX_H_ */

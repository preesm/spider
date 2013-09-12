/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/

#ifndef JOBINSTANCE_H
#define JOBINSTANCE_H

#include "../SRDAG/SRDAGVertex.h"

class JobInstance {

	friend class FlowShopScheduler;

	private :

	/**
	 the type of Job this Job instance represents. holds an index to the Job repository array
	*/
	unsigned int jobType;

	/**
	 the original SRDAG vertex to which the Operations in this Job refer to
	*/
	SRDAGVertex *operations[MAX_CSDAG_VERTICES];

	public :

	/**
	 Constructor
	*/	
	JobInstance();

	/**
	 Destructor
	*/	
	~JobInstance();

	/**
	 Retrieve the Job type of this queued Job instance
	*/	
	unsigned int getJobType();

	/**
	 Set the Job type of this queued Job instance
	*/	
	void setJobType(unsigned int type);

	/**
	 Retrieve the SRDAG reference of an Operation
	*/	
	SRDAGVertex* getOperationReference(int index);

	/**
	 Set the SRDAG reference of an Operation
	*/	
	void addOperationReference(int index, SRDAGVertex* ref);

};

#endif

/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/

#include "JobInstance.h"

/**
 Constructor
*/
JobInstance::JobInstance()
{
}

/**
 Destructor
*/
JobInstance::~JobInstance()
{
}

/**
 Retrieve the Job type of this queued Job instance
*/	
unsigned int JobInstance::getJobType()
{
	return jobType;
}

/**
 Set the Job type of this queued Job instance
*/	
void JobInstance::setJobType(unsigned int type)
{
	jobType = type;
}

/**
 Retrieve the SRDAG reference of an Operation
*/	
SRDAGVertex* JobInstance::getOperationReference(int index)
{
	return operations[index];
}

/**
 Set the SRDAG reference of an Operation
*/	
void JobInstance::addOperationReference(int index, SRDAGVertex* ref)
{
	operations[index] = ref;
}


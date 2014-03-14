/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/

#include "Job.h"
#include <cstdio>
/**
 Constructor
*/
Job::Job ()
{
	opCount = MAX_CSDAG_VERTICES;
	flush();
}

/**
 Destructor
*/
Job::~Job()
{
}

/**
 Get the type of this Job
 
 @return: the Job type
*/
int Job::getType()
{
	return type;
}

/**
 Set the type of this Job
 
 @return: the Job type
*/
void Job::setType(int value)
{
	type = value;
}

/**
 Reset the Job
*/
void Job::flush()
{
	for(int i = 0; i < opCount; i++)
		offset[i] = 0;
	opCount = 0;
	duration = 0;
	hash = 0;
	type = 0;
}

/**
 Setting the duration of this Job
*/
void Job::setDuration(int value)
{
	duration = value;
}

/**
 Add an Operation to this Job
 note: Operations must be added in exec. order

 @param opType: the type of the Operation to be added

 @return the index to the added Operation
 */
char Job::addOperation(int opType)
{
	return opps[opCount++] = opType;
}

/**
 Retrieve a pointer to an Operation

 @return index to the Operation;
*/
char Job::getOperation(int index)
{
	return opps[index];
}

/**
 Compute a unique hash code for this Job
*/
void Job::computeHash()
{
	hash = 5381;

	for(int i = 0; i < opCount; i++)
		hash = ((hash << 5) + hash) + opps[i];
}

/**
 retrieve the unique hash code of this Job
*/
unsigned int Job::getHash()
{
	return hash;
}

/**
 Get Operation count
 
 @return: the Operation count
*/
int Job::getOpCount()
{
	return opCount;
}

/**
 Retrieve the start time offset of this Operation within its Job

 @param index: Operation index
 @return: the offset value
*/
int Job::getOffset(int index)
{
	return offset[index];
}

/**
 Set the start time offset of this Operation within its Job

 @param index: Operation index
 @param value: the new offset value
*/
void Job::setOffset(int index, int value)
{
	offset[index] = value;
}


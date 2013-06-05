/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#include "../SRDAG/SRDAGVertex.h"

#include "Operation.h"
#include <cstdlib>

/**
 Constructor
*/
Operation::Operation(){
	machineSetId = -1;
	flush();
}

/**
 Destructor
*/
Operation::~Operation(){
}

/**
 Retrieve the duration of this Operation

 @return: the duration
*/
int Operation::getDuration()
{
	return duration;
}

/**
 Set the duration of this Operation

 @param value: the new duration of this Operation
*/
void Operation::setDuration(int value)
{
	duration = value;
}

/**
 Retrieve a machine that is able to execute this
 Operation. The machine is identified based on
 on a given index.

 @param index: the index of the machine

 @return: the machine corresponding the index
*/
int Operation::getMachineSetId(){
	return machineSetId;
}

/**
 Add a new machine that is able to execute this
 Operation.

 @param value: the new machine 
*/
void Operation::setMachineSet(int value){
	machineSetId = value;
}

/**
 Reset the Operation
*/
void Operation::flush()
{
	vertex = NULL;
	duration = 0;
	type = -1;
	endTime = 0;
}

/**
 Retrieve the type of this Operation.

 @return: the Operation type
*/
int Operation::getType()
{
	return type;
}

/**
 Set the type of this Operation

 @param value: the Operation type
*/
void Operation::setType(int value)
{
	type = value;
}

/**
 Retrieve the SRDAGVertex associated to
 this Operation. (temporary storage)

 @return: the pointer to the vertex
*/
SRDAGVertex* Operation::getReference()
{
	return vertex;
}

/**
 Set the SRDAGVertex associated to this
 Operation. (temporary storage)

 @param srVertex: the pointer to the vertex
*/
void Operation::setReference(SRDAGVertex *srVertex)
{
	vertex = srVertex;
}

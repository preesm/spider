/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/

#ifndef JOB_H
#define JOB_H

#include "../../SchedulerDimensions.h"
#include "Operation.h"

/**
	the Job class used for "repository" purposes. a Job consists of one or more Operations.
*/

class Job {

	friend class FlowShopScheduler;

	private :

		/**
		 Time offset of Operations with respect to the start time of the Job (fixed value)
		*/
		int offset[MAX_CSDAG_VERTICES];			 

		/**
		 the time duration of this Job (sum of Operation durations)
		*/
		int duration;

		/**
		 hash identifier of this Job
		*/
		unsigned int hash;

		/**
		 the type of this Job
		*/
		unsigned char type;

		/**
		 number of Operations in this Job
		*/
		unsigned char opCount;

		/**
		 indices to Operations
		*/
		unsigned char opps[MAX_CSDAG_VERTICES];

	public :

		/**
		 Constructor
		*/
		Job();

		/**
		 Destructor
		*/
		~Job();

		/**
		 Reset the Job
		*/
		void flush();
			
		/**
		 Setting the duration of this Job
		*/
		void setDuration(int value);

		/**
		 Add an Operation to this Job
		 note: Operations must be added in exec. order

		 @param opType: the type of the Operation to be added

		 @return the index to the added Operation
		 */
		char addOperation(int opType);

		/**
		 Retrieve a pointer to an Operation

		 @param index: the index of the requested Operation

		 @return: pointer to the added Operation;
		*/
		char getOperation(int index);

		/**
		 Get Operation count
		 
		 @return: the Operation count
		*/
		int getOpCount();

		/**
		 Get the type of this Job
		 
		 @return: the Job type
		*/
		int getType();

		/**
		 Set the type of this Job
		 
		 @return: the Job type
		*/
		void setType(int value);

		/**
		 Compute a unique hash code for this Job
		*/
		void computeHash();

		/**
		 retrieve the unique hash code of this Job
		*/
		unsigned int getHash();

		/**
		 Retrieve the start time offset of this Operation within its Job

		 @param index: Operation index
		 @return: the offset value
		*/
		int getOffset(int index);

		/**
		 Set the start time offset of this Operation within its Job

		 @param index: Operation index
		 @param value: the new offset value
		*/
		void setOffset(int index, int value);
};

#endif

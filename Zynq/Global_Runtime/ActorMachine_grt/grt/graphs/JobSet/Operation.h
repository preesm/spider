/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/

#ifndef OPERATION_H
#define OPERATION_H

#include "../../graphs/SRDAG/SRDAGVertex.h"

class Operation {

	friend class FlowShopScheduler;
	friend class ProcessorMapper;

	private :

		/**
		 Type of this Operation; i.e. CSDag function index
		*/
		char type;

		/**
		 The SRDAG reference of this Operation (temporary storage)
		*/
		SRDAGVertex *vertex;

		/**
		 Time duration of this Operation
		*/
		int duration;

		int machineSetId;

		int production;

		int consomation;

		/**
		 The time instant when the last instance of this Operation has ended
		*/
		int endTime;

	public :

		/**
		 Constructor
		*/
		Operation();

		/**
		 Destructor
		*/
		~Operation();

		/**
		 Reset the Operation
		*/
		void flush();

		/**
		 Retrieve the duration of this Operation

		 @return: the duration
		*/
		int getDuration();

		/**
		 Set the duration of this Operation

		 @param value: the new duration of this Operation
		*/
		void setDuration(int value);

		/**
		 Retrieve a machine that is able to execute this
		 Operation. The machine is identified based on
		 on a given index.

		 @param index: the index of the machine

		 @return: the machine corresponding the index
		*/
		int getMachineSetId();


		int getProd(){return production;};


		int getConso(){return consomation;};

		/**
		 Add a new machine that is able to execute this
		 Operation.

		 @param value: the new machine 
		*/
		void setMachineSet(int value);

		/**
		 Retrieve the type of this Operation.

		 @return: the Operation type
		*/
		int getType();

		/**
		 Set the type of this Operation

		 @param value: the Operation type
		*/
		void setType(int value);

		/**
		 Retrieve the SRDAGVertex associated to
		 this Operation. (temporary storage)

		 @return: the pointer to the vertex
		*/
		SRDAGVertex* getReference();

		/**
		 Set the SRDAGVertex associated to this
		 Operation. (temporary storage)

		 @param srVertex: the pointer to the vertex
		*/
		void setReference(SRDAGVertex *srVertex);


		void setProd(int prod){production=prod;};


		void setConso(int conso){consomation=conso;};
};

#endif

/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef ARCHITECTURE
#define ARCHITECTURE

#include "../../SchedulerDimensions.h"
#include "../../tools/SchedulingError.h"

typedef enum{
	Read = 0,
	Write = 1
} ComDirection;

/**
 * The architecture defines the number of slaves and their types
 * 
 * @author mpelcat
 */
class Architecture {

	private :

		/**
		 Slave name for each slave
		*/
		char slaveNames[MAX_SLAVES][MAX_SLAVE_NAME_SIZE];
		
		/**
		 Number of slaves on which we schedule
		*/
		int nbSlaves;

		int nbActiveSlaves;

		/**
		 Slave type for each slave
		*/
		int slaveTypes[MAX_SLAVES];
		
		/**
		 Number of slaves types
		*/
		int nbSlaveTypes;

		/**
		 Datarate in Time Unit/ Memory Units
			t = As+B
			t: time in cycles
			s: size
		*/
		double slaveCom_A[MAX_SLAVES][2];
		double slaveCom_B[MAX_SLAVES][2];
	public : 
		/**
		 Constructor
		*/
		Architecture();


		/**
		 Destructor
		*/
		~Architecture();


		/**
		 Adding a slave of a given type


		 @param type of the slave
		 @param name of the slave
		*/
		void addSlave(int type, const char* name, double readA, double readB, double writeA, double writeB);

		/**
		 Returns the name of a slave

		 @param index of the slave
		 @return the name
		*/
		char* getSlaveName(int index);

		/**
		 Returns the type of a slave

		 @param index of the slave
		 @return the type
		*/
		int getSlaveType(int index);

		/**
		 Returns the number of slaves types in the architecture

		 @return the number of slaves types
		*/
		int getNbSlaveTypes();

		/**
		 Returns the number of slaves in the architecture

		 @return the number of slaves
		*/
		int getNbSlaves();

		/**
		 Gets the communication rate between two slaves in ns/64Bytes

		 @param senderId: index of the render slave
		 @param senderId: index of the receiver slave

		 @return datarate in ns/64Bytes
		*/
		unsigned int getTimeCom(int senderId, ComDirection dir, unsigned int size);

		int getNbActiveSlaves();
		void setNbActiveSlaves(int nbActiveSlave);
};

/**
 Returns the name of a slave

 @param index of the slave
*/
inline int Architecture::getSlaveType(int index){
	return slaveTypes[index];
}

/**
 Returns the number of slaves in the architecture

 @return the number of slaves
*/
inline int Architecture::getNbSlaves(){
	return nbSlaves;
}

/**
 Returns the number of slaves types in the architecture

 @return the number of slaves types
*/
inline int Architecture::getNbSlaveTypes(){
	return nbSlaveTypes;
}

/**
 Gets the communication rate between two slaves in ns/64Bytes

 @param senderId: index of the render slave
 @param receiverId: index of the receiver slave

 @return datarate in ns/64Bytes
*/
inline unsigned int Architecture::getTimeCom(int senderId, ComDirection dir, unsigned int size){
	return slaveCom_A[senderId][dir]*size+slaveCom_B[senderId][dir];
}

/**
 Returns the name of a slave

 @param index of the slave
*/
inline char* Architecture::getSlaveName(int index){
	return slaveNames[index];
}

#endif

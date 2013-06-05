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

		/**
		 Slave type for each slave
		*/
		int slaveTypes[MAX_SLAVES];
		
		/**
		 Number of slaves types
		*/
		int nbSlaveTypes;

		/**
		 Datarate in Time Unit/64 Memory Units (usually ns/ 64bytes). 
		 slaveComRates[0][1] = 70 => slaveComRates[0][1] = 1.09 cycles/byte
		*/
		unsigned short slaveComRates[MAX_SLAVES][MAX_SLAVES];
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
		void addSlave(int type, const char* name);

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
		 Sets the communication rate between two slaves in kbit/s

		 @param senderId: index of the render slave
		 @param senderId: index of the receiver slave
		 @param senderId: datarate in kbit/s (negative values mean infinite data rate: no transfer cost)
		*/
		void setComRate(int senderId, int receiverId, int rate);

		/**
		 Gets the communication rate between two slaves in ns/64Bytes

		 @param senderId: index of the render slave
		 @param senderId: index of the receiver slave

		 @return datarate in ns/64Bytes
		*/
		unsigned short getComRate(int senderId, int receiverId);

		/**
		 Removes the last slave of the architecture (if any)
		*/
		void removeLastSlave();
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
 Sets the communication rate between two slaves in kbit/s

 @param senderId: index of the render slave
 @param receiverId: index of the receiver slave
 @param rate: datarate in kbit/s (negative values mean infinite data rate: no transfer cost)
*/
inline void Architecture::setComRate(int senderId, int receiverId, int rate){

	if(senderId > MAX_SLAVES || receiverId > MAX_SLAVES){	
		// Accessing a slave with an incorrect index
		exitWithCode(1041);
	}

	if(rate == 0 || rate > 1000000000){
	// Setting an improper data rate
		exitWithCode(1042);
	}

	// Special case: negative values mean infinite data rate: no transfer cost
	if(rate < 0){
		slaveComRates[senderId][receiverId] = 0;
	}
	else{
		unsigned int modifiedRate = 1000000000/rate; // converting in ns/kBit
		modifiedRate = (modifiedRate * 8 * 64) / 1000; // converting in ns/64Bytes

		if(modifiedRate >= (1<<16)){
			// The data rate does not fit in table dynamic. Too low rate.
			exitWithCode(1044);
		}
		slaveComRates[senderId][receiverId] = modifiedRate;
	}
}

/**
 Gets the communication rate between two slaves in ns/64Bytes

 @param senderId: index of the render slave
 @param receiverId: index of the receiver slave

 @return datarate in ns/64Bytes
*/
inline unsigned short Architecture::getComRate(int senderId, int receiverId){
	return slaveComRates[senderId][receiverId];
}

/**
 Removes the last slave of the architecture (if any)
*/
inline void Architecture::removeLastSlave(){

	if(nbSlaves == 0){
		// Removing a slave from an empty architecture
		exitWithCode(1043);
	}

	nbSlaves--;
}

#endif

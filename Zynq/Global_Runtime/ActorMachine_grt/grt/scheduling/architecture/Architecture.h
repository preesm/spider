
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#ifndef ARCHITECTURE
#define ARCHITECTURE

#include <grt_definitions.h>
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

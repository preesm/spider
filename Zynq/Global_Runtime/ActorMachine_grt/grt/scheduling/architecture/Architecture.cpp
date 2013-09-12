/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 * The architecture defines the number of slaves and their types
 * 
 * @author mpelcat
 */
#include "Architecture.h"
#include "../../tools/SchedulingError.h"

#include <cstdio>
#include <cstring>

/**
 Constructor

 @param nbSlaves: current number of slaves
*/
Architecture::Architecture()
{
	nbSlaves = 0;
	nbActiveSlaves = 0;
	nbSlaveTypes = 0;
}

/**
 Destructor
*/
Architecture::~Architecture()
{
}

/**
 Adding a slave of a given type

 @param type of the slave
 @param name of the slave
*/
void Architecture::addSlave(int type, const char* name, double readA, double readB, double writeA, double writeB){
	if(nbSlaves>=MAX_SLAVES){
		// Adding a slave when the slave table is full
		exitWithCode(1008);
	}
	else if(type>=MAX_SLAVE_TYPES){
		// Adding a slave type with a too big index
		exitWithCode(1009);
	}
	else{
		slaveTypes[nbSlaves] = type;
		nbSlaveTypes = (nbSlaveTypes < type)? type+1:nbSlaveTypes;
		strcpy(slaveNames[nbSlaves],name);
		slaveCom_A[nbSlaves][Read] = readA;
		slaveCom_B[nbSlaves][Read] = readB;
		slaveCom_A[nbSlaves][Write] = writeA;
		slaveCom_B[nbSlaves][Write] = writeB;
		nbSlaves++;
	}
}

int Architecture::getNbActiveSlaves() {
	return nbActiveSlaves;
}

void Architecture::setNbActiveSlaves(int nbas) {
	if(nbas>nbSlaves){
		// Adding a slave when the slave table is full
		exitWithCode(1053);
	}
	nbActiveSlaves = nbas;
}

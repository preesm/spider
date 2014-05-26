/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

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

void Architecture::reset(){
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


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

#ifndef AMACTION_H_
#define AMACTION_H_

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "grt_definitions.h"
#include "launcher/launcher.h"



class AMAction {
private:
	int functionID;
	int nbInFifos;
	int nbOutFifos;
	LauncherFIFO* inFIFOs[MAX_NB_FIFO];
	LauncherFIFO* outFIFOs[MAX_NB_FIFO];
//	int fifoIn[MAX_SRDAG_INPUT_EDGES];
//	int fifoOut[MAX_SRDAG_OUTPUT_EDGES];
//	int fifoInSize[MAX_SRDAG_INPUT_EDGES];
//	int fifoOutSize[MAX_SRDAG_OUTPUT_EDGES];
//	int fifoInAddr[MAX_SRDAG_INPUT_EDGES];
//	int fifoOutAddr[MAX_SRDAG_OUTPUT_EDGES];
	int nbArgs;
	int args[MAX_VARIABLE_NUMBER];
	char name[AM_ACTION_NAME];

public:
	AMAction(){
		functionID=-1;
		nbInFifos=0;
		nbOutFifos=0;
		nbArgs=0;
	}

	int getFunctionId(){return functionID;}
	void setFunctionId(int functionId){functionID = functionId;}


	int getNbArgs(){return nbArgs;}
	int getArg(int id){return args[id];}
	void addArg(int value){
		if(nbArgs>=MAX_VARIABLE_NUMBER){
			printf("nbArgs>=MAX_VARIABLE_NUMBER\n");
			abort();
		}
		args[nbArgs++] = value;
	}

	int getNbInFifos(){return nbInFifos;}
	void setInFifo(LauncherFIFO* newFifo){
		inFIFOs[nbInFifos++] = newFifo;
	}
	LauncherFIFO* getInFifo(int i){return inFIFOs[i];}
//	int getFifoInSize(int id){return fifoInSize[id];}
//	int getFifoInAddr(int id){return fifoInAddr[id];}
//	void addFifoIn(int Id, UINT32 size, UINT32 addr){
//		if(nbFifoIn>=MAX_SRDAG_INPUT_EDGES){
//			printf("nbFifoIn>=MAX_SRDAG_INPUT_EDGES\n");
//			abort();
//		}
//		fifoIn[nbFifoIn] = Id;
//		fifoInSize[nbFifoIn] = size;
//		fifoInAddr[nbFifoIn++] = addr;
//	}

	int getNbOutFifos(){return nbOutFifos;}
	void setOutFifo(LauncherFIFO* newFifo){
		outFIFOs[nbOutFifos++] = newFifo;
	}
	LauncherFIFO* getOutFifo(int i){return outFIFOs[i];}
//	int getFifoOut(int id){return fifoOut[id];}
//	int getFifoOutSize(int id){return fifoOutSize[id];}
//	int getFifoOutAddr(int id){return fifoOutAddr[id];}
//	void addFifoOut(int Id, UINT32 size, UINT32 addr){
//		if(nbFifoOut>=MAX_SRDAG_OUTPUT_EDGES){
//			printf("nbFifoOut>=MAX_SRDAG_OUTPUT_EDGES\n");
//			abort();
//		}
//		fifoOut[nbFifoOut] = Id;
//		fifoOutSize[nbFifoOut] = size;
//		fifoOutAddr[nbFifoOut++] = addr;
//	}

	void setName(const char* _name){strncpy(name, _name, AM_ACTION_NAME);}
	char* getName(){return name;}

};

#endif /* AMACTION_H_ */

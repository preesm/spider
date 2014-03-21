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

#ifndef AMVERTEX_H_
#define AMVERTEX_H_

#include "AMCond.h"
#include <grt_definitions.h>

#include <stdio.h>
#include <stdlib.h>

#include <debuggingOptions.h>
#if USE_AM

typedef enum{
	AMVxTypeUndefined=0,
	AMVxTypeWait,
	AMVxTypeState,
	AMVxTypeTest,
	AMVxTypeExec
} AMVertexType;

typedef enum{
	COND_false	= '0',
	COND_TRUE	= '1',
	COND_X 		= 'X'
} CondValue;

class AMVertex {
private:
	int ID;
	AMVertexType type;

	int nbSuc;
	int successorsID[AM_VERTEX_MAX_SUCCESSOR];

	union{
		/* ExecVertex specific attributes */
		int actionID;

		/* CondVertex specific attributes */
		int condID;

		/* WaitVertex specific attributes */

		/* StateVertex specific attributes */
		struct{
			CondValue condValue[AM_GRAPH_MAX_COND];
			int nbConds;
		} state;
	};
public:
	AMVertex(){
		ID = -1;
		type = AMVxTypeUndefined;
		nbSuc = 0;
	}

	/* ExecVertex Constructor */
	/* CondVertex Constructor */
	/* WaitVertex Constructor */
	AMVertex(int id, AMVertexType t, int param=0){
		ID = id;
		type = t;
		nbSuc = 0;
		switch(type){
		case AMVxTypeWait:
			break;

		case AMVxTypeTest:
			condID = param;
			break;

		case AMVxTypeExec:
			actionID = param;
			break;

		case AMVxTypeState:
			break;
		default:
			//todo error
			break;
		}
	}

	/* StateVertex Constructor */
	AMVertex(int id, AMVertexType t, CondValue* _condValue, int _nbConds){
		if(t != AMVxTypeState){
			// todo error
		}else{
			type = AMVxTypeState;
			ID = id;
			nbSuc = 0;
			state.nbConds = 0;//_nbConds;
//			for(int i=0; i<state.nbConds; i++){
//				state.condValue[i] = _condValue[i];
//			}
//			for(int i=state.nbConds; i<AM_GRAPH_MAX_COND; i++){
//				state.condValue[i] = COND_X;
//			}
		}
	}

	AMVertexType getType(){ return type;};
	int getId(){ return ID;};

	int getNbSuc(){ return nbSuc;}
	int getSucID(int ix){ return successorsID[ix];}

	void addSuc(int suc){
		if(nbSuc>=AM_VERTEX_MAX_SUCCESSOR){
			printf("AMVertex: nbSuc > AM_VERTEX_MAX_SUCCESSOR\n");
			abort();
		}
		successorsID[nbSuc++] = suc;
	}

	/* ExecVertex specific functions */
	int getAction(){
		//todo error
		return actionID;
	}

	/* CondVertex specific functions */
	int getCondID(){
		//todo error
		return condID;
	}
	void setNbConds(int n){
		//todo error
		state.nbConds = n;
	}

	/* WaitVertex specific functions */

	/* StateVertex specific functions */
	void printStateVertex(char* vals){
		for(int i=0; i<state.nbConds; i++){
			vals[i] = state.condValue[i];
		}
		vals[state.nbConds]='\0';
	}
};

#endif

#endif /* AMVERTEX_H_ */

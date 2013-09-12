/*
 * AMVertex.h
 *
 *  Created on: Jun 17, 2013
 *      Author: jheulot
 */

#ifndef AMVERTEX_H_
#define AMVERTEX_H_

#include "AMCond.h"
#include "../../SchedulerDimensions.h"

#include <cstdio>
#include <cstdlib>

typedef enum{
	VERTEX_UNINITIALIZED=0,
	WAIT,
	STATE,
	TEST,
	EXEC
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
		type = VERTEX_UNINITIALIZED;
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
		case WAIT:
			break;

		case TEST:
			condID = param;
			break;

		case EXEC:
			actionID = param;
			break;

		case STATE:
			break;
		default:
			//todo error
			break;
		}
	}

	/* StateVertex Constructor */
	AMVertex(int id, AMVertexType t, CondValue* _condValue, int _nbConds){
		if(t != STATE){
			// todo error
		}else{
			type = STATE;
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

#endif /* AMVERTEX_H_ */

/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef CSDAG_VERTEX
#define CSDAG_VERTEX

class CSDAGGraph;
class CSDAGEdge;
#include <cstring>

#include <grt_definitions.h>
#include "../../scheduling/architecture/Architecture.h"
#include "../../expressionParser/XParser.h"

/**
 * A vertex in a DAG graph
 * 
 * @author mpelcat
 */

class CSDAGVertex {

	private :
		/**
		 The base, i.e. the graph in which current vertex is included
		*/
		CSDAGGraph* base;

		/**
		 The vertex name
		*/
		char name[MAX_VERTEX_NAME_SIZE];

		/**
		 Expressions defining for each type of slave the DAET (Deterministic Actor Execution Time) in ns.
		*/
		char charTimings[MAX_SLAVE_TYPES][MAX_VERTEX_NAME_SIZE];
		
		/**
		 Integer defining for each type of slave the DAET (Deterministic Actor Execution Time) in ns.
		 Resolved by the resolveTimings() method from the char timings
		*/
		int intTimings[MAX_SLAVE_TYPES];
		
		/**
		 boolean value saved as char defining if a vertex can be executed by a slave.
		*/
		char constraints[MAX_SLAVES];

		/**
		 Actor function pointer index in the table named "functionPointers".
		 Used to call the actual actor code
		*/
		short functionIndex; 

		/**
		 Number of parameters
		*/
		int paramNb;

		/**
		 Pointers on the parameter patterns stored in the graph. One pattern for each parameter
		*/
		abstract_syntax_elt params[MAX_PARAM][REVERSE_POLISH_STACK_MAX_ELEMENTS];
		abstract_syntax_elt* paramPatterns[MAX_PARAM];

		/**
		 Integer solved parameters. Retrieved while solving the edges
		*/
		int paramValues[MAX_PARAM];
		int paramsValues[MAX_CSDAG_VERTEX_REPETITION][MAX_PARAM];

		/**
		 Number of repetitions of the vertex in the current SRDAG.
		*/
		unsigned char repetitionNb;

		CSDAGEdge *inputEdges[MAX_CSDAG_INPUT_EDGES];
		int nbInputEdges;

	public : 

		/**
		 Constructor
		*/
		CSDAGVertex();

		/**
		 Destructor
		*/
		~CSDAGVertex();

		/**
		 Setting the base, i.e. the graph in which current vertex is included

		 @param base: the base
		*/
		void setBase(CSDAGGraph* graph);

		/**
		 Setting the vertex name

		 @param name: the name
		*/
		void setName(const char* name);

		/**
		 Getting the vertex name

		 @param name: the name
		*/
		char* getName();

		/**
		 Setting the function index: the function index references the C function
		 corresponding to the current vertex. THis function is retrieved in a global
		 table using the index

		 @param index: the function index
		*/
		void setFunctionIndex(int index);

		/**
		 Getting the function index: the function index references the C function
		 corresponding to the current vertex. This function is retrieved in a global
		 table using the index

		 @return: the function index
		*/
		int getFunctionIndex();

		/**
		 Getting the number of parameters stored for the current vertex

		 @return the number of parameters
		*/
		int getParamNb();

		/**
		 Getting the pattern corresponding to the parameter with given index

		 @param index: the parameter index
		 @return an abstract_syntax_elt pattern in reverse polish notation
		*/
		abstract_syntax_elt* getParamPattern(int index);

		/**
		 Getting the integer timing corresponding to a slave type

		 @param slaveTypeIndex: the slave type index
		 @return a timing
		*/
		int getIntTiming(int slaveTypeIndex);

		/**
		 Getting the value of a parameter considering a repetition index of the csDag vertex.

		 @param srDagRepetitionIndex: the index of the SRDAG vertex among the CSDAG vertex repetitions
		 @param paramIndex: the parameter index
		 @return the parameter value
		*/
		int getParamValue(int srDagRepetitionIndex, int paramIndex);
		int getParamValue(int paramIndex){return paramValues[paramIndex];}

		/**
		 Getting the constraint corresponding to a given slave :
		 1 if the vertex can be executed on the slave. 0 otherwise.

		 @param slaveIndex: the index of slave
		 @return the constraint
		*/
		char getConstraint(int slaveIndex);

		/**
		 Setting the value of a parameter considering a repetition index of the csDag vertex.

		 @param srDagRepetitionIndex: the index of the SRDAG vertex among the CSDAG vertex repetitions
		 @param paramIndex: the parameter index
		 @param value: the parameter value
		*/
		void setParamValue(int srDagRepetitionIndex, int paramIndex, int value);

		/**
		 Adding an expression defining a timing

		 @param slaveTypeId: the type of slave corresponding to the current DAET
		 @param timingExpression: the expression defining the timing
		*/
		void addTiming(int slaveTypeId, const char* timingExpression);

		/**
		 Adding a boolean defining a constraint

		 @param slaveId: id of the tested slave
		 @param constraint: the boolean defining if the vertex can be executed or not on the slave
		*/
		void addConstraint(int slaveId, char constraint);

		/**
		 Sets the same constraint for all slaves

		 @param constraint: the boolean defining if the vertex can be executed or not on the slaves
		 @param nbSlaves: number of initialized slaves
		*/
		void addAllConstraints(char constraint, int nbSlaves);

		/**
		 Resolves the char expressions of timings for the first nbSlaves slaves

		 @param archi the current architecture
		*/
		void resolveTimings(Architecture* archi);

		void resolveArgs();

		/**
		 Adds a pattern of parameter assignment or values in the form {712,titi=tutu,tata}
		 where tutu and tata must be previously evaluated variables

		 @param pattern added pattern
		*/
		void addParam(const char* param);
		void addParamPattern(const char* pattern);

		/**
		 Setting the instantaneous repetition number of the 
		 CSDAG vertex in the SRDAG graph.

		 @param repNum: the repetition number
		*/
		void setRepetitionNb(int repetitions);

		/**
		 Getting the instantaneous repetition number of the 
		 CSDAG vertex in the SRDAG graph.

		 @return the repetition number
		*/
		int getRepetitionNb();
		void addInputEdge(CSDAGEdge* edge);
		int getNbInputEdge();
		CSDAGEdge* getInputEdge(int id);
};


/**
 Setting the base, i.e. the graph in which current vertex is included

 @param base: the base
*/
inline
void CSDAGVertex::setBase(CSDAGGraph* graph){
	this->base = graph;
}
inline
void CSDAGVertex::addInputEdge(CSDAGEdge* edge){
	if(nbInputEdges > MAX_CSDAG_INPUT_EDGES){
		exitWithCode(-1);
	}
	inputEdges[nbInputEdges++] = edge;
}inline
int CSDAGVertex::getNbInputEdge(){
	return nbInputEdges;
}

inline
CSDAGEdge* CSDAGVertex::getInputEdge(int id){
	return inputEdges[id];
}

/**
 Setting the vertex name

 @param name: the name
*/
inline
void CSDAGVertex::setName(const char* name){
	strcpy(this->name,name);
}

/**
 Getting the vertex name

 @param name: the name
*/
inline
char* CSDAGVertex::getName(){
	return name;
}

/**
 Setting the function index: the function index references the C function
 corresponding to the current vertex. This function is retrieved in a global
 table using the index

 @param index: the function index
*/
inline
void CSDAGVertex::setFunctionIndex(int index){
	functionIndex = index;
}

/**
 Getting the function index: the function index references the C function
 corresponding to the current vertex. This function is retrieved in a global
 table using the index

 @return: the function index
*/
inline
int CSDAGVertex::getFunctionIndex(){
	return functionIndex;
}

/**
 Getting the number of parameters stored for the current vertex

 @return the number of parameters
*/
inline
int CSDAGVertex::getParamNb(){
	return paramNb;
}

/**
 Getting the pattern corresponding to the parameter with given index

 @param index: the parameter index
 @return an abstract_syntax_elt pattern in reverse polish notation
*/
inline
abstract_syntax_elt* CSDAGVertex::getParamPattern(int index){
	return paramPatterns[index];
}

/**
 Getting the integer timing corresponding to a slave type

 @param slaveTypeIndex: the slave type index
 @return a timing
*/
inline
int CSDAGVertex::getIntTiming(int slaveTypeIndex){
	return intTimings[slaveTypeIndex];
}

/**
 Getting the value of a parameter considering a repetition index of the csDag vertex.

 @param srDagRepetitionIndex: the index of the SRDAG vertex among the CSDAG vertex repetitions
 @param paramIndex: the parameter index
 @return the parameter value
*/
inline
int CSDAGVertex::getParamValue(int srDagRepetitionIndex, int paramIndex){
	return paramsValues[srDagRepetitionIndex][paramIndex];
}

/**
 Setting the value of a parameter considering a repetition index of the csDag vertex.

 @param srDagRepetitionIndex: the index of the SRDAG vertex among the CSDAG vertex repetitions
 @param paramIndex: the parameter index
 @param value: the parameter value
*/
inline
void CSDAGVertex::setParamValue(int srDagRepetitionIndex, int paramIndex, int value){
	paramsValues[srDagRepetitionIndex][paramIndex] = value;
}

/**
 Getting the constraint correxponding to a given slave: 
 1 if the vertex can be executed on the slave. 0 otherwise.

 @param slaveIndex: the index of slave
 @return the constraint
*/
inline
char CSDAGVertex::getConstraint(int slaveIndex){
	return constraints[slaveIndex];
}

/**
 Setting the instantaneous repetition number of the 
 CSDAG vertex in the SRDAG graph.

 @param repNum: the repetition number
*/
inline
void CSDAGVertex::setRepetitionNb(int repetitions){
	repetitionNb = repetitions;
}

/**
 Getting the instantaneous repetition number of the 
 CSDAG vertex in the SRDAG graph.

 @return the repetition number
*/
inline
int CSDAGVertex::getRepetitionNb(){
	return repetitionNb;
}

#endif

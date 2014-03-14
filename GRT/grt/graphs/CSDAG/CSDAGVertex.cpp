/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#include "CSDAGVertex.h"
#include "CSDAGGraph.h"
#include "../../expressionParser/XParser.h"
#include "../../tools/SchedulingError.h"

/**
 Constructor
*/
CSDAGVertex::CSDAGVertex(){
	functionIndex = -1;
	repetitionNb = -1;
	base=NULL;
	strcpy(this->name,"undefined");
	paramNb = 0;
}

/**
 Destructor
*/
CSDAGVertex::~CSDAGVertex()
{
}

/**
 Adding an expression defining a timing (timings in ns)

 @param slaveTypeId: the type of slave corresponding to the current DAET
 @param timingExpression: the expression defining the timing
*/
void CSDAGVertex::addTiming(int slaveTypeId, const char* timingExpression){
	strcpy(charTimings[slaveTypeId],timingExpression);
}

/**
 Adding a boolean defining a constraint

 @param slaveId: id of the tested slave
 @param constraint: the boolean defining if the vertex can be executed or not on the slave
*/
void CSDAGVertex::addConstraint(int slaveId, char constraint){
	constraints[slaveId] = constraint;
}

/**
 Sets the same constraint for all slaves

 @param constraint: the boolean defining if the vertex can be executed or not on the slaves
 @param nbSlaves: number of initialized slaves
*/
void CSDAGVertex::addAllConstraints(char constraint, int nbSlaves){
	for(int i=0; i<nbSlaves; i++){
		addConstraint(i, constraint);
	}
}

/**
 Resolves the char expressions of timings for the first nbSlaves slaves (timings in ns)

 @param archi the current architecture
*/
void CSDAGVertex::resolveTimings(Architecture* archi){
	for(int i=0; i<archi->getNbSlaves(); i++){
		int type = archi->getSlaveType(i);
		intTimings[type] = globalParser.parse(charTimings[type]);
		
		if(intTimings[type]<0){
			// Unknown timing
			exitWithCode(1024);
		}
	}
}

void CSDAGVertex::resolveArgs(){
	for(int i=0; i<paramNb; i++){
		globalParser.interpret(params[i], &paramValues[i]);
	}
}

/**
 Adds a pattern of parameter assignment or values in the form {712,titi=tutu,tata}
 where tutu and tata must be previously evaluated variables

 @param pattern added pattern
*/
void CSDAGVertex::addParam(const char* param){

	if(paramNb > MAX_PARAM){
		// Adding a parameter in a full table
		exitWithCode(1040);
	}

	// Allocating the pattern in the base graph
//	this->paramPatterns[paramNb] = (abstract_syntax_elt *)(&base->patternsTable[base->patternsTableSize]);

	// Parsing the pattern
	globalParser.parse(param,this->params[paramNb]);

//	base->patternsTableSize += patternSize;

//	if(base->patternsTableSize > MAX_CSDAG_PATTERN_TABLE_SIZE){
//		// Adding a parameter pattern in a full table
//		exitWithCode(1039);
//	}
//	if(patternSize > 1){
//		exitWithCode(1052);
//	}
	paramNb++;
}


/**
 Adds a pattern of parameter assignment or values in the form {712,titi=tutu,tata}
 where tutu and tata must be previously evaluated variables

 @param pattern added pattern
*/
void CSDAGVertex::addParamPattern(const char* pattern){
	
	if(paramNb > MAX_PARAM){
		// Adding a parameter in a full table
		exitWithCode(1040);
	}

	// Allocating the pattern in the base graph
	this->paramPatterns[paramNb] = (abstract_syntax_elt *)(&base->patternsTable[base->patternsTableSize]);

	// Parsing the pattern
	int patternSize = globalParser.parsePattern(pattern,this->paramPatterns[paramNb]);

	base->patternsTableSize += patternSize;

	if(base->patternsTableSize > MAX_CSDAG_PATTERN_TABLE_SIZE){
		// Adding a parameter pattern in a full table
		exitWithCode(1039);
	}
	paramNb++;
}




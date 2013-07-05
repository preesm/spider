/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#include "SRDAGVertex.h"
#include <cstdio>
#include <cstring>

/**
 Constructor
*/
SRDAGVertex::SRDAGVertex(){
	base=0;
	visited=0;
	csDagReference=NULL;
	referenceIndex=-1;
	tLevel=-1;
	nbInputEdges=0;
	nbOutputEdges=0;
	slaveIndex=-1;
	type = 0; // Normal type by default.
}

/**
 Destructor
*/
SRDAGVertex::~SRDAGVertex()
{
}

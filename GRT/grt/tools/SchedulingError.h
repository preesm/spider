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

#ifndef SCHEDULING_ERROR
#define SCHEDULING_ERROR

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/**
 * Handling exit code when something is going wrong in the scheduler
 * 
 * @author mpelcat
 */

#define MAX_ERROR_COUNT 100

static const char* errorString[MAX_ERROR_COUNT] = {
	/* 2000*/ "Name too large",
	/* 2001*/ "Impossible to create new element",
	/* 2002*/ "Removing element not present",
	/* 2003*/ "Index out of bound",
	/* 2004*/ "Access not set element",
	/* 2005*/ "Set already set element",
	/* 2006*/ "Reset not set element",

//	/* 0*/ "Adding a vertex while the graph is already full",
//	/* 1*/ "Adding an edge while the graph is already full",
//	/* 2*/ "The edge source repetition number outsizes the maximal pattern size",
//	/* 3*/ "Two input edges set different repetition numbers to their common sink",
//	/* 4*/ "The edge sink repetition number outsizes the maximal pattern size",
//	/* 5*/ "The CSDAG to SRDAG transformation led to an unbalanced number of produced and consumed token for an edge",
//	/* 6*/ "SRDAG edge creation led to an unbalanced number of produced and consumed token for an edge",
//	/* 7*/ "Removing an edge from an empty graph",
//	/* 8*/ "Adding a slave when the slave table is full",
//	/* 9*/ "Adding a slave type with a too big index",
//	/*10*/ "Adding a production pattern in a full table",
//	/*11*/ "Adding a consumption pattern in a full table",
//	/*12*/ "Adding a variable in an already full variable table",
//	/*13*/ "Too long expression",
//	/*14*/ "Early end of expression after delimiter",
//	/*15*/ "User entered a not existing operator like \"//\"",
//	/*16*/ "Garbage at the end of the expression ",
//	/*17*/ "Unwanted character in expression",
//	/*18*/ "",
//	/*19*/ "Unexpected end of expression",
//	/*20*/ "Expression syntax error",
//	/*21*/ "Operator is not recognized",
//	/*22*/ "Operator is not recognized",
//	/*23*/ "Unknown variable",
//	/*24*/ "Unknown timing",
//	/*25*/ "Trying to allocate a buffer on a full memory",
//	/*26*/ "Adding an expression element in a full stack",
//	/*27*/ "Searching a non existing variable while creating a stack",
//	/*28*/ "Unrecognized token",
//	/*29*/ "Adding an expression element in a full queue",
//	/*30*/ "Removing an expression element from an empty queue",
//	/*31*/ "Popping an expression element from an empty stack",
//	/*32*/ "Parser parenthesis mismatch",
//	/*33*/ "Parser parenthesis mismatch. Left parenthesis missing",
//	/*34*/ "",
//	/*35*/ "Peeking an expression element from an empty stack",
//	/*36*/ "Failure in RPN parsing",
//	/*37*/ "Expression syntax error: wrong variable assignment",
//	/*38*/ "Problem with variable assignment",
//	/*39*/ "Adding a parameter pattern in a full table",
//	/*40*/ "Adding a parameter in a full table",
//	/*41*/ "Incorrect index",
//	/*42*/ "Setting an improper data rate",
//	/*43*/ "Removing a slave from an empty architecture",
//	/*44*/ "The data rate does not fit in table dynamic. Too low rate.",
//	/*45*/ "Error in xmlParser",
//	/*46*/ "Error in Timer",
//	/*47*/ "Too many Input for a Vertex",
//	/*48*/ "Too many Output for a Vertex",
//	/*49*/ "Schedule Check Failed",
//	/*50*/ "Production/Consumption Pattern too long",
//	/*51*/ "Parameter Pattern too long",
//	/*52*/ "Parameters cannot have a pattern",
//	/*53*/ "Try to activate more slaves that the Architecture got",
//	/*54*/ "No more available subgraphs.",
//	/*55*/ "Too many parameters.",
//	/*56*/ "Root vertex not set.",
//	/*57*/ "Too many parameters.",
//	/*58*/ "Too many actor machines.",
//	/*59*/ "Too many actors.",
//	/*60*/ "Too many FIFOs.",
//	/*61*/ "Graph already executed.",
//	/*62*/ "No executable vertices found.",
//	/*63*/ "Multiple SrDAG vertices reference a single PiSDF configure vertex.",
//	/*64*/ "Matching not found for unplugged vertex when merging SrDAG.",
//	/*65*/ "Vertex was deleted.",
//	/*66*/ "Invalid production/consumption value.",
//	/*67*/ "Unexpected amount of data received.",
//	/*68*/ "Unexpected data values received.",
//	/*69*/ "Top's subgraph is missing.",
//	/*70*/ "Top's root vertex is missing.",
//	/*71*/ "Local SDF graph was empty.",
//	/*72*/ "MAX_FILE_NAME_SIZE too small.",
//	/*73*/ "MAX_SLAVE_NAME_SIZE too small.",
//	/*74*/ "EXPR_LEN_MAX too small.",
//	/*75*/ "MAX_VERTEX_NAME_SIZE too small.",
//	/*76*/ "List too small",
};

/**
 Exits returning a code representing the current error

 @param exitCode: code representing the current error
*/
static void exitWithCode(int exitCode, const char* opt = ""){
	printf("\nExiting with code %d:\n", exitCode);
	if(exitCode>=2000 && exitCode<2000+MAX_ERROR_COUNT)
		printf("[%s] %s\n", opt, errorString[exitCode-2000]);
	exit(exitCode);
}

#endif

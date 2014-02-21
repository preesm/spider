
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

/**
 Exits returning a code representing the current error

 @param exitCode: code representing the current error
*/
static void exitWithCode(int exitCode){

	// ExitCodes:

	// 1000: Adding a vertex while the graph is already full
	// 1001: Adding an edge while the graph is already full
	// 1002: The edge source repetition number outsizes the maximal pattern size
	// 1003: Two input edges set different repetition numbers to their common sink
	// 1004: The edge sink repetition number outsizes the maximal pattern size
	// 1005: The CSDAG to SRDAG transformation led to an unbalanced number of produced and consumed token for an edge
	// 1006: SRDAG edge creation led to an unbalanced number of produced and consumed token for an edge
	// 1007: Removing an edge from an empty graph
	// 1008: Adding a slave when the slave table is full
	// 1009: Adding a slave type with a too big index
	// 1010: Adding a production pattern in a full table
	// 1011: Adding a consumption pattern in a full table
	// 1012: Adding a variable in an already full variable table
	// 1013: Too long expression
	// 1014: Early end of expression after delimiter
	// 1015: User entered a not existing operator like "//"
	// 1016: Garbage at the end of the expression 
	// 1018: Unwanted character in expression
	// 1019: Unexpected end of expression
	// 1020: Expression syntax error
	// 1021: Operator is not recognized
	// 1022: Operator is not recognized
	// 1023: Unknown variable
	// 1024: Unknown timing
	// 1025: Trying to allocate a buffer on a full memory
	// 1026: Adding an expression element in a full stack
	// 1027: Searching a non existing variable while creating a stack
	// 1028: Unrecognized token
	// 1029: Adding an expression element in a full queue
	// 1030: Removing an expression element from an empty queue
	// 1031: Popping an expression element from an empty stack
	// 1032: Parser parenthesis mismatch
	// 1033: Parser parenthesis mismatch. Left parenthesis missing
	// 1035: Peeking an expression element from an empty stack
	// 1036: Failure in RPN parsing
	// 1037: Expression syntax error: wrong variable assignment
	// 1038: Problem with variable assignment
	// 1039: Adding a parameter pattern in a full table
	// 1040: Adding a parameter in a full table
	// 1041: Incorrect index
	// 1042: Setting an improper data rate
	// 1043: Removing a slave from an empty architecture
	// 1044: The data rate does not fit in table dynamic. Too low rate.
	// 1045: Error in xmlParser
	// 1046: Error in Timer
	// 1047: Too many Input for a Vertex
	// 1048: Too many Output for a Vertex
	// 1049: Schedule Check Failed
	// 1050: Production/Consumption Pattern too long
	// 1051: Parameter Pattern too long
	// 1052: Parameters cannot have a pattern
	// 1053: Try to activate more slaves that the Architecture got
	// 1054: No more available subgraphs.
	// 1055: Too many parameters.
	// 1056: Root vertex not set.
	// 1057: Too many parameters.
	// 1058: Too many actor machines.
	// 1059: Too many actors.
	// 1060: Too many FIFOs.
	// 1061: Graph already executed.
	// 1062: No executable vertices found.
	// 1063: Multiple SrDAG vertices reference a single PiSDF configure vertex.
	// 1064: Matching not found for unplugged vertex when merging SrDAG.
	// 1065: Vertex was deleted.
	// 1066: Invalid production/consumption value.
	// 1067: Unexpected amount of data received.
	// 1068: Unexpected data values received.

	printf("\nExiting with code %d\n", exitCode);
	exit(exitCode);
}

#endif

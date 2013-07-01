/*
 * MtrixHandler.h
 *
 *  Created on: 1 juil. 2013
 *      Author: yoliva
 */

#ifndef MTRIXHANDLER_H_
#define MTRIXHANDLER_H_

/*
 * Computes the null space or kernel of a matrix.
 * Parameters :
 * 				nb_rows, nb_cols  	: Number of rows and columns respectively.
 * 				matrix			 	: Pointer to the linear system's matrix.
 * 				results				: Vector containing the results.
 * Returns	  :
 * 				0 if OK.
 * 				1 if no solution or singular matrix.
 */
int nullspace(int nb_rows, int nb_cols, int* matrix, int *results);

#endif /* MTRIXHANDLER_H_ */

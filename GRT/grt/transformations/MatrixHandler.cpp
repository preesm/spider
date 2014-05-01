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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Rational.h"

#define MAX_NB_ROWS		32
#define MAX_NB_COLS		32

typedef struct ratio_nb{
	int numerator;
	int denominator;
	int cocient;
	int rest;
}RATIO_NB;


//==============================================================================
void MatrixPrintMxN(int nb_rows, int nb_cols, int* pfMatr)
{
  int i,j;

  printf("------------------------------------------------------------------\n");
  for(i=0; i<nb_rows; i++)
  {
    for(j=0; j<nb_cols; j++)
    {
      printf("%d ", pfMatr[nb_cols*i + j]);
    }
    printf("\n");
  }
}


/*
 * Computes the null space or kernel of a matrix.
 * Parameters :
 * 				nb_rows, nb_cols  	: Number of rows and columns respectively.
 * 				matrix			 	: Pointer to the linear system's matrix.
 * 				results				: Vector containing the results.
 * Returns	  :
 * 				0 if OK.
 * 				1 otherwise.
 */
int nullspace(int nb_rows, int nb_cols, int* matrix, int *results){
	Rational ratioMatrix[MAX_NB_ROWS*MAX_NB_COLS];
	Rational ratioResult[MAX_NB_ROWS];

#define ratioMatrix(r,c) ratioMatrix[r*nb_cols+c]

	/* Copy matrix into ratioMatrix */
	for(int i=0; i<nb_rows*nb_cols; i++){
		ratioMatrix[i] = Rational(matrix[i]);
	}

	for (int i=0; i < nb_cols; i++) {
		Rational pivotMax = 0;
		int maxIndex = i;
		for (int t = i; t < nb_rows; t++) {
			if (ratioMatrix(t,i).getAbs() > pivotMax) {
				maxIndex = t;
				pivotMax = ratioMatrix(t,i).getAbs();
			}
		}

		if (pivotMax != 0 && maxIndex != i) {
			/* Switch Rows */
			Rational tmp;
			for (int t = 0; t < nb_cols; t++) {
				tmp = ratioMatrix(maxIndex,t);
				ratioMatrix(maxIndex,t) = ratioMatrix(i,t);
				ratioMatrix(i,t) = tmp;
			}
		} else if (maxIndex == i && pivotMax != 0) {

		} else {
			break;
		}

		Rational odlPivot = ratioMatrix(i,i);
		for (int t = i; t < nb_cols; t++) {
			ratioMatrix(i,t) = ratioMatrix(i,t)/odlPivot;
		}
		for (int j = i + 1; j < nb_rows; j++) {
			if (ratioMatrix(j,i) != 0) {
				Rational oldji = ratioMatrix(j,i);
				for (int k = 0; k < nb_cols; k++) {
					ratioMatrix(j,k) = ratioMatrix(j,k)
							- oldji*ratioMatrix(i,k);
				}
			}
		}
	}

	for (int i = 0; i < nb_cols; i++) {
		ratioResult[i]=1;
	}

	for(int i=nb_rows-1; i>=0; i--){
		Rational val = 0;
		for (int k = i + 1; k < nb_cols; k++) {
			val = val + ratioMatrix(i,k) * ratioResult[k];
		}
		if (val != 0) {
			if(ratioMatrix(i,i) == 0){
				printf("elt diagonal zero\n");
				return 1;
			}
			ratioResult[i] = val.getAbs() / ratioMatrix(i,i);
		}
	}

	for (int i = 0; i < nb_cols; i++) {
		results[i]=ratioResult[i].toInt();
	}

	return 0;
}

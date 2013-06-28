/*
 * main.cpp
 *
 *  Created on: 27 juin 2013
 *      Author: yoliva
 */

//==============================================================================
//
// Linear System Solution by Gauss method
//
// Developer: Henry Guennadi Levkin
//
//==============================================================================

#include <stdio.h>
#include <math.h>

//==============================================================================
void VectorPrint(int nDim, double* pfVect)
{
  int i;

  printf("------------------------------------------------------------------\n");
  for(i=0; i<nDim; i++)
  {
    printf("%9.2lf ", pfVect[i]);
  }
  printf("\n");
}
//==============================================================================
void MatrixPrint(int nDim, double* pfMatr)
{
  int i,j;

  printf("------------------------------------------------------------------\n");
  for(i=0; i<nDim; i++)
  {
    for(j=0; j<nDim; j++)
    {
      printf("%9.2lf ", pfMatr[nDim*i + j]);
    }
    printf("\n");
  }
}

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


//==============================================================================
// return 1 if system not solving
// nDim - system dimension
// pfMatr - matrix with coefficients
// pfVect - vector with free members
// pfSolution - vector with system solution
// pfMatr becames trianglular after function call
// pfVect changes after function call
//
// Developer: Henry Guennadi Levkin
//
//==============================================================================
int LinearEquationsSolving(int nDim, double* pfMatr, double* pfVect, double* pfSolution)
{
  double fMaxElem;
  double fAcc;

  int i , j, k, m;


  for(k=0; k<(nDim-1); k++) // base row of matrix
  {
    // search of line with max element
    fMaxElem = fabs( pfMatr[k*nDim + k] );
    m = k;
    for(i=k+1; i<nDim; i++)
    {
      if(fMaxElem < fabs(pfMatr[i*nDim + k]) )
      {
        fMaxElem = pfMatr[i*nDim + k];
        m = i;
      }
    }

    // permutation of base line (index k) and max element line(index m)
    if(m != k)
    {
      for(i=k; i<nDim; i++)
      {
        fAcc               = pfMatr[k*nDim + i];
        pfMatr[k*nDim + i] = pfMatr[m*nDim + i];
        pfMatr[m*nDim + i] = fAcc;
      }
      fAcc = pfVect[k];
      pfVect[k] = pfVect[m];
      pfVect[m] = fAcc;
    }

    if( pfMatr[k*nDim + k] == 0.) return 1; // needs improvement !!!

    // triangulation of matrix with coefficients
    for(j=(k+1); j<nDim; j++) // current row of matrix
    {
      fAcc = - pfMatr[j*nDim + k] / pfMatr[k*nDim + k];
      for(i=k; i<nDim; i++)
      {
        pfMatr[j*nDim + i] = pfMatr[j*nDim + i] + fAcc*pfMatr[k*nDim + i];
      }
      pfVect[j] = pfVect[j] + fAcc*pfVect[k]; // free member recalculation
    }
  }

  for(k=(nDim-1); k>=0; k--)
  {
    pfSolution[k] = pfVect[k];
    for(i=(k+1); i<nDim; i++)
    {
      pfSolution[k] -= (pfMatr[k*nDim + i]*pfSolution[i]);
    }
    pfSolution[k] = pfSolution[k] / pfMatr[k*nDim + k];
  }

  return 0;
}


void swap_rows(int row1, int row2, int nb_cols, int* matrix){
	int temp_coeff;
	for (int j = 0; j < nb_cols; j++) {
		temp_coeff = matrix[row1 * nb_cols + j];
		matrix[row1 * nb_cols + j] = matrix[row2 * nb_cols + j];
		matrix[row2 * nb_cols + j] = temp_coeff;
	}
}


int AllIntegerEchelonMethod(int nb_rows, int nb_cols, int *matrix, int *last_pivot, int* last_row){
	int curr_pivot, prev_pivot = 1;
	int upper_left, upper_right, lower_right, lower_left;

	for (int i = 0; i < nb_rows; i++) {
		// Getting current pivot.
		curr_pivot = matrix[i*nb_cols + i];

		// If the pivot is 0, a nonzero pivot within lower rows of the same column needs to be found.
		if (curr_pivot == 0){
			// If the current row has all 0's to the left and a nonzero to the right means no solution or inconsistency.
			int temp_j = i;
			//Verifying there is at least one nonzero value to the left.
			do {
				temp_j++;
				if (matrix[i * nb_cols + temp_j] != 0) break;
			} while (temp_j < nb_cols - 1);

			if(temp_j == nb_cols - 1)
			{
				if (matrix[i * nb_cols + nb_cols - 1] != 0)	// All values to the left are 0's. i.e. no solution.
					return 1;
				else										// The entire row is null, stop the algorithm.
				{
					*last_pivot = prev_pivot;
					*last_row = i - 1;
					return 0;
				}
			}

			// Looking for a nonzero value.
			int temp_i = i;
			do{
				temp_i++;
			}while((matrix[temp_i*nb_cols + i] == 0) && (temp_i < nb_rows));

			if(temp_i == nb_rows) return 1; // Couldn't find nonzero values (Matrix is singular!).

			swap_rows(i, temp_i, nb_cols, matrix);

			curr_pivot = matrix[i*nb_cols + i];
		}

		// Getting previous pivot.
		if (i > 0) prev_pivot = matrix[(i - 1)*nb_cols + (i - 1)];

		// Updating pivots from already reduced columns.
		for (int temp_i = i - 1; temp_i >= 0; temp_i--)
			matrix[temp_i*nb_cols + temp_i] = curr_pivot;

		// Replacing elements using the criss-cross multiplication method.
		for (int temp_j = i + 1; temp_j < nb_cols; temp_j++) { // Iterates over the columns.
			for (int temp_i = 0; temp_i < nb_rows; temp_i++) { // Iterates over the rows.
				if (temp_i != i) { // The current pivot's row remains identical.
					if(temp_i < i){
						// The element is in an upper row from the current pivot.
						upper_left = matrix[temp_i * nb_cols + i];
						upper_right = matrix[temp_i * nb_cols + temp_j];
						lower_left = curr_pivot;
						lower_right = matrix[i * nb_cols + temp_j];

						// Computing and replacing.
						matrix[temp_i * nb_cols + temp_j] = (lower_left * upper_right - upper_left * lower_right)/prev_pivot;
					}
					else
					{
						// The element is in a lower row from the current pivot.
						upper_left = curr_pivot;
						upper_right = matrix[i * nb_cols + temp_j];
						lower_left = matrix[temp_i * nb_cols + i];
						lower_right = matrix[temp_i * nb_cols + temp_j];

						// Computing and replacing.
						matrix[temp_i * nb_cols + temp_j] = (upper_left * lower_right - lower_left * upper_right)/prev_pivot;
					}
				}
			}
		}

		// Filling with 0's the pivot's column (except the pivot!).
		for (int temp_i = 0; temp_i < nb_rows; temp_i++) {
			if(temp_i != i) matrix[temp_i * nb_cols + i] = 0;
		}
	}

	*last_pivot = prev_pivot;
	*last_row = nb_rows - 1;
	return 0;
}




//==============================================================================
// testing of function
//==============================================================================

#define MATRIX_DIMENSION 	4
#define MATRIX_DIMENSION_3 	3
#define NB_ROWS				4
#define NB_COLS				5

int main(int nArgs, char** pArgs)
{
  int nDim = MATRIX_DIMENSION;
  double fMatr[MATRIX_DIMENSION*MATRIX_DIMENSION] =
  {
  1.0,  2.0, -1.0, -2.0,
  1.0,  3.0, -1.0, -2.0,
  2.0,  1.0,  1.0,  1.0,
  3.0,  1.0,  2.0,  1.0,
  };
  double fVec[MATRIX_DIMENSION] = {-6.0, -4.0, 11.0, 15.0};

  double fSolution[MATRIX_DIMENSION];
  int res;
  int last_pivot, last_row;
  int coeff, rest;

  res = LinearEquationsSolving(nDim, fMatr, fVec, fSolution); // !!!

  if(res)
  {
    printf("No solution!\n");
    return 1;
  }
  else
  {
    printf("Solution:\n");
    VectorPrint(nDim, fSolution);

  }


  int matrix[NB_ROWS * NB_COLS] =
  {
	3, -2, 0, 0, 0,
	3, 0, -2, 0, 0,
	0, 2, 0, -4, 0,
	0, 0, 2, -4, 0
  };

  MatrixPrintMxN(NB_ROWS, NB_COLS, matrix);

  res = AllIntegerEchelonMethod(NB_ROWS, NB_COLS, matrix, &last_pivot, &last_row);

  if(res)
  {
	printf("No solution!\n");
	MatrixPrintMxN(4, 5, matrix);
  }
  else
  {
	  MatrixPrintMxN(last_row + 1, NB_COLS, matrix);
	  for (int i = 0; i <= last_row; i++) {
		  coeff = matrix[i * NB_COLS + NB_COLS - 2];
		  if(coeff >= last_pivot)
		  {
			  rest = coeff % last_pivot;
			  if(rest > 0)
		  }

	}
  }


  int matrix_3x3[MATRIX_DIMENSION_3*MATRIX_DIMENSION] =
  {
	2, -6, 4, 1,
	4, -10, 10, 3,
	1, -2, 3, 2
  };

  MatrixPrintMxN(MATRIX_DIMENSION_3, MATRIX_DIMENSION, matrix_3x3);

  res = AllIntegerEchelonMethod(MATRIX_DIMENSION_3, MATRIX_DIMENSION, matrix_3x3, &last_pivot, &last_row);

  if(res)
  {
	printf("No solution!\n");
	MatrixPrintMxN(3, 4, matrix);
  }
  else
	  MatrixPrintMxN(last_row + 1, MATRIX_DIMENSION - 1, matrix_3x3);

  return 0;
}

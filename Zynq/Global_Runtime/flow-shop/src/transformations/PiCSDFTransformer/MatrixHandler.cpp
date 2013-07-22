/*
 * MatrixHandler.cpp
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
#include <stdlib.h>
#include <math.h>

#define MAX_NB_ROWS		32

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
 * Swaps two matrix's rows.
 */
void swap_rows(int row1, int row2, int nb_cols, int* matrix){
	int temp_coeff;
	for (int j = 0; j < nb_cols; j++) {
		temp_coeff = matrix[row1 * nb_cols + j];
		matrix[row1 * nb_cols + j] = matrix[row2 * nb_cols + j];
		matrix[row2 * nb_cols + j] = temp_coeff;
	}
}


/*
 * Resolves a linear system's matrix by computing the matrix's Echelon reduced form.
 * Parameters :
 * 				nb_rows, nb_cols : Number of rows and columns respectively.
 * 				matrix			 : Pointer to the linear system's matrix.
 * 				last_pivot 		 : Stores the value of the last used pivot within the algorithm.
 * 				last_row		 : Stores the index of the last valid row of the matrix after reduction.
 * Returns	  : 1 - if the system is inconsistent (no solution) or the matrix is singular.
 * 				0 - other way.
 */
int AllIntegerEchelonMethod(int nb_rows, int nb_cols, int *matrix, int *last_pivot, int* last_row){
	int curr_pivot = 0;
	int prev_pivot = 1;
	int upper_left, upper_right, lower_right, lower_left;

	// TODO: Handle eventual errors.
	for (int i = 0; i < nb_rows; i++) {

		// Getting previous pivot.
		if (i > 0) prev_pivot = matrix[(i - 1)*nb_cols + (i - 1)];

		// Getting current pivot.
		curr_pivot = matrix[i*nb_cols + i];

		if (curr_pivot == 0){
			// If the current row has all 0's to the left and a nonzero to the right means no solution or inconsistency.
			// Verifying there is at least one nonzero value to the left.
			int temp_j = i;
			do {
				temp_j++;
				if (matrix[i * nb_cols + temp_j] != 0) break;
			} while (temp_j < nb_cols - 2);

			if(temp_j == nb_cols - 1)
			{
				// The entire row is null.
				// Ending the algorithm assuming that this is the last row.
				*last_pivot = prev_pivot;
				*last_row = i - 1;
				return 0;
			}
			else
				// All values to the left are 0's. i.e. no solution.
				return 1;


			// Looking for a nonzero value within the column.
			int temp_i = i;
			do{
				temp_i++;
			}while((matrix[temp_i*nb_cols + i] == 0) && (temp_i < nb_rows));

			if(temp_i == nb_rows) return 1; // Couldn't find nonzero values (Matrix is singular!).

			swap_rows(i, temp_i, nb_cols, matrix);

			curr_pivot = matrix[i*nb_cols + i];
		}

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

	*last_pivot = curr_pivot;
	*last_row = nb_rows - 1;
	return 0;
}


/*
 * Computes the greatest common divisor between a and b by using the Euclidean algorithm.
 * It assumes : a > b, b >= 0.
 */
int greatest_common_divisor(int a, int b){
    int t;
    while (b != 0){
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}


/*
 * Computes the least common multiple between a and b by using the Euclidean algorithm.
 */
int lcm(int a, int b)
{
	return (a * b / greatest_common_divisor(a, b));
}


/*
 * Computes the least common denominator for "nb_elements" rational numbers.
 */
int least_common_multiple(RATIO_NB* rational_vector, int nb_elements){
	int result = rational_vector[0].denominator;
	for (int i = 1; i <= nb_elements; i++)
		result = lcm(result, rational_vector[i].denominator);
	return result;
}


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
int nullspace(int nb_rows, int nb_cols, int* matrix, int *results){
	/* At this point, the matrix should contain only one column with nonzero elements.
	* It will be divided by "last_pivot" to obtain the values of each variable.
	* As the results must be integer values, a vector of rational numbers (rational_vector) is used .
	* Then the greatest common divisor (GCD) is computed to reduce the fraction.
	* Afterwards, the least common multiplier (LCM) is computed for the denominators of all fractions.
	* The final results are stored in the "results" vector.
	*/
	int last_pivot, last_row;
	int res = AllIntegerEchelonMethod(nb_rows, nb_cols, matrix, &last_pivot, &last_row);

	if(res) return 1;

	int matrix_elem;
	int gcd, lcm;
	RATIO_NB rational_vector[MAX_NB_ROWS];

	for (int i = 0; i <= last_row; i++) {
	  matrix_elem = abs(matrix[i * nb_cols + nb_cols - 1]);
	  if(matrix_elem >= last_pivot)
		  gcd = greatest_common_divisor(matrix_elem, last_pivot);
	  else
		  gcd = greatest_common_divisor(last_pivot, matrix_elem);

	  rational_vector[i].numerator = matrix_elem / gcd;
	  rational_vector[i].denominator = last_pivot / gcd;
	}

	lcm = least_common_multiple(rational_vector, last_row);

	for (int i = 0; i <= last_row; i++) {
	  results[i] = lcm * rational_vector[i].numerator/rational_vector[i].denominator;
	}
	results[nb_cols - 1] = lcm;
	return 0;
}

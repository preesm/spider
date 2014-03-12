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
 * Returns	  : 0 - if OK.
 * 				1 - otherwise.
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
			// Looking for a nonzero value downward from the current position.
			int temp_i = i;
			do{
				temp_i++;
			}while((matrix[temp_i*nb_cols + i] == 0) && (temp_i < nb_rows));

			if(temp_i == nb_rows)
			{ // Couldn't find nonzero values within the rest of the column.

				// Looking for a nonzero value leftward from the current position.
				int temp_j = i;
				do {
					temp_j++;
					if (matrix[i * nb_cols + temp_j] != 0) break;
				} while (temp_j < nb_cols - 2);

				if(temp_j == nb_cols)
				{ // Couldn't find nonzero values within the rest of the column.

					/*
					 * TODO: Ending the algorithm assuming that this is the last row. Not sure of this!!!!!!!
					 *
					 */
					*last_pivot = prev_pivot;
					*last_row = i - 1;
					return 0;
				}
			}
			else{
				swap_rows(i, temp_i, nb_cols, matrix);
				curr_pivot = matrix[i*nb_cols + i];
			}
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
 * 				1 otherwise.
 */
int nullspace(int nb_rows, int nb_cols, int* matrix, int *results){

	int last_pivot = 0;
	int last_row = 0;
	int res = AllIntegerEchelonMethod(nb_rows, nb_cols, matrix, &last_pivot, &last_row);

	/* At this point, the matrix should contain only one column with nonzero elements.
	* It will be divided by "last_pivot" to obtain the values of each variable.
	* As the results must be integer values, a vector of rational numbers (rational_vector) is used .
	* Then the greatest common divisor (GCD) is computed to reduce the fraction.
	* Afterwards, the least common multiplier (LCM) is computed for the denominators of all fractions.
	* The final results are stored in the "results" vector.
	*/

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
	  results[i] = abs(lcm * rational_vector[i].numerator/rational_vector[i].denominator);
	}
	results[nb_cols - 1] = abs(lcm);
	return 0;
}
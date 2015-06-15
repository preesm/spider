#ifndef DATA_H
#define DATA_H

/* Constants */
#define N_DATA 65536

/* Type Definition */
typedef struct Cplx16{
	short	imag;
	short	real;
} Cplx16;

extern Cplx16 data_in[N_DATA];

#endif //DATA_H

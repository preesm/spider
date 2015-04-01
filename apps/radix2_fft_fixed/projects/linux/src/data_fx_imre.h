#ifndef DATA_H
#define DATA_H

/* Constants */
#define N_DATA 65536

/* Type Definition */
typedef struct Cplx16{
	short	imag;
	short	real;
} Cplx16;

typedef struct Cplx32{
	long	imag;
	long	real;
} Cplx32;

typedef struct CplxSp{
	float	imag;
	float	real;
} CplxSp;

extern Cplx16 data_in[N_DATA];
extern CplxSp data_out[N_DATA];

#endif //DATA_H

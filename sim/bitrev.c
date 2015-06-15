#include <math.h>
#include <stdio.h>
#include <string.h>

#define N1 16
#define N2 4096

/*N is the maximum number of the index, v is the index.*/
static inline int bitRev(short v, int N) {
	short r=0;
	int logN = log2(N);
	for(int n=0; n<logN; n++){
		r = (r<<1) + (v & 0x1);
		v = v>>1;
	}
	return r;
}

void bin(unsigned n)
{
	unsigned i;
  for (i = 1 << 15; i > 0; i = i / 2)
     (n & i)? printf("1"): printf("0");
}

void validate_bitreverse(void) {
		//validate bitreverse function
		for(int i=0; i<N1; i++){
    	printf ("%5d,%5d\n", i, bitRev(i, N1));
		}
}

void validate_interleaving(void) {
		//validate interleaving
		for(int n1=0; n1<N1; n1++){
			for(int n2=0; n2<N2; n2++){
				printf ("%d, %d\n", bitRev(n1, N1)+n2*N1, n1*N2+n2);
			}
		}
}


int main (int argc, char** argv) {
  unsigned short val;

	if (argc != 2) {
		printf("Error : too much arguments or argument is missing. (\"bitrev\" or \"itleave\")\n");
		return 1;
	}

	if (!strcmp(argv[1], "bitrev"))
		validate_bitreverse();
	else if (!strcmp(argv[1], "itleave"))
		validate_interleaving();
	else
		printf("Error : too much arguments or argument is missing. (\"bitrev\" or \"itleave\")\n");

}

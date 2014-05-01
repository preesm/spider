
#include "actors.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void sobelTile(unsigned char* in, unsigned char* out, int nbSlices, int nbTiles, int width, int height){
	int i, j, g, k;
	int tileWidth = width/nbTiles;
	int tileHeight = height/nbSlices;
//	memset(out,0,width/nbTiles);
//	for(i=1; i<height/nbSlices-1; i++){
//		out[i*width] = 0;
//		for(j=1; j<width/nbTiles-1; j++){
//			int gx = -(int)in[(i-1)*width + j-1] -2*(int)in[  i*width + j-1] -(int)in[(i+1)*width + j-1]
//					 +(int)in[(i-1)*width + j+1] +2*(int)in[  i*width + j+1] +(int)in[(i+1)*width + j+1];
//			int gy = -(int)in[(i-1)*width + j-1] -2*(int)in[(i-1)*width + j] -(int)in[(i-1)*width + j+1]
//					 +(int)in[(i+1)*width + j-1] +2*(int)in[(i+1)*width + j] +(int)in[(i+1)*width + j+1];
//
//			out[i*width + j] = (abs(gx) + abs(gy))/8;
//		}
//		out[i*width+width/nbTiles-1] = 0;
//	}
//	memset(out+(height/nbSlices-1)*width,0,width/nbTiles);
//}
	memset(out,0,3*tileWidth);
	for(i=3; i<tileHeight-3; i++){
		out[i*tileWidth] = 0;
		out[i*tileWidth+1] = 0;
		out[i*tileWidth+2] = 0;

		for(j=3; j<tileWidth-3; j++){

			 g = 0;
			 g+= 3*in[(i-3)*tileWidth + j-3]; g+= 2*in[(i-3)*tileWidth + j-2]; g+= 1*in[(i-3)*tileWidth + j-1]; g+=-1*in[(i-3)*tileWidth + j+1]; g+=-2*in[(i-3)*tileWidth + j+2]; g+=-3*in[(i-3)*tileWidth + j+3];
			 g+= 4*in[(i-2)*tileWidth + j-3]; g+= 3*in[(i-2)*tileWidth + j-2]; g+= 2*in[(i-2)*tileWidth + j-1]; g+=-2*in[(i-2)*tileWidth + j+1]; g+=-3*in[(i-2)*tileWidth + j+2]; g+=-4*in[(i-2)*tileWidth + j+3];
			 g+= 5*in[(i-1)*tileWidth + j-3]; g+= 4*in[(i-1)*tileWidth + j-2]; g+= 3*in[(i-1)*tileWidth + j-1]; g+=-3*in[(i-1)*tileWidth + j+1]; g+=-4*in[(i-1)*tileWidth + j+2]; g+=-5*in[(i-1)*tileWidth + j+3];
			 g+= 6*in[(i  )*tileWidth + j-3]; g+= 5*in[(i  )*tileWidth + j-2]; g+= 4*in[(i  )*tileWidth + j-1]; g+=-4*in[(i  )*tileWidth + j+1]; g+=-5*in[(i  )*tileWidth + j+2]; g+=-6*in[(i  )*tileWidth + j+3];
			 g+= 5*in[(i+1)*tileWidth + j-3]; g+= 4*in[(i+1)*tileWidth + j-2]; g+= 3*in[(i+1)*tileWidth + j-1]; g+=-3*in[(i+1)*tileWidth + j+1]; g+=-4*in[(i+1)*tileWidth + j+2]; g+=-5*in[(i+1)*tileWidth + j+3];
			 g+= 4*in[(i+2)*tileWidth + j-3]; g+= 3*in[(i+2)*tileWidth + j-2]; g+= 2*in[(i+2)*tileWidth + j-1]; g+=-2*in[(i+2)*tileWidth + j+1]; g+=-3*in[(i+2)*tileWidth + j+2]; g+=-4*in[(i+2)*tileWidth + j+3];
			 g+= 3*in[(i+3)*tileWidth + j-3]; g+= 2*in[(i+3)*tileWidth + j-2]; g+= 1*in[(i+3)*tileWidth + j-1]; g+=-1*in[(i+3)*tileWidth + j+1]; g+=-2*in[(i+3)*tileWidth + j+2]; g+=-3*in[(i+3)*tileWidth + j+3];

			 k = 0;
			 k+=-3*in[(i-3)*tileWidth + j-3]; k+=-4*in[(i-3)*tileWidth + j-2]; k+=-5*in[(i-3)*tileWidth + j-1]; k+=-6*in[(i-3)*tileWidth + j]; k+=-5*in[(i-3)*tileWidth + j+1]; k+=-4*in[(i-3)*tileWidth + j+2]; k+=-3*in[(i-3)*tileWidth + j+3];
			 k+=-2*in[(i-2)*tileWidth + j-3]; k+=-3*in[(i-2)*tileWidth + j-2]; k+=-4*in[(i-2)*tileWidth + j-1]; k+=-5*in[(i-2)*tileWidth + j]; k+=-4*in[(i-2)*tileWidth + j+1]; k+=-3*in[(i-2)*tileWidth + j+2]; k+=-2*in[(i-2)*tileWidth + j+3];
			 k+=-1*in[(i-1)*tileWidth + j-3]; k+=-2*in[(i-1)*tileWidth + j-2]; k+=-3*in[(i-1)*tileWidth + j-1]; k+=-4*in[(i-1)*tileWidth + j]; k+=-3*in[(i-1)*tileWidth + j+1]; k+=-2*in[(i-1)*tileWidth + j+2]; k+=-1*in[(i-1)*tileWidth + j+3];
			 k+= 1*in[(i+1)*tileWidth + j-3]; k+= 2*in[(i+1)*tileWidth + j-2]; k+= 3*in[(i+1)*tileWidth + j-1]; k+= 4*in[(i  )*tileWidth + j]; k+= 3*in[(i+1)*tileWidth + j+1]; k+= 2*in[(i+1)*tileWidth + j+2]; k+=-1*in[(i+1)*tileWidth + j+3];
			 k+= 2*in[(i+2)*tileWidth + j-3]; k+= 3*in[(i+2)*tileWidth + j-2]; k+= 4*in[(i+2)*tileWidth + j-1]; k+= 5*in[(i+1)*tileWidth + j]; k+= 4*in[(i+2)*tileWidth + j+1]; k+= 3*in[(i+2)*tileWidth + j+2]; k+=-2*in[(i+2)*tileWidth + j+3];
			 k+= 3*in[(i+3)*tileWidth + j-3]; k+= 4*in[(i+3)*tileWidth + j-2]; k+= 5*in[(i+3)*tileWidth + j-1]; k+= 6*in[(i+2)*tileWidth + j]; k+= 5*in[(i+3)*tileWidth + j+1]; k+= 4*in[(i+3)*tileWidth + j+2]; k+=-3*in[(i+3)*tileWidth + j+3];
			out[i*tileWidth + j] = (abs(g) + abs(k)) / 266;
		}
		out[(i+1)*tileWidth-1] = 0;
		out[(i+1)*tileWidth-2] = 0;
		out[(i+1)*tileWidth-3] = 0;
	}
	memset(out+(tileHeight-3)*tileWidth,0,3*tileWidth);

//void sobelSlice(unsigned char* in, unsigned char* out, int nbSlices, int width, int height){
//	int i, j;
//	memset(out,0,width);
//	for(i=1; i<height/nbSlices-1; i++){
//		out[i*width] = 0;
//		for(j=1; j<width-1; j++){
//			int gx = -(int)in[(i-1)*width + j-1] -2*(int)in[  i*width + j-1] -(int)in[(i+1)*width + j-1]
//					 +(int)in[(i-1)*width + j+1] +2*(int)in[  i*width + j+1] +(int)in[(i+1)*width + j+1];
//			int gy = -(int)in[(i-1)*width + j-1] -2*(int)in[(i-1)*width + j] -(int)in[(i-1)*width + j+1]
//					 +(int)in[(i+1)*width + j-1] +2*(int)in[(i+1)*width + j] +(int)in[(i+1)*width + j+1];
//
//			out[i*width + j] = (abs(gx) + abs(gy))/8;
//		}
//		out[i*width+width-1] = 0;
//	}
//	memset(out+(height/nbSlices-1)*width,0,width);
}

void sobelSlice(unsigned char* in, unsigned char* out, int nbSlices, int width, int height){
	int i, j,g ,k;

	memset(out,0,3*width);
	for(i=3; i<height/nbSlices-3; i++){
		out[i*width] = 0;
		out[i*width+1] = 0;
		out[i*width+2] = 0;

		for(j=3; j<width-3; j++){

			 g = 0;
			 g+= 3*in[(i-3)*width + j-3]; g+= 2*in[(i-3)*width + j-2]; g+= 1*in[(i-3)*width + j-1]; g+=-1*in[(i-3)*width + j+1]; g+=-2*in[(i-3)*width + j+2]; g+=-3*in[(i-3)*width + j+3];
			 g+= 4*in[(i-2)*width + j-3]; g+= 3*in[(i-2)*width + j-2]; g+= 2*in[(i-2)*width + j-1]; g+=-2*in[(i-2)*width + j+1]; g+=-3*in[(i-2)*width + j+2]; g+=-4*in[(i-2)*width + j+3];
			 g+= 5*in[(i-1)*width + j-3]; g+= 4*in[(i-1)*width + j-2]; g+= 3*in[(i-1)*width + j-1]; g+=-3*in[(i-1)*width + j+1]; g+=-4*in[(i-1)*width + j+2]; g+=-5*in[(i-1)*width + j+3];
			 g+= 6*in[(i  )*width + j-3]; g+= 5*in[(i  )*width + j-2]; g+= 4*in[(i  )*width + j-1]; g+=-4*in[(i  )*width + j+1]; g+=-5*in[(i  )*width + j+2]; g+=-6*in[(i  )*width + j+3];
			 g+= 5*in[(i+1)*width + j-3]; g+= 4*in[(i+1)*width + j-2]; g+= 3*in[(i+1)*width + j-1]; g+=-3*in[(i+1)*width + j+1]; g+=-4*in[(i+1)*width + j+2]; g+=-5*in[(i+1)*width + j+3];
			 g+= 4*in[(i+2)*width + j-3]; g+= 3*in[(i+2)*width + j-2]; g+= 2*in[(i+2)*width + j-1]; g+=-2*in[(i+2)*width + j+1]; g+=-3*in[(i+2)*width + j+2]; g+=-4*in[(i+2)*width + j+3];
			 g+= 3*in[(i+3)*width + j-3]; g+= 2*in[(i+3)*width + j-2]; g+= 1*in[(i+3)*width + j-1]; g+=-1*in[(i+3)*width + j+1]; g+=-2*in[(i+3)*width + j+2]; g+=-3*in[(i+3)*width + j+3];

			 k = 0;
			 k+=-3*in[(i-3)*width + j-3]; k+=-4*in[(i-3)*width + j-2]; k+=-5*in[(i-3)*width + j-1]; k+=-6*in[(i-3)*width + j]; k+=-5*in[(i-3)*width + j+1]; k+=-4*in[(i-3)*width + j+2]; k+=-3*in[(i-3)*width + j+3];
			 k+=-2*in[(i-2)*width + j-3]; k+=-3*in[(i-2)*width + j-2]; k+=-4*in[(i-2)*width + j-1]; k+=-5*in[(i-2)*width + j]; k+=-4*in[(i-2)*width + j+1]; k+=-3*in[(i-2)*width + j+2]; k+=-2*in[(i-2)*width + j+3];
			 k+=-1*in[(i-1)*width + j-3]; k+=-2*in[(i-1)*width + j-2]; k+=-3*in[(i-1)*width + j-1]; k+=-4*in[(i-1)*width + j]; k+=-3*in[(i-1)*width + j+1]; k+=-2*in[(i-1)*width + j+2]; k+=-1*in[(i-1)*width + j+3];
			 k+= 1*in[(i+1)*width + j-3]; k+= 2*in[(i+1)*width + j-2]; k+= 3*in[(i+1)*width + j-1]; k+= 4*in[(i  )*width + j]; k+= 3*in[(i+1)*width + j+1]; k+= 2*in[(i+1)*width + j+2]; k+=-1*in[(i+1)*width + j+3];
			 k+= 2*in[(i+2)*width + j-3]; k+= 3*in[(i+2)*width + j-2]; k+= 4*in[(i+2)*width + j-1]; k+= 5*in[(i+1)*width + j]; k+= 4*in[(i+2)*width + j+1]; k+= 3*in[(i+2)*width + j+2]; k+=-2*in[(i+2)*width + j+3];
			 k+= 3*in[(i+3)*width + j-3]; k+= 4*in[(i+3)*width + j-2]; k+= 5*in[(i+3)*width + j-1]; k+= 6*in[(i+2)*width + j]; k+= 5*in[(i+3)*width + j+1]; k+= 4*in[(i+3)*width + j+2]; k+=-3*in[(i+3)*width + j+3];
			out[i*width + j] = (abs(g) + abs(k)) / 266;
		}
		out[(i+1)*width-1] = 0;
		out[(i+1)*width-2] = 0;
		out[(i+1)*width-3] = 0;
	}
	memset(out+(height/nbSlices-3)*width,0,3*width);

//	memset(out,0,2*width);
//	for(i=2; i<height/nbSlices-2; i++){
//		out[i*width] = 0;
//		out[i*width+1] = 0;
//		for(j=2; j<width-2; j++){
//			int gx =  1*(int)in[(i-2)*width + j-2] +2 *(int)in[(i-2)*width + j-1] -2 *(int)in[(i-2)*width + j+1] -1*(int)in[(i-2)*width + j+2]
//					 +4*(int)in[(i-1)*width + j-2] +8 *(int)in[(i-1)*width + j-1] -8 *(int)in[(i-1)*width + j+1] -4*(int)in[(i-1)*width + j+2]
//					 +6*(int)in[(i-0)*width + j-2] +12*(int)in[(i-0)*width + j-1] -12*(int)in[(i-0)*width + j+1] -6*(int)in[(i-0)*width + j+2]
//					 +4*(int)in[(i+1)*width + j-2] +8 *(int)in[(i+1)*width + j-1] -8 *(int)in[(i+1)*width + j+1] -4*(int)in[(i+1)*width + j+2]
//					 +1*(int)in[(i+2)*width + j-2] +2 *(int)in[(i+2)*width + j-1] -2 *(int)in[(i+2)*width + j+1] -1*(int)in[(i+2)*width + j+2];
//
//		    int gy = -1*(int)in[(i-2)*width + j-2] -4 *(int)in[(i-2)*width + j-1] -6 *(int)in[(i-2)*width + j+1] -4 *(int)in[(i-2)*width + j] -1*(int)in[(i-2)*width + j+2]
//					 -2*(int)in[(i-1)*width + j-2] -8 *(int)in[(i-1)*width + j-1] -12*(int)in[(i-1)*width + j+1] -8 *(int)in[(i-1)*width + j] -2*(int)in[(i-1)*width + j+2]
//					 +2*(int)in[(i+1)*width + j-2] +8 *(int)in[(i+1)*width + j-1] +12*(int)in[(i+1)*width + j+1] +8 *(int)in[(i+1)*width + j] +2*(int)in[(i+1)*width + j+2]
//					 +1*(int)in[(i+2)*width + j-2] +4 *(int)in[(i+2)*width + j-1] +6 *(int)in[(i+2)*width + j+1] +4 *(int)in[(i+2)*width + j] +1*(int)in[(i+2)*width + j+2];
//
//			out[i*width + j] = (abs(gx) + abs(gy))/156;
//		}
//		out[i*width+width-2] = 0;
//		out[i*width+width-1] = 0;
//	}
//	memset(out+(height/nbSlices-2)*width,0,2*width);

//	memset(out,0,width);
//	for(i=1; i<height/nbSlices-1; i++){
//		out[i*width] = 0;
//		for(j=1; j<width-1; j++){
//			int gx = -(int)in[(i-1)*width + j-1] -2*(int)in[  i*width + j-1] -(int)in[(i+1)*width + j-1]
//					 +(int)in[(i-1)*width + j+1] +2*(int)in[  i*width + j+1] +(int)in[(i+1)*width + j+1];
//			int gy = -(int)in[(i-1)*width + j-1] -2*(int)in[(i-1)*width + j] -(int)in[(i-1)*width + j+1]
//					 +(int)in[(i+1)*width + j-1] +2*(int)in[(i+1)*width + j] +(int)in[(i+1)*width + j+1];
//
//			out[i*width + j] = (abs(gx) + abs(gy))/8;
//		}
//		out[i*width+width-1] = 0;
//	}
//	memset(out+(height/nbSlices-1)*width,0,width);
}

//inline int partition( unsigned char a[], int l, int r) {
//   int pivot, i, j, t;
//   pivot = a[l];
//   i = l; j = r+1;
//
//   while( 1)
//   {
//   	do ++i; while( a[i] <= pivot && i <= r );
//   	do --j; while( a[j] > pivot );
//   	if( i >= j ) break;
//   	t = a[i]; a[i] = a[j]; a[j] = t;
//   }
//   t = a[l]; a[l] = a[j]; a[j] = t;
//   return j;
//}
//
//inline void quickSort( unsigned char a[], int l, int r)
//{
//   int j;
//
//   if( l < r )
//   {
//   	// divide and conquer
//        j = partition( a, l, r);
//       quickSort( a, l, j-1);
//       quickSort( a, j+1, r);
//   }
//
//}

//void medianSlice(unsigned char* in, unsigned char* out, int nbSlices, int width, int height){
//	int i, j, pi, pj;
//	unsigned char median_array[3*3];
//
//	memset(out,0,width);
//	for(i=1; i<height/nbSlices-1; i++){
//		out[i*width] = 0;
//		for(j=1; j<width-1; j++){
//			int iter=0;
//			for(pi=-1; pi<2; pi++){
//				for(pj=-1; pj<2; pj++){
//					median_array[iter++] = in[(i+pi)*width+(j+pj)];
//				}
//			}
//
//			for(pi=0; pi<9-1; pi++){
//				if(median_array[pi] > median_array[pi+1]){
//					int tmp = median_array[pi];
//					median_array[pi] = median_array[pi+1];
//					median_array[pi+1] = tmp;
//					pi = 0;
//				}
//			}
////			quickSort(median_array, 0, 8);
//
//			out[i*width + j] = median_array[4];
//		}
//		out[(i+1)*width-1] = 0;
//	}
//	memset(out+(height/nbSlices-1)*width,0,width);
//}

//void medianSlice(unsigned char* in, unsigned char* out, int nbSlices, int width, int height){
//	int i, j, pi, pj;
//	unsigned char histo[256];
//
//	memset(out,0,width);
//	for(i=1; i<height/nbSlices-1; i++){
//		out[i*width] = 0;
//
//		memset(histo, 0, 256);
//		for(pi=-1; pi<2; pi++){
//			for(pj=-1; pj<1; pj++){
//				histo[in[(i+pi)*width+(1+pj)]]++;
//			}
//		}
//
//		for(j=1; j<width-1; j++){
//			// Histo
//			if(j-2>=0)
//				for(pi=-1; pi<2; pi++)
//					histo[in[(i+pi)*width+(j-2)]]--;
//
//			for(pi=-1; pi<2; pi++){
//				histo[in[(i+pi)*width+(j+1)]]++;
//			}
//
//			int median,n;
//			for (n = 5, median = 0; median < 256 && (n -= histo[median]) > 0; median++);
//
//			out[i*width + j] = median;
//		}
//		out[(i+1)*width-1] = 0;
//	}
//	memset(out+(height/nbSlices-1)*width,0,width);
//}
//
//void medianTile(unsigned char* in, unsigned char* out, int nbSlices, int nbTiles, int width, int height){
//	int i, j;
//	unsigned char histo[256];
//	int tileWidth = width/nbTiles;
//	int tileHeight = height/nbSlices;
//
//	memset(out,0,tileWidth);
//	for(i=1; i<tileHeight-1; i++){
//		out[i*tileWidth] = 0;
//
//		memset(histo, 0, 256);
//
//		histo[in[(i-1)*tileWidth  ]]++;
//		histo[in[(i-1)*tileWidth+1]]++;
//		histo[in[(i  )*tileWidth  ]]++;
//		histo[in[(i  )*tileWidth+1]]++;
//		histo[in[(i+1)*tileWidth  ]]++;
//		histo[in[(i+1)*tileWidth+1]]++;
//
//
//		for(j=1; j<tileWidth-1; j++){
//			// Histo
//			if(j-2>=0){
//				histo[in[(i-1)*tileWidth+(j-2)]]--;
//				histo[in[(i)  *tileWidth+(j-2)]]--;
//				histo[in[(i+1)*tileWidth+(j-2)]]--;
//			}
//
//			histo[in[(i-1)*tileWidth+(j+1)]]++;
//			histo[in[(i  )*tileWidth+(j+1)]]++;
//			histo[in[(i+1)*tileWidth+(j+1)]]++;
//
//			int median,n;
//			for (n = 5, median = 0; median < 256 && (n -= histo[median]) > 0; median++);
//
//			out[i*tileWidth + j] = median;
//		}
//		out[(i+1)*tileWidth-1] = 0;
//	}
//	memset(out+(tileHeight-1)*tileWidth,0,tileWidth);
//}

void averageTile(unsigned char* in, unsigned char* out, int nbSlices, int nbTiles, int width, int height){
	int i, j;
	int tileWidth = width/nbTiles;
	int tileHeight = height/nbSlices;
	int g;

	memset(out,0,3*tileWidth);
	for(i=3; i<tileHeight-3; i++){
		out[i*tileWidth] = 0;
		out[i*tileWidth+1] = 0;
		out[i*tileWidth+2] = 0;

		for(j=3; j<tileWidth-3; j++){

			 g = 0;
			 g+= 1*in[(i-3)*tileWidth + j-3]; g+= 2*in[(i-3)*tileWidth + j-2]; g+= 4*in[(i-3)*tileWidth + j-1]; g+= 8*in[(i-3)*tileWidth + j]; g+= 4*in[(i-3)*tileWidth + j+1]; g+= 2*in[(i-3)*tileWidth + j+2]; g+= 1*in[(i-3)*tileWidth + j+3];
			 g+= 2*in[(i-2)*tileWidth + j-3]; g+= 4*in[(i-2)*tileWidth + j-2]; g+= 8*in[(i-2)*tileWidth + j-1]; g+=16*in[(i-2)*tileWidth + j]; g+= 8*in[(i-2)*tileWidth + j+1]; g+= 4*in[(i-2)*tileWidth + j+2]; g+= 2*in[(i-2)*tileWidth + j+3];
			 g+= 4*in[(i-1)*tileWidth + j-3]; g+= 8*in[(i-1)*tileWidth + j-2]; g+=16*in[(i-1)*tileWidth + j-1]; g+=32*in[(i-1)*tileWidth + j]; g+=16*in[(i-1)*tileWidth + j+1]; g+= 8*in[(i-1)*tileWidth + j+2]; g+= 4*in[(i-1)*tileWidth + j+3];
			 g+= 8*in[(i  )*tileWidth + j-3]; g+=16*in[(i  )*tileWidth + j-2]; g+=32*in[(i  )*tileWidth + j-1]; g+=64*in[(i  )*tileWidth + j]; g+=32*in[(i  )*tileWidth + j+1]; g+=16*in[(i  )*tileWidth + j+2]; g+= 8*in[(i  )*tileWidth + j+3];
			 g+= 4*in[(i+1)*tileWidth + j-3]; g+= 8*in[(i+1)*tileWidth + j-2]; g+=16*in[(i+1)*tileWidth + j-1]; g+=32*in[(i+1)*tileWidth + j]; g+=16*in[(i+1)*tileWidth + j+1]; g+= 8*in[(i+1)*tileWidth + j+2]; g+= 4*in[(i+1)*tileWidth + j+3];
			 g+= 2*in[(i+2)*tileWidth + j-3]; g+= 4*in[(i+2)*tileWidth + j-2]; g+= 8*in[(i+2)*tileWidth + j-1]; g+=16*in[(i+2)*tileWidth + j]; g+= 8*in[(i+2)*tileWidth + j+1]; g+= 4*in[(i+2)*tileWidth + j+2]; g+= 2*in[(i+2)*tileWidth + j+3];
			 g+= 1*in[(i+3)*tileWidth + j-3]; g+= 2*in[(i+3)*tileWidth + j-2]; g+= 4*in[(i+3)*tileWidth + j-1]; g+= 8*in[(i+3)*tileWidth + j]; g+= 4*in[(i+3)*tileWidth + j+1]; g+= 2*in[(i+3)*tileWidth + j+2]; g+= 1*in[(i+3)*tileWidth + j+3];
			out[i*tileWidth + j] = g / 480;
		}
		out[(i+1)*tileWidth-1] = 0;
		out[(i+1)*tileWidth-2] = 0;
		out[(i+1)*tileWidth-3] = 0;
	}
	memset(out+(tileHeight-3)*tileWidth,0,3*tileWidth);
}

void averageSlice(unsigned char* in, unsigned char* out, int nbSlices, int width, int height){
	int i, j;
	int g;

	memset(out,0,3*width);
	for(i=3; i<height/nbSlices-3; i++){
		out[i*width] = 0;
		out[i*width+1] = 0;
		out[i*width+2] = 0;

		for(j=3; j<width-3; j++){

			 g = 0;
			 g+= 1*in[(i-3)*width + j-3]; g+= 2*in[(i-3)*width + j-2]; g+= 4*in[(i-3)*width + j-1]; g+= 8*in[(i-3)*width + j]; g+= 4*in[(i-3)*width + j+1]; g+= 2*in[(i-3)*width + j+2]; g+= 1*in[(i-3)*width + j+3];
			 g+= 2*in[(i-2)*width + j-3]; g+= 4*in[(i-2)*width + j-2]; g+= 8*in[(i-2)*width + j-1]; g+=16*in[(i-2)*width + j]; g+= 8*in[(i-2)*width + j+1]; g+= 4*in[(i-2)*width + j+2]; g+= 2*in[(i-2)*width + j+3];
			 g+= 4*in[(i-1)*width + j-3]; g+= 8*in[(i-1)*width + j-2]; g+=16*in[(i-1)*width + j-1]; g+=32*in[(i-1)*width + j]; g+=16*in[(i-1)*width + j+1]; g+= 8*in[(i-1)*width + j+2]; g+= 4*in[(i-1)*width + j+3];
			 g+= 8*in[(i  )*width + j-3]; g+=16*in[(i  )*width + j-2]; g+=32*in[(i  )*width + j-1]; g+=64*in[(i  )*width + j]; g+=32*in[(i  )*width + j+1]; g+=16*in[(i  )*width + j+2]; g+= 8*in[(i  )*width + j+3];
			 g+= 4*in[(i+1)*width + j-3]; g+= 8*in[(i+1)*width + j-2]; g+=16*in[(i+1)*width + j-1]; g+=32*in[(i+1)*width + j]; g+=16*in[(i+1)*width + j+1]; g+= 8*in[(i+1)*width + j+2]; g+= 4*in[(i+1)*width + j+3];
			 g+= 2*in[(i+2)*width + j-3]; g+= 4*in[(i+2)*width + j-2]; g+= 8*in[(i+2)*width + j-1]; g+=16*in[(i+2)*width + j]; g+= 8*in[(i+2)*width + j+1]; g+= 4*in[(i+2)*width + j+2]; g+= 2*in[(i+2)*width + j+3];
			 g+= 1*in[(i+3)*width + j-3]; g+= 2*in[(i+3)*width + j-2]; g+= 4*in[(i+3)*width + j-1]; g+= 8*in[(i+3)*width + j]; g+= 4*in[(i+3)*width + j+1]; g+= 2*in[(i+3)*width + j+2]; g+= 1*in[(i+3)*width + j+3];
			out[i*width + j] = g / 480;
		}
		out[(i+1)*width-1] = 0;
		out[(i+1)*width-2] = 0;
		out[(i+1)*width-3] = 0;
	}
	memset(out+(height/nbSlices-3)*width,0,3*width);
}

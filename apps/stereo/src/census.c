/*
 * census.c
 *
 *  Created on: 19 févr. 2014
 *      Author: jmenant
 */

#include <stdint.h>
#include "stereomatch.h"

/*!
 * Compute the census (3x3 operator) of gray level image
 * \param height : height of image in pixels
 * \param width : width of image in pixels
 * \param image : gray level input image. Size must be (height*width).
 * \param out : output gray level image. Size must be (height*width).
 */
void census(
	int height, int width,
	uint8_t* image,
	uint8_t* out)
{
	int x, y;

	/*store temporary values*/
	/*contains all the value of pixels to compare*/
	uint8_t op[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	/*current pixels to compare*/
	uint8_t val, curpix;
	/*lines pointers (three line are read at on time)*/
	uint8_t *ptr_l1, *ptr_l2, *ptr_l3;
	/*ouput pointer*/
	uint8_t *cout;


	ptr_l1 = image;
	ptr_l2 = image + width;
	ptr_l3 = ptr_l2 + width;
	cout = out+width;
	for(y=1; y<(height-1); y++)
	{
		/*intialize pointer*/
		ptr_l1 = image + (y-1)*width ;
		ptr_l2 = image + (y)*width;
		ptr_l3 = image + (y+1)*width ;

		/*initialize operator content (and manage left border case)*/
		op[1] = op[4] = op[7] = 0;
		op[2] = *ptr_l1++;
		op[5] = *ptr_l2++;
		op[8] = *ptr_l3++;
		for(x=0; x<(width-1); x++)
		{
			/*load values*/
			op[0] = op[1];
			op[1] = op[2];
			op[2] = *ptr_l1++;

			op[3] = op[4];
			op[4] = op[5];
			op[5] = *ptr_l2++;

			op[6] = op[7];
			op[7] = op[8];
			op[8] = *ptr_l3++;
			curpix = op[4];

			/*compute census*/
			val = 0;
			val |= (op[0] > curpix)?0x01:0;
			val |= (op[1] > curpix)?0x02:0;
			val |= (op[2] > curpix)?0x04:0;
			val |= (op[3] > curpix)?0x08:0;
			val |= (op[5] > curpix)?0x10:0;
			val |= (op[6] > curpix)?0x20:0;
			val |= (op[7] > curpix)?0x40:0;
			val |= (op[8] > curpix)?0x80:0;

			/*store output*/
			*cout++ = val;
		}
		/*right border case*/
		val = 0;
		curpix = op[5];
		val |= (op[1] > curpix)?0x01:0;
		val |= (op[2] > curpix)?0x02:0;
		val |= (op[4] > curpix)?0x08:0;
		val |= (op[7] > curpix)?0x20:0;
		val |= (op[8] > curpix)?0x40:0;
		*cout++ = val;
	}
}

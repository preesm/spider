/*
 * image.h
 *
 *  Created on: 8 janv. 2014
 *      Author: jmenant
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <stdlib.h>

struct str_image_rgb
{
	uint32_t w;
	uint32_t h;
	uint8_t* r;
	uint8_t* g;
	uint8_t* b;
};

struct str_image_gray
{
	uint32_t w;
	uint32_t h;
	uint8_t* g;
};



#define IMAGE_XSAT(im, x) ((x) < 0 ? 0 : ((x) >= (im).w ? ((im).w - 1) : (x)))
#define IMAGE_YSAT(im, y) ((y) < 0 ? 0 : ((y) >= (im).h ? ((im).h - 1) : (y)))

#define IMAGE_IDX(im, x, y) ((y)*(im).w + (x))
#define IMAGE_IDXSAT(im, x, y) (IMAGE_YSAT(im, y)*(im).w + IMAGE_XSAT(im, x))

void image_rgb_destroy(struct str_image_rgb* str);
int image_rgb_init(struct str_image_rgb* str, size_t width, size_t heigth);

void image_gray_destroy(struct str_image_gray* str);
int image_gray_init(struct str_image_gray* str, size_t width, size_t heigth);

void image_rgb2gray(struct str_image_gray* dst, struct str_image_rgb* src);

#endif /* IMAGE_H_ */

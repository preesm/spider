/*
 * image.c
 *
 *  Created on: 8 janv. 2014
 *      Author: jmenant
 */

#include "image.h"

#include <stdlib.h>
#include <errno.h>

void image_rgb_destroy(struct str_image_rgb* str)
{
	if(str->r != NULL)
	{
		free(str->r);
	}

	if(str->g != NULL)
	{
		free(str->g);
	}

	if(str->b != NULL)
	{
		free(str->b);
	}

	str->r = NULL;
	str->g = NULL;
	str->b = NULL;

	str->w = str->h = 0;

}

int image_rgb_init(struct str_image_rgb* str, size_t width, size_t heigth)
{
	str->w = width;
	str->h = heigth;
	str->r = str->g = str->b = NULL;

	str->r = calloc(width*heigth, sizeof(uint8_t));
	if(str->r == NULL)
	{
		image_rgb_destroy(str);
		return ENOMEM;
	}

	str->g = calloc(width*heigth, sizeof(uint8_t));
	if(str->g == NULL)
	{
		image_rgb_destroy(str);
		return ENOMEM;
	}

	str->b = calloc(width*heigth, sizeof(uint8_t));
	if(str->b == NULL)
	{
		image_rgb_destroy(str);
		return ENOMEM;
	}

	return 0;
}

void image_gray_destroy(struct str_image_gray* str)
{
	if(str->g)
	{
		free(str->g);
	}
	str->h = str->w = 0;
	str->g = NULL;
}

int image_gray_init(struct str_image_gray* str, size_t width, size_t heigth)
{
	str->w = width;
	str->h = heigth;

	str->g = calloc( width * heigth, sizeof(str->g[0]));

	if(str->g == NULL)
	{
		return ENOMEM;
	}
	return 0;
}

void image_rgb2gray(struct str_image_gray* dst, struct str_image_rgb* src)
{
	uint32_t idx, size;
	size = dst->h*dst->w;

	for(idx = 0; idx<size; idx++)
	{
		dst->g[idx] = (uint16_t)(src->r[idx] + src->g[idx] + src->b[idx])/3;
	}
}



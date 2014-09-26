/*
 * image_png.c
 *
 *  Created on: 8 janv. 2014
 *      Author: jmenant
 */

#include "image_png.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <png.h>

static void copy_pngrgb8_2_rgb(png_byte ** row_pointers, struct str_image_rgb* str)
{
	uint32_t x, y;
	uint8_t *cr, *cg, *cb;
	png_byte* cur_src;

	cr = str->r;
	cg = str->g;
	cb = str->b;

	for(y=0; y<str->h; y++)
	{
		cur_src = row_pointers[y];
		for(x=0; x < str->w; x++)
		{
			*cr++ = *cur_src++;
			*cg++ = *cur_src++;
			*cb++ = *cur_src++;
		}
	}
}

static void copy_pnggray8_2_rgb(png_byte ** row_pointers, struct str_image_rgb* str)
{
	uint32_t x, y;
	uint8_t *cr, *cg, *cb;
	png_byte* cur_src;

	cr = str->r;
	cg = str->g;
	cb = str->b;

	for(y=0; y<str->h; y++)
	{
		cur_src = row_pointers[y];
		for(x=0; x < str->w; x++)
		{
			*cr++ = *cg++ = *cb++ = *cur_src++;
		}
	}
}

int image_rgb_loadPng(const char* filename, struct str_image_rgb* str)
{
	/*PNG variables*/
	png_structp png_p = NULL;
	png_byte header[8];
	png_infop info_p = NULL;

	png_uint_32 width, height, y;
	png_uint_32 rowsize;
	png_byte color_type, bit_depth;
	png_byte ** row_pointers = NULL;
	int etemp;

	int errcode = -1;

	FILE* file = fopen(filename, "rb");
	if(!file)
	{
		fprintf(stderr, "Unable to open \"%s\"\n", filename);
		return -1;
	}

	/*GEt header*/
	if(fread(header, sizeof(header), 1, file) != 1)
	{
		fprintf(stderr, "Error while reading PNG header of \"%s\"\n", filename);
		goto close_file;
	}

	if(png_sig_cmp(header, 0, sizeof(header)))
	{
		fprintf(stderr, "\"%s\" is not a PNG File !\n", filename);
		goto close_file;
	}

	/*Init lib PNG.*/
	png_p = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_p)
	{
		fprintf(stderr, "Unable to create png context !\n");
		goto close_file;
	}
	/*should be the same as memcpy, but i prefer this for alignement reasons*/
	info_p = png_create_info_struct(png_p);

	if (!info_p)
	{
		fprintf(stderr, "Unable to create info context !\n");
		goto free_png;
	}

	if (setjmp(png_jmpbuf(png_p)))
	{
		fprintf(stderr, "Error during init_io");
		goto free_info;
	}

	png_init_io(png_p, file);
	png_set_sig_bytes(png_p, 8);

	/*get image informations*/
	png_read_info(png_p, info_p);

	width = png_get_image_width(png_p, info_p);
	height = png_get_image_height(png_p, info_p);
	color_type = png_get_color_type(png_p, info_p);
	bit_depth = png_get_bit_depth(png_p, info_p);

	if(bit_depth != 8)
	{if((etemp = image_rgb_init(str, width, height)) != 0)
	{
		errcode = etemp;
		goto free_im;
	}
		fprintf(stderr, "Only 8 bits data are supported.\n");
		goto free_im;
	}

	if (setjmp(png_jmpbuf(png_p)))
	{
		fprintf(stderr, "Error during read_image\n");
		goto free_im;
	}


	rowsize = png_get_rowbytes(png_p, info_p);

	row_pointers = (png_bytep*) calloc(height, sizeof(png_bytep));

	if(!row_pointers)
	{
		errcode = ENOMEM;
		goto free_info;
	}

	row_pointers[0] = (png_byte*)malloc(rowsize*height);
	if(!row_pointers[0])
	{
		goto free_im;
	}

	for(y=1; y<height; y++)
	{
		row_pointers[y] = row_pointers[y-1] + rowsize;
	}

	png_read_image(png_p, row_pointers);

	/*copy image in structure*/
	if((etemp = image_rgb_init(str, width, height)) != 0)
	{
		errcode = etemp;
		goto free_im;
	}

	switch(color_type)
	{
	case PNG_COLOR_TYPE_RGB :
		copy_pngrgb8_2_rgb(row_pointers, str);
		break;
	case PNG_COLOR_TYPE_GRAY :
		copy_pnggray8_2_rgb(row_pointers, str);
		break;
	default :
		fprintf(stderr, "PNG Format not managed Yet (only RGB).\n");
		image_rgb_destroy(str);
		goto free_im;
		break;
	}

	/*Deinit and free sequence.*/
	errcode = 0;
free_im:
	if(row_pointers)
	{
		if(row_pointers[0])
		{
			free(row_pointers[0]);
			row_pointers[0] = NULL;
		}
	}

	free(row_pointers);
	row_pointers = NULL;

free_info :
	png_destroy_info_struct(png_p, &info_p);

free_png:
	png_destroy_read_struct(&png_p, NULL, NULL);

close_file:
	fclose(file);

	return errcode;
}

int image_rgb_savePng(const char* filename, const struct str_image_rgb* str)
{
	/*PNG variable*/

	png_structp png_p = NULL;
	png_infop info_p = NULL;

	png_byte ** row_pointers = NULL;
	size_t rowsize;
	uint32_t x, y;
	png_byte* cur_dst;
	const uint8_t *cr, *cg, *cb;

	FILE* file = NULL;
	int errcode = -1;

	/*File mangagement*/
	file = fopen(filename, "wb");

	if(!file)
	{
		fprintf(stderr, "Unable to create \"%s\" !\n", filename);
		return -1;
	}

	/*PNG context*/
	png_p = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_p)
	{
		fprintf(stderr, "Unable to create png context !\n");
		goto close_file;
	}

	info_p = png_create_info_struct(png_p);
	if(!info_p)
	{
		fprintf(stderr, "Unable to create png info !\n");
		goto free_pngp;
	}

	if (setjmp(png_jmpbuf(png_p)))
	{
		fprintf(stderr, "Unable to init png io !\n");
		goto free_infop;
	}

	png_init_io(png_p, file);

	/*PNG File settings and header*/
	if (setjmp(png_jmpbuf(png_p)))
	{
		fprintf(stderr, "Error on header writing !\n");
		goto free_infop;
	}
	png_set_IHDR(png_p, info_p, str->w, str->h, 8, PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_p, info_p);

	/*Write File*/
	row_pointers = calloc(str->h, sizeof(png_byte*));
	if(!row_pointers)
	{
		errcode = ENOMEM;
		goto free_infop;
	}

	rowsize = png_get_rowbytes(png_p, info_p);

	row_pointers[0] = malloc(rowsize*str->h);
	if(!row_pointers[0])
	{
		errcode = ENOMEM;
		goto free_row;
	}


	for(y=1; y<str->h; y++)
	{
		row_pointers[y] = row_pointers[y-1] + rowsize;
	}

	cr = str->r;
	cg = str->g;
	cb = str->b;

	for(y=0; y<str->h; y++)
	{
		cur_dst = row_pointers[y];
		for(x = 0; x < str->w; x++)
		{
			*cur_dst++ = *cr++;
			*cur_dst++ = *cg++;
			*cur_dst++ = *cb++;
		}
	}

	if (setjmp(png_jmpbuf(png_p)))
	{
		fprintf(stderr, "Error on data writing !\n");
		goto free_row;
	}

	png_write_image(png_p, row_pointers);
	png_write_end(png_p, NULL);

	errcode = 0;

	/*Free and cleanup stuff*/
free_row :
	if(row_pointers)
	{
		if(row_pointers[0] != NULL)
		{
			free(row_pointers[0]);
			row_pointers[0] = NULL;
		}
		free(row_pointers);
		row_pointers = NULL;
	}


free_infop:
	png_destroy_info_struct(png_p, &info_p);

free_pngp:
	png_destroy_write_struct(&png_p, NULL);

close_file:
	fclose(file);
	return errcode;
}

int image_gray_savePng(const char* filename, const struct str_image_gray* str)
{
	png_structp png_p = NULL;
	png_infop info_p = NULL;

	png_byte ** row_pointers = NULL;
	size_t rowsize;
	uint32_t x, y;
	png_byte* cur_dst;
	const uint8_t *cg;

	FILE* file = NULL;
	int errcode = -1;

	/*File mangagement*/
	file = fopen(filename, "wb");

	if(!file)
	{
		fprintf(stderr, "Unable to create \"%s\" !\n", filename);
		return -1;
	}

	/*PNG context*/
	png_p = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_p)
	{
		fprintf(stderr, "Unable to create png context !\n");
		goto close_file;
	}

	info_p = png_create_info_struct(png_p);
	if(!info_p)
	{
		fprintf(stderr, "Unable to create png info !\n");
		goto free_pngp;
	}

	if (setjmp(png_jmpbuf(png_p)))
	{
		fprintf(stderr, "Unable to init png io !\n");
		goto free_infop;
	}

	png_init_io(png_p, file);

	/*PNG File settings and header*/
	if (setjmp(png_jmpbuf(png_p)))
	{
		fprintf(stderr, "Error on header writing !\n");
		goto free_infop;
	}
	png_set_IHDR(png_p, info_p, str->w, str->h, 8, PNG_COLOR_TYPE_GRAY,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_p, info_p);

	/*Write File*/
	row_pointers = calloc(str->h, sizeof(png_byte*));
	if(!row_pointers)
	{
		errcode = ENOMEM;
		goto free_infop;
	}

	rowsize = png_get_rowbytes(png_p, info_p);

	row_pointers[0] = malloc(rowsize*str->h);
	if(!row_pointers[0])
	{
		errcode = ENOMEM;
		goto free_row;
	}


	for(y=1; y<str->h; y++)
	{
		row_pointers[y] = row_pointers[y-1] + rowsize;
	}

	cg = str->g;

	for(y=0; y<str->h; y++)
	{
		cur_dst = row_pointers[y];
		for(x = 0; x < str->w; x++)
		{
			*cur_dst++ = *cg++;
		}
	}

	if (setjmp(png_jmpbuf(png_p)))
	{
		fprintf(stderr, "Error on data writing !\n");
		goto free_row;
	}

	png_write_image(png_p, row_pointers);
	png_write_end(png_p, NULL);

	errcode = 0;

	/*Free and cleanup stuff*/
free_row :
	if(row_pointers)
	{
		if(row_pointers[0] != NULL)
		{
			free(row_pointers[0]);
			row_pointers[0] = NULL;
		}
		free(row_pointers);
		row_pointers = NULL;
	}


free_infop:
	png_destroy_info_struct(png_p, &info_p);

free_pngp:
	png_destroy_write_struct(&png_p, NULL);

close_file:
	fclose(file);
	return errcode;
}

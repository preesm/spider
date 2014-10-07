/*
 * compute_stereo.c
 *
 *  Created on: 19 févr. 2014
 *      Author: jmenant
 */

#include "../image/image.h"

#include "stereomatch.h"
#include "compute_stereo.h"
#include <string.h>
#include <stdio.h>

/*
 * WHEN setted, saves intermediate step inside files
 * (./out/w and ./out/c must exist)
 */
//#define DEBUG
static const int deltas[] = {1, 4, 9, 16, 25, 3};

/*!
 * \brief compute a disparity map.
 * \param imr : right image (rgb).
 * \param iml : left image (rgb).
 * \param ndisp :  number of disparity level.
 * \param out : output disparity map.
 */
void stereoMatch(
	struct str_image_rgb* imr,
	struct str_image_rgb* iml,
	int ndisp,
	struct str_image_gray* out)
{
	int i, d;
	int niter = sizeof(deltas)/sizeof(deltas[0]);

	#ifdef DEBUG
	/*variables used only to generate file*/
	char string_filename[128];	/*filename string*/

	/*variables used to reconstruct the weights matrix*/
	int delta;
	struct str_image_gray iw0, iwm, iwp;
	int x,y;
	uint8_t w0, wm, wp;
	#endif/*DEBUG*/

	/*buffer for algorithm*/
	struct str_image_gray imgl, imgr;
	struct str_image_gray vweight[sizeof(deltas)/sizeof(deltas[0])];
	struct str_image_gray hweight[sizeof(deltas)/sizeof(deltas[0])];
	struct str_image_gray cost1, cost2, buffval;
	struct str_image_gray cenl, cenr;

	/*gray scale image*/
	image_gray_init(&imgl, iml->w, iml->h);
	image_gray_init(&imgr, imr->w, imr->h);
	image_rgb2gray(&imgl, iml);
	image_rgb2gray(&imgr, imr);


	/*compute weight*/
	for(i=0; i<niter; i++)
	{
		image_gray_init(&vweight[i], imr->w, imr->h);
		image_gray_init(&hweight[i], imr->w, imr->h);

		compute_vweigth(vweight[i].h, vweight[i].w,
				deltas[i],
				imr->r, imr->g, imr->b,
				vweight[i].g);

		compute_hweigth(hweight[i].h, hweight[i].w,
				deltas[i],
				imr->r, imr->g, imr->b,
				hweight[i].g);
	}

	/*compute census*/
	image_gray_init(&cenl, iml->w, iml->h);
	image_gray_init(&cenr, imr->w, imr->h);

	census(cenl.h, cenl.w, imgl.g, cenl.g);
	census(cenr.h, cenr.w, imgr.g, cenr.g);

	image_gray_init(&cost1, imr->w, imr->h);
	image_gray_init(&cost2, imr->w, imr->h);
	image_gray_init(&buffval, imr->w, imr->h);

	/*compute disparity map*/
	for(d=0; d<ndisp; d++)
	{
		/*cost construction*/
		cost_construction(cost1.h, cost1.w, d,
				imgl.g, imgr.g, cenl.g, cenr.g,
				cost1.g);

		#ifdef DEBUG
		sprintf(string_filename, "out/c/r%d.png", d);
		image_gray_savePng(string_filename, &cost1);
		#endif/*DEBUG*/

		/*recursive aggregation*/
		for(i=0; i<niter; i++)
		{
			aggregateV(imr->h, imr->w, deltas[i], cost1.g, vweight[i].g, cost2.g);
			aggregateH(imr->h, imr->w, deltas[i], cost2.g, hweight[i].g, cost1.g);
			#ifdef DEBUG
			sprintf(string_filename, "out/c/c%d_%d.png", d, i);
			image_gray_savePng(string_filename, &cost1);
			sprintf(string_filename, "out/c/c%d_%dv.png", d, i);
			image_gray_savePng(string_filename, &cost2);
			#endif/*DEBUG*/
		}

		#ifdef DEBUG
		sprintf(string_filename, "out/c/f%d.png", d);
		image_gray_savePng(string_filename, &cost1);
		#endif/*DEBUG*/

		/*disparity is argmin of cost*/
		if(d == 0)
		{
			/*first iteration*/
			memset(out->g, 0, (imr->h*imr->w)*sizeof(uint8_t));
			memcpy(buffval.g, cost1.g, (imr->h*imr->w)*sizeof(uint8_t));
		}
		else
		{
			/*select disparity to minimize cost*/
			for(i=0; i<(imr->h*imr->w); i++)
			{
				/*buffval contains the current minimum cost*/
				if(cost1.g[i] < buffval.g[i])
				{
					buffval.g[i] = cost1.g[i];	/*buffval = min(cost)*/
					out->g[i] = d;				/*out = argmin(cost)*/
				}
			}
		}
	}

#ifdef DEBUG
	/*compute and store three matrix of vweight for each iteration*/
	for(i=0; i<(sizeof(deltas)/sizeof(deltas[0])); i++)
	{
		image_gray_init(&iw0, imr->w, imr->h);
		image_gray_init(&iwp, imr->w, imr->h);
		image_gray_init(&iwm, imr->w, imr->h);

		delta = deltas[i];

		/*compute, same formula used in aggregateV*/
		for(y=delta; y<(vweight[i].h-delta); y++)
		{
			for(x=0; x<vweight[i].w; x++)
			{

				/*this code section must be the same as in aggregateV*/
				wp = vweight[i].g[y*vweight[i].w + x];
				wm = vweight[i].g[(y-delta)*vweight[i].w + x];

				/*compute true weight*/
				w0 = ((uint16_t)(0xFEFF))/(255 + wp + wm);
				wp = ((uint16_t)w0*(uint16_t)wp+0x80)>>8;
				wm = ((uint16_t)w0*(uint16_t)wm+0x80)>>8;

				/*store output*/
				iw0.g[y*vweight[i].w + x] = w0;
				iwp.g[y*vweight[i].w + x] = wp;
				iwm.g[y*vweight[i].w + x] = wm;
			}
		}

		/*store*/
		sprintf(string_filename, "out/w/vo%d.png", i);
		image_gray_savePng(string_filename, &iw0);
		sprintf(string_filename, "out/w/vp%d.png", i);
		image_gray_savePng(string_filename, &iwp);
		sprintf(string_filename, "out/w/vm%d.png", i);
		image_gray_savePng(string_filename, &iwm);

		image_gray_destroy(&iw0);
		image_gray_destroy(&iwp);
		image_gray_destroy(&iwm);
	}

	/*compute and store three matrix of hweight for each iteration*/
	for(i=0; i<(sizeof(deltas)/sizeof(deltas[0])); i++)
	{
		image_gray_init(&iw0, imr->w, imr->h);
		image_gray_init(&iwp, imr->w, imr->h);
		image_gray_init(&iwm, imr->w, imr->h);

		delta = deltas[i];

		/*compute, same formula used in aggregateH*/
		for(y=0; y<hweight[i].h; y++)
		{
			for(x=delta; x<(hweight[i].w-delta); x++)
			{

				/*this code section must be the same as in aggregateH*/
				wp = hweight[i].g[y*hweight[i].w + x];
				wm = hweight[i].g[y*hweight[i].w + x - delta];

				/*compute true weight*/
				w0 = ((uint16_t)(0xFEFF))/(255 + wp + wm);
				wp = ((uint16_t)w0*(uint16_t)wp+0x80)>>8;
				wm = ((uint16_t)w0*(uint16_t)wp+0x80)>>8;

				/*store output*/
				iw0.g[y*hweight[i].w + x] = w0;
				iwp.g[y*hweight[i].w + x] = wp;
				iwm.g[y*hweight[i].w + x] = wm;
			}
		}

		/*store*/
		sprintf(string_filename, "out/w/ho%d.png", i);
		image_gray_savePng(string_filename, &iw0);
		sprintf(string_filename, "out/w/hp%d.png", i);
		image_gray_savePng(string_filename, &iwp);
		sprintf(string_filename, "out/w/hm%d.png", i);
		image_gray_savePng(string_filename, &iwp);

		image_gray_destroy(&iw0);
		image_gray_destroy(&iwp);
		image_gray_destroy(&iwm);
	}
#endif /*DEBUG*/

	/*free weight*/
	for(i=0; i<niter; i++)
	{
		image_gray_destroy(&vweight[i]);
		image_gray_destroy(&hweight[i]);
	}
	/*free other buffer*/
	image_gray_destroy(&cost1);
	image_gray_destroy(&cost2);
	image_gray_destroy(&buffval);
	image_gray_destroy(&cenl);
	image_gray_destroy(&cenr);
	image_gray_destroy(&imgl);
	image_gray_destroy(&imgr);

}

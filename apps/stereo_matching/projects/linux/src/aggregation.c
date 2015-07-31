#include "stereomatch.h"
#include <stdio.h>

//#define APPR_NORM

/*!
 * \brief aggregate vertically a cost map.
 * \param height : height of image in pixels
 * \param width : width of image in pixels
 * \param delta : aggregation distance in pixels
 * \param cost_in : pointer to input cost (before vertical aggregation). Size
 *        must be (height*width).
 * \param weight : corresponding weight computed with compute_vweigth with the
 * 		  same delta. Size must be (height*width).
 * \param cost_out : pointer to output cost (after vertical aggregation). Size
 *        must be (height*width).
 */
void aggregateV(
	int height, int width,
	int delta,
	uint8_t* cost_in,
	uint8_t* weight,
	uint8_t* cost_out)
{
	int x,y;

	/*pointer to costs (cost - delta, cost, cost + delta)*/
	uint8_t *p_im, *p_i, *p_ip;
	/*pointer to weight (weight + delta, weight - delta)*/
	uint8_t *p_wp, *p_wm;
	uint8_t *p_out;

	/*weights values*/
	uint8_t wp, wm;
	/*inputs values*/
	uint8_t inp, in, inm;
	uint32_t num;	/*temp value for computing*/
#ifdef APPR_NORM
	uint16_t w0;
#else
	uint32_t den;
#endif
	/*main case*/
	p_im = cost_in;
	p_i = cost_in;


	p_wm = weight;
	p_wp = weight + width*delta;

	p_out = cost_out;
	/*border case, just copy*/
	for(y=0; y<delta; y++)
	{
		for(x=0; x<width; x++)
		{
			*p_out++ = *p_i++;
		}
	}

	p_ip = p_i + width*delta;

	for(y=delta; y<(height-delta); y++)
	{
		for(x=0; x<width; x++)
		{
			/*read inputs*/
			wp = *p_wp++;
			wm = *p_wm++;
			inp = *p_ip++;
			in = *p_i++;
			inm = *p_im++;

			/*compute*/
#ifdef APPR_NORM
			num = (uint16_t)wp*inp;
			num += (uint16_t)wm*inm;
			w0 = 1024 - wp - wm;
			num += (uint32_t)(in*w0);
			/*round result*/
			num = (num + 0x0200) >> 10;
#else
			num = (uint16_t)wp*inp;
			num += (uint16_t)wm*inm;
			num += (uint16_t)in << 8;

			den = wp;
			den += wm;
			den += 256;

			num += den >> 1;
			num = num/den;
#endif


			/*store output*/
			*p_out++ = num;
		}
	}

	/*border case, just copy*/
	for(y=height-delta; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			*p_out++ = *p_i++;
		}
	}
}


/*!
 * \brief aggregate horizontally a cost map.
 * \param height : height of image in pixels
 * \param width : width of image in pixels
 * \param delta : aggregation distance in pixels
 * \param cost_in : pointer to input cost (before horizontal aggregation). Size
 *        must be (height*width).
 * \param weight : corresponding weight computed with compute_hweigth with the
 * 		  same delta. Size must be (height*width).
 * \param cost_out : pointer to output cost (after horizontal aggregation). Size
 *        must be (height*width).
 */
void aggregateH(
	int height, int width,
	int delta,
	uint8_t* cost_in,
	uint8_t* weight,
	uint8_t* cost_out)
{
	int x,y;

	/*pointer to costs (cost - delta, cost, cost + delta)*/
	uint8_t *p_im, *p_i, *p_ip;
	/*pointer to weight (weight + delta, weight - delta)*/
	uint8_t *p_wp, *p_wm;
	uint8_t *p_out;

	/*weights values*/
	uint8_t wp, wm;
	/*inputs values*/
	uint8_t inp, in, inm;
	uint32_t num;	/*temp value for computing*/
	#ifdef APPR_NORM
		uint16_t w0;
	#else
		uint32_t den;
	#endif

	/*main case*/
	p_out = cost_out;
	for(y=0; y<height; y++)
	{

		p_i = cost_in + y*width;
		/*border case, just copy*/
		for(x=0; x<delta; x++)
		{
			*p_out++ = *p_i++;
		}

		p_im = cost_in + y*width;
		p_i = p_im + delta;
		p_ip = p_i + delta;

		p_wm = weight + y*width;
		p_wp = weight + y*width + delta;

		for(x=delta; x<(width-delta); x++)
		{
			/*read inputs*/
			wp = *p_wp++;
			wm = *p_wm++;
			inp = *p_ip++;
			in = *p_i++;
			inm = *p_im++;

			/*compute*/
#ifdef APPR_NORM
			num = (uint16_t)wp*inp;
			num += (uint16_t)wm*inm;
			w0 = 1024 - wp - wm;
			num += (uint32_t)(in*w0);
			/*round result*/
			num = (num + 0x0200) >> 10;
#else
			num = (uint16_t)wp*inp;
			num += (uint16_t)wm*inm;
			num += (uint16_t)in << 8;

			den = wp;
			den += wm;
			den += 256;

			num += den >> 1;
			num = num/den;
#endif
			/*store output*/
			*p_out++ = num;
		}

		/*right border case, just copy*/
		for(x=width-delta; x<width; x++)
		{
			*p_out++ = *p_i++;
		}
	}
}

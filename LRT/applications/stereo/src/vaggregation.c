#include "stereomatch.h"

#include <string.h>

#include <c6x.h>

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
	uint8_t* restrict cost_in,
	uint8_t* restrict weight,
	uint8_t* restrict cost_out)
{
	_nassert((int) height % 2 == 0);
	_nassert((int) width % 8 == 0);

	int i;
	int delta_width = width*delta;
	int loop_len;



	/*pointer to cost*/
	uint8_t* restrict p_i;
	/*pointer to weight*/
	uint8_t* restrict p_wm;
	/*output pointer*/
	uint8_t* restrict p_out;

	/*weights values (8bits*4)*/
	unsigned long long wp, wm, w0;
	/*inputs values (8bits*4) */
	unsigned long long inp, in, inm;
	/*computing values*/
	__x128_t  temp, acc;
	unsigned long long out;

	/*border case, just copy*/
	p_out = cost_out + delta_width;
	p_i = cost_in + delta_width;
	p_wm = weight;

	/*number of loop iteration*/
	loop_len = width*(height - 2*delta)/8;

	/*main loop*/
#pragma MUST_ITERATE(32, ,2)
//#pragma UNROLL(2)
	for(i=0; i<loop_len; i++)
	{
		/*read weight*/
		wp = _amem8(p_wm + delta_width);
		wm = _amem8(p_wm);
		p_wm += 8;

		/*read inputs*/
		inp = _amem8(p_i+delta_width);
		inm = _amem8(p_i-delta_width);
		in = _amem8(p_i);
		p_i += 8;

		/*compute*/
		/*
		 * no simd code version (with 8 bits)
		num = (uint16_t)wp*inp;
		num += (uint16_t)wm*inm;
		w0 = 1024 - wp - wm;
		num += (uint32_t)(in*w0);
		//round result
		num = (num + 0x0200) >> 10;
		*/

		acc = _dmpyu4(wp, inp);	/*wp*inp*/
		temp = _dmpyu4(wm, inm);/*wm*inm*/

		/*acc = acc + temp (8*16bits)*/
		acc = _llto128(
			_dadd2(_hi128(acc), _hi128(temp)),
			_dadd2(_lo128(acc), _lo128(temp)));

		/*wo = 255 - (wm+wp)*/
		w0 = _itoll(
			_sub4(0xFFFFFFFF, _add4(_hill(wp), _hill(wm))),
			_sub4(0xFFFFFFFF, _add4(_loll(wp), _loll(wm)))
			);

		temp = _dmpyu4(w0, in); /*w0*in*/
		/*acc = acc + temp (8*16bits)*/
		acc = _llto128(_dadd2(_hi128(acc), _hi128(temp)), _dadd2(_lo128(acc), _lo128(temp)));

		/*takes the higher bits*/
		out = _itoll(
			_packh4(_get32_128(acc, 3), _get32_128(acc, 2)),
			_packh4(_get32_128(acc, 1), _get32_128(acc, 0))
		);

		/*store output*/
		_amem8(p_out) = out;
		p_out+=8;
	}

	/*border case, just copy*/
	memcpy(cost_out, cost_in, delta*width);
	memcpy(cost_out + (height-delta)*width, cost_in + (height-delta)*width, delta*width);
}

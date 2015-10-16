#include "stereomatch.h"

#include <c6x.h>

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
	uint8_t* restrict cost_in,
	uint8_t* restrict weight,
	uint8_t* restrict cost_out)
{
	int x,y, idx, loop_len;

	/*pointer to costs (cost - delta, cost, cost + delta)*/
	uint8_t * restrict p_i;
	/*pointer to weight (weight + delta, weight - delta)*/
	uint8_t * restrict p_wm;
	uint8_t * restrict p_out;

	/*weights values (8bits*4)*/
	unsigned long long wp, wm, w0;
	/*inputs values (8bits*4) */
	unsigned long long inp, in, inm;
	/*computing values*/
	__x128_t  temp, acc;
	unsigned long long out;

	/*main case*/
	p_out = cost_out + delta;
	p_i = cost_in + delta;
	p_wm = weight;

	loop_len = (height*width - 2*delta + 7) / 8;

	for(y=0; y<loop_len; y++)
	{
		/*read weight*/
		wp = _mem8(p_wm + delta);
		wm = _amem8(p_wm);
		p_wm += 8;

		/*read inputs*/
		inp = _mem8(p_i+delta);
		inm = _amem8(p_i-delta);
		in = _mem8(p_i);
		p_i += 8;

		/*compute*/
		/*acc = (uint16_t)wp*inp;
		acc += (uint16_t)wm*inm;
		w0 = 1024 - wp - wm;
		acc += (uint32_t)(in*w0);
		//round result
		acc = (acc + 0x0200) >> 10;
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
		_mem8(p_out) = out;
		p_out += 8;
	}

	/*upper left*/
	for(x=0; x<delta; x++)
	{
		cost_out[x] = cost_in[x];
	}

	/*mid right+left (in one iter)*/
	for(y=1; y<height; y++)
	{
		idx = y*width - delta;
		for(x=0; x<(2*delta); x++)
		{
			cost_out[idx + x] = cost_in[idx + x];
		}
	}

	/*lower right*/
	idx = height*width - delta;
	for(x=0; x<delta; x++)
	{
		cost_out[idx + x] = cost_in[idx + x];
	}
}

#include "stereomatch.h"

#define THR 20
#define PCEN 120
#define PCP  120
/*
 * LUT to compute census,
 * For each index contains the number of bits to one.
 * For instance cenLUT[9] = 2 because 9 = 0b00001001 and has two bit to one.
 * The factor is directly included inside LUT (PCEN/8)
 */
/*
 * generated with (python) :
 * ',\n'.join([', '.join(['{}*(PCEN/8)'.format(np.sum([v/(2**i)%2 for i in range(0, 8)])) for v in range(k, k+8)]) for k in range(0, 256, 8)])
 */
static const uint8_t cenLUT[256] =
{
		0*(PCEN/8), 1*(PCEN/8), 1*(PCEN/8), 2*(PCEN/8), 1*(PCEN/8), 2*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8),
		1*(PCEN/8), 2*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8),
		1*(PCEN/8), 2*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		1*(PCEN/8), 2*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		1*(PCEN/8), 2*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 6*(PCEN/8), 7*(PCEN/8),
		1*(PCEN/8), 2*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 6*(PCEN/8), 7*(PCEN/8),
		2*(PCEN/8), 3*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 6*(PCEN/8), 7*(PCEN/8),
		3*(PCEN/8), 4*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8),
		4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 6*(PCEN/8), 7*(PCEN/8),
		4*(PCEN/8), 5*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 5*(PCEN/8), 6*(PCEN/8), 6*(PCEN/8), 7*(PCEN/8),
		5*(PCEN/8), 6*(PCEN/8), 6*(PCEN/8), 7*(PCEN/8), 6*(PCEN/8), 7*(PCEN/8), 7*(PCEN/8), 8*(PCEN/8)
};

/*!
 * \brief compute cost of images with a specific disparity level
 * \param height : height of image in pixels
 * \param width : width of image in pixels
 * \param disp : disparity level associated with the cost
 * \param iml_g : input left image in gray level. Size must be (height*width).
 * \param imr_g : input right image in gray level. Size must be (height*width).
 * \param cenl : census value left image, computed with census. Size must be (height*width).
 * \param cenr : census value right image, computed with census. Size must be (height*width).
 * \param out : Pointer to output cost map. Size must be (height*width).
 */
void cost_construction(
		/*images constants*/
		int heigth, int width,
		/*input param*/
		int disp,
		/*inputs images*/
		const uint8_t* iml_g,
		const uint8_t* imr_g,
		const uint8_t* cenl,
		const uint8_t* cenr,
		/*outputs images*/
		uint8_t* out)
{
	/*loop indices*/
	int x, y;

	/*pointer to walk on buffers*/
	const uint8_t* ptr_l = iml_g;
	const uint8_t* ptr_r = imr_g;
	const uint8_t* ptr_cl = cenl;
	const uint8_t* ptr_cr = cenr;
	uint8_t* ptr_o = out;

	/*data for computing*/
	uint8_t l, r;
	uint8_t temp;
	uint8_t curcen, cl, cr;

	for(y=0; y<heigth; y++)
	{
		l = *ptr_l;
		cl = *ptr_cl;
		/*left pointer doesn't move  for the firsts elements*/

		/*compute border case*/
		for(x=0; x<disp; x++)
		{
			/*load inputs*/
			r = *ptr_r++;
			cr = *ptr_cr++;

			/*compute*/
			temp = (r>l)?(r-l):(l-r);		/*absolute difference*/
			temp = (temp > THR)?THR:temp;	/*truncation*/
			curcen = cenLUT[(cl^cr)];		/*compare census and count differences*/
			/*Aggregates two costs*/
			temp = temp*(PCP/THR) + curcen;

			/*dynamic adaptation and store result*/
			*ptr_o++ = temp;
		}

		/*compute main loop*/
		for(x=disp; x<width; x++)
		{
			/*load inputs*/
			l = *ptr_l++;
			r = *ptr_r++;
			cl = *ptr_cl++;
			cr = *ptr_cr++;

			/*compute*/
			temp = (r>l)?(r-l):(l-r);		/*absolute difference*/
			temp = (temp > THR)?THR:temp;	/*truncation*/
			curcen = cenLUT[(cl^cr)];		/*compare census and count differences*/
			/*Aggregates two costs*/
			temp = temp*(PCP/THR) + curcen;

			/*dynamic adaptation and store result*/
			*ptr_o++ = temp;
		}

		/*readjusts left image pointer to point on start of next line*/
		ptr_l += disp;
		ptr_cl += disp;
	}
}

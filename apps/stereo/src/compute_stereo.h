/*
 * compute_stereo.h
 *
 *  Created on: 19 févr. 2014
 *      Author: jmenant
 */

#ifndef COMPUTE_STEREO_H_
#define COMPUTE_STEREO_H_

#include "../image/image.h"


void stereoMatch(
	struct str_image_rgb* imr,
	struct str_image_rgb* iml,
	int ndisp,
	struct str_image_gray* out);

#endif /* COMPUTE_STEREO_H_ */

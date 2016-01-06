/*
 * stereomatch.h
 *
 *  Created on: 18 févr. 2014
 *      Author: jmenant
 */

#ifndef STEREOMATCH_H_
#define STEREOMATCH_H_

#include <stdint.h>

void cost_construction(
	int heigth, int width,
	int disp,
	const uint8_t* iml_g,
	const uint8_t* imr_g,
	const uint8_t* cenl,
	const uint8_t* cenr,
	uint8_t* out);

void compute_vweigth(
	int heigth, int width,
	int delta,
	const uint8_t* imrgb,
	uint8_t* out);

void compute_hweigth(
	int heigth, int width,
	int delta,
	const uint8_t* imrgb,
	uint8_t* out);

void census(
	int height, int width,
	uint8_t* image,
	uint8_t* out);

void aggregateV(
	int height, int width,
	int delta,
	uint8_t* cost_in,
	uint8_t* weight,
	uint8_t* cost_out);

void aggregateH(
	int height, int width,
	int delta,
	uint8_t* cost_in,
	uint8_t* weight,
	uint8_t* cost_out);

#endif /* STEREOMATCH_H_ */

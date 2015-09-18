/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#ifndef ACTORS_H_
#define ACTORS_H_

#include <spider.h>
#include <stdint.h>

#define IN
#define OUT

void Config(OUT Param* nSlice, OUT Param* truncValue, OUT Param* scale, OUT Param* nIter, OUT Param* sizeFilter, OUT Param* minDisp, OUT Param* maxDisp, OUT Param* height, OUT Param* width);

void Camera(Param height, Param width, OUT uint8_t* rgb_L, OUT uint8_t* rgb_R);
void RGB2Gray(Param size, IN uint8_t* rgb, OUT uint8_t* gray);
void Census(Param height, Param width, uint8_t* gray, OUT uint8_t* cen);
void Split(Param nSlice, Param sizeFilter, Param height, Param width, uint8_t* in, OUT uint8_t* out);
void MedianFilter(Param height, Param width, Param sizeFilter, uint8_t* in, OUT uint8_t* out);
void Display(Param height, Param width, uint8_t* rgb, uint8_t* depth);

void GenIx(Param nIter, OUT uint8_t* ixs);
void DisparityGen(Param maxDisp, Param minDisp, OUT uint8_t* dispIxs);
void HWeights(Param height, Param width, uint8_t* offset, uint8_t* rgbL, OUT uint8_t* out);
void VWeights(Param height, Param width, uint8_t* offset, uint8_t* rgbL, OUT uint8_t* out);
void CostConstruction(Param height, Param width, Param truncValue, uint8_t* grayL, uint8_t* grayR, uint8_t* cenL, uint8_t* cenR, uint8_t* disp, OUT uint8_t* error);
void AggregateCost(Param height, Param width, Param nIter, uint8_t* offsets, uint8_t* vWeights, uint8_t* hWeights, uint8_t* cost, OUT uint8_t* disp);
void DisparitySelect(Param height, Param width, Param scale, Param minDisp, Param maxDisp, uint8_t* dispVal, uint8_t* dispIx, uint8_t* curDisp, uint8_t* curCost, OUT uint8_t* disp, OUT uint8_t* cost);

#endif /* ACTORS_H_ */

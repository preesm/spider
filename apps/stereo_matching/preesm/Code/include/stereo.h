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

#ifndef STEREO_H_
#define STEREO_H_

#include <spider.h>

void Config(OUT Param nSlice, OUT Param truncValue, OUT Param scale, OUT Param nIter, OUT Param sizeFilter, OUT Param minDisp, OUT Param maxDisp, OUT Param height, OUT Param width);

void Camera(Param height, Param width, OUT char* rgb_L, OUT char* rgb_R);
void RGB2Gray(Param size, IN char* rgb, OUT char* gray);
void Census(Param height, Param width, char* gray, OUT char* cen);
void Split(Param nSlice, Param sizeFilter, Param height, Param width, char* in, OUT char* out);
void MedianFilter(Param height, Param width, Param sizeFilter, char* in, OUT char* out);
void Display(Param height, Param width, char* rgb, char* depth);

void GenIx(Param nIter, OUT int* ixs);
void DisparityGen(Param maxDisp, Param minDisp, OUT int* dispIxs);
void HWeights(Param height, Param width, char* offset, char* rgbL, OUT char* out);
void VWeights(Param height, Param width, char* offset, char* rgbL, OUT char* out);
void CostConstruction(Param height, Param width, Param truncValue, char* grayL, char* grayR, char* cenL, char* cenR, char* disp, OUT char* error);
void AggregateCost(Param height, Param width, Param nIter, char* offsets, char* vWeights, char* hWeights, char* cost, OUT char* disp);
void DisparitySelect(Param height, Param width, Param scale, Param minDisp, Param maxDisp, char* dispVal, char* dispIx, char* curDisp, char* curCost, OUT char* disp, OUT char* cost);

#endif /* STEREO_H_ */

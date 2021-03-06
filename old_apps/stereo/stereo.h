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

#include <platform_types.h>
#include <spider.h>

void config(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);

void file(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void cam(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void mixInput(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void stereoMono(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void writeFile(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void netDisplay(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void display(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void rgb2Gray(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void census(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void genDelta(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void compWeight(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void disp(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void costConst(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void aggregate(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void select(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void genDisp(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void medianSlice(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void split(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);
void null(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]);

PiSDFGraph* initPisdf_stereo(PiSDFGraph* _graphs);

#endif /* STEREO_H_ */

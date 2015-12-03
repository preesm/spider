/**
 * *****************************************************************************
 * Copyright or © or Copr. IETR/INSA: Maxime Pelcat, Jean-François Nezan,
 * Karol Desnos, Julien Heulot, Clément Guy, Yaset Oliva Venegas
 *
 * [mpelcat,jnezan,kdesnos,jheulot,cguy,yoliva]@insa-rennes.fr
 *
 * This software is a computer program whose purpose is to prototype
 * parallel applications.
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-C license and that you accept its terms.
 * ****************************************************************************
 */

#ifndef STEREO_H
#define STEREO_H

#include <spider.h>

#define N_FCT_STEREO 16
extern lrtFct stereo_fcts[N_FCT_STEREO];

void init_stereo();
void free_stereo();

typedef enum{
	CORE_CORE0 = 0,
} PE;

typedef enum{
	CORE_TYPE_X86 = 0,
} PEType;

typedef enum{
	STEREO_CONFIG_FCT = 0,
	STEREO_CAMERA_FCT = 1,
	STEREO_RGB2GRAY_L_FCT = 2,
	STEREO_RGB2GRAY_R_FCT = 3,
	STEREO_CENSUS_L_FCT = 4,
	STEREO_CENSUS_R_FCT = 5,
	STEREO_SPLIT_FCT = 6,
	STEREO_MEDIANFILTER_FCT = 7,
	STEREO_DISPLAY_FCT = 8,
	COSTPARALLEL_GENIX_FCT = 9,
	COSTPARALLEL_GENDISP_FCT = 10,
	COSTPARALLEL_VWEIGHTS_FCT = 11,
	COSTPARALLEL_HWEIGHT_FCT = 12,
	COSTPARALLEL_COSTCONSTRUCTION_FCT = 13,
	DISPCOMP_AGGREGATECOST_FCT = 14,
	DISPCOMP_DISPARITYSELECT_FCT = 15,
} FctIxs;

#endif//STEREO_H

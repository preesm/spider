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

#ifndef TOP_HCLM_H
#define TOP_HCLM_H

#include <lrt.h>

#define N_FCT_TOP_HCLM 8
extern lrtFct top_hclm_fcts[N_FCT_TOP_HCLM];

#define N_FCT_TOP_HCLM_OPT 8
extern lrtFct top_hclm_fcts[N_FCT_TOP_HCLM_OPT];

typedef enum{
	TOP_HCLM_CFG_N_FCT = 0,
	TOP_HCLM_SRC_FCT = 1,
//	TOP_HCLM_END_FCT = 2,
	TOP_HCLM_SNK_FCT = 3,
	FIR_CHAN_CFG_M_FCT = 4,
	FIR_CHAN_INITSW_FCT = 5,
	FIR_CHAN_SWITCH_FCT = 6,
	FIR_CHAN_FIR_FCT = 7,
} FctIxs;

#endif//TOP_HCLM_H

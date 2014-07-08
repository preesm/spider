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

#include <ti/csl/csl_cache.h>
#include <ti/csl/csl_cacheAux.h>
#include <ti/csl/csl_chipAux.h>
#include <ti/csl/csl_xmc.h>
#include <ti/csl/csl_xmcAux.h>
#include <ti/csl/cslr_cgem.h>

#include <platform.h>
#include <platform_types.h>
#include "semaphore.h"
#include "memoryAlloc.h"

void platform_file_init();
void platform_time_init();
void platform_queue_Init(UINT8 nbSlaves);

void set_MPAX(int index, Uint32 bAddr, Uint32 rAddr, Uint8 segSize){

    CSL_XMC_XMPAXH mpaxh;
    mpaxh.bAddr = bAddr;
    mpaxh.segSize =  segSize;

    CSL_XMC_XMPAXL mpaxl;
    mpaxl.rAddr = rAddr;
    mpaxl.sr = 1;
    mpaxl.sw = 1;
    mpaxl.sx = 1;
    mpaxl.ur = 1;
    mpaxl.uw = 1;
    mpaxl.ux = 1;

    CSL_XMC_setXMPAXH(index, &mpaxh);
    CSL_XMC_setXMPAXL(index, &mpaxl);
}

void reset_MPAX(int index){

    CSL_XMC_XMPAXH mpaxh;
    mpaxh.bAddr = 0;
    mpaxh.segSize =  0;

    CSL_XMC_XMPAXL mpaxl;
    mpaxl.rAddr = 0;
    mpaxl.sr = 0;
    mpaxl.sw = 0;
    mpaxl.sx = 0;
    mpaxl.ur = 0;
    mpaxl.uw = 0;
    mpaxl.ux = 0;

    CSL_XMC_setXMPAXH(index, &mpaxh);
    CSL_XMC_setXMPAXL(index, &mpaxl);
}

void platform_init(UINT8 nbSlaves){
	mutex_pend(MUTEX_CACHE);

	for(int i=0; i<256; i++){
		CACHE_disableCaching(i);
//		CACHE_resetPrefetch(i);
	}

	set_MPAX(0, 0x00000, 0x00000, 0x1E);
	set_MPAX(1, 0x80000, 0x80000, 0x1E);
	for(int i=2; i<16; i++)
		reset_MPAX(i);

	CACHE_setL1PSize(CACHE_L1_32KCACHE);
	CACHE_setL1DSize(CACHE_L1_32KCACHE);
	CACHE_setL2Size (CACHE_0KCACHE);

#ifndef ENABLE_CACHE
	set_MPAX(3, 0xa0000, 0x0c000, 0x15); // "translate" 256KB (0x11) from 0xa1000000 to 0x00c000000 using the MPAX number 3
#endif
	mutex_post(MUTEX_CACHE);

	platform_file_init();
	platform_time_init();
	platform_queue_Init(nbSlaves);
}

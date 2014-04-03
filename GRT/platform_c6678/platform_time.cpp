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

#include <platform_time.h>

#include <ti/csl/csl_tmr.h>
#include <ti/csl/csl_tmrAux.h>
#include <ti/csl/csl_chipAux.h>

#include <stdio.h>

void timer_start();
void timer_reset();


void platform_time_init(){
	CSL_TmrContext context;
	CSL_TmrHwSetup hwSetup;
	CSL_TmrParam param;
	CSL_TmrObj object;
	CSL_Status status;

	CSL_tmrInit(&context);

	CSL_TmrHandle timer_hdl = CSL_tmrOpen(&object, 0, &param, &status);

	hwSetup.tmrTimerPeriodLo		= 0xFFFF;
	hwSetup.tmrTimerPeriodHi		= 0xFFFF;
	hwSetup.tmrTimerCounterLo		= 0x0;
	hwSetup.tmrTimerCounterHi		= 0x0;
	hwSetup.tmrIpGateHi				= CSL_TMR_CLOCK_INP_NOGATE;
	hwSetup.tmrClksrcHi				= CSL_TMR_CLKSRC_INTERNAL;
	hwSetup.tmrIpGateLo				= CSL_TMR_CLOCK_INP_NOGATE;
	hwSetup.tmrClksrcLo				= CSL_TMR_CLKSRC_INTERNAL;
	hwSetup.tmrPreScalarCounterHi	= 1;
	hwSetup.tmrTimerMode			= CSL_TMR_TIMMODE_GPT;
	CSL_tmrHwSetup(timer_hdl, &hwSetup);

	CSL_tmrClose(timer_hdl);
	timer_start();
}

void timer_reset(){
	CSL_TmrParam param;
	CSL_TmrObj object;
	CSL_TmrHwSetup hwSetup;
	CSL_Status status;

	CSL_TmrHandle timer_hdl = CSL_tmrOpen(&object, 0, &param, &status);

		hwSetup.tmrTimerPeriodLo		= 0xFFFF;
		hwSetup.tmrTimerPeriodHi		= 0xFFFF;
		hwSetup.tmrTimerCounterLo		= 0x0;
		hwSetup.tmrTimerCounterHi		= 0x0;
		hwSetup.tmrIpGateHi				= CSL_TMR_CLOCK_INP_NOGATE;
		hwSetup.tmrClksrcHi				= CSL_TMR_CLKSRC_INTERNAL;
		hwSetup.tmrIpGateLo				= CSL_TMR_CLOCK_INP_NOGATE;
		hwSetup.tmrClksrcLo				= CSL_TMR_CLKSRC_INTERNAL;
		hwSetup.tmrPreScalarCounterHi	= 1;
		hwSetup.tmrTimerMode			= CSL_TMR_TIMMODE_GPT;
		CSL_tmrHwSetup(timer_hdl, &hwSetup);

		CSL_tmrClose(timer_hdl);
}

void timer_start(){
	CSL_TmrParam param;
	CSL_TmrObj object;
	CSL_Status status;
//    CSL_TmrEnamode TimeCountMode = CSL_TMR_ENAMODE_ENABLE;

	CSL_TmrHandle timer_hdl = CSL_tmrOpen(&object, 0, &param, &status);
	CSL_TmrStart64(timer_hdl, CSL_TMR_ENAMODE_ENABLE);
//    CSL_tmrHwControl(timer_hdl, CSL_TMR_CMD_START64, (void *)&TimeCountMode);
    CSL_tmrClose(timer_hdl);
}

void timer_stop(){
	CSL_TmrParam param;
	CSL_TmrObj object;
	CSL_Status status;
//    CSL_TmrEnamode TimeCountMode = CSL_TMR_ENAMODE_ENABLE;

	CSL_TmrHandle timer_hdl = CSL_tmrOpen(&object, 0, &param, &status);
	CSL_TmrStop64(timer_hdl);
//    CSL_tmrHwControl(timer_hdl, CSL_TMR_CMD_STOP64, (void *)&TimeCountMode);
    CSL_tmrClose(timer_hdl);
}

void platform_time_reset(){
	timer_stop();
	timer_reset();
	timer_start();
}

UINT32 platform_time_getValue(){
	CSL_Uint64 res;
	CSL_TmrParam param;
	CSL_TmrObj object;
	CSL_Status status;

	CSL_TmrHandle timer_hand = CSL_tmrOpen(&object, 0, &param, &status);

	res = object.regs->CNTHI;
	res = res << 32;
	res += object.regs->CNTLO;

	CSL_tmrClose(timer_hand);

	return res;
}

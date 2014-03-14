
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#include "Timer.h"

#ifdef ZYNQ
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <linux/types.h>
#include <sys/ioctl.h>

#include <timerArm_module/timerArm.h>
#include <sys/time.h>

#include "SchedulingError.h"

#else
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#endif

Timer timer;

Timer::Timer() {
#ifdef ZYNQ
	file = open("/dev/timerArm", O_RDWR, S_IRUSR | S_IWUSR);
	if(file == -1){
		fprintf(stderr, "Cannot open /dev/timerArm\n");
		exitWithCode(1046);
	}
#endif
}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

unsigned int Timer::getValue(){
	unsigned int time;
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_GETVALUE, &time);
#else
	timeval end, val;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &val);
    time = (val.tv_sec*1000000 + val.tv_usec)*667;
#endif
	return time;
}

void Timer::resetAndStart(){
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_RESET);
	ioctl(file,TIMERARM_IOCT_START);
#else
    gettimeofday(&start, NULL);
#endif
}

unsigned int  Timer::print(const char* txt){
	unsigned int time;
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_STOP);
	ioctl(file,TIMERARM_IOCT_GETVALUE, &time);
#else
	timeval end, val;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &val);
    time = (val.tv_sec*1000000 + val.tv_usec)*667;
#endif
	printf("%s: %d\t(%fms)\n", txt, time, time/667000.0);
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_START);
#endif
	return time;
}

unsigned int Timer::getValueAndReset(){
	unsigned int time;
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_STOP);
	ioctl(file,TIMERARM_IOCT_GETVALUE, &time);
	ioctl(file,TIMERARM_IOCT_RESET);
	ioctl(file,TIMERARM_IOCT_START);
#else
	timeval end, val;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &val);
    time = (val.tv_sec*1000000 + val.tv_usec)*667;
    gettimeofday(&start, NULL);
#endif
	return time;
}

unsigned int Timer::printAndReset(const char* txt){
	unsigned int time;
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_STOP);
	ioctl(file,TIMERARM_IOCT_GETVALUE, &time);
#else
	timeval end, val;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &val);
    time = (val.tv_sec*1000000 + val.tv_usec)*667;
#endif
	printf("%s: %d\t(%fms)\n", txt, time, time/667000.0);
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_RESET);
	ioctl(file,TIMERARM_IOCT_START);
#else
    gettimeofday(&start, NULL);
#endif
	return time;
}


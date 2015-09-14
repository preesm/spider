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

#include "platformLinux.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <sched.h>

#define PLATFORM_FPRINTF_BUFFERSIZE 200
#define SHARED_MEM_KEY		8452 //0x2104

static struct timespec start;
static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];

PlatformLinux::PlatformLinux(){
	if(platform_)
		throw "Try to create 2 platforms";

	platform_ = this;

	this->rstTime();
}

PlatformLinux::~PlatformLinux(){
}

/** File Handling */
int PlatformLinux::fopen(const char* name){
	return open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP| S_IROTH | S_IWOTH);
}

void PlatformLinux::fprintf(int id, const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);
	if(n >= PLATFORM_FPRINTF_BUFFERSIZE){
		printf("PLATFORM_FPRINTF_BUFFERSIZE too small\n");
	}
	write(id, buffer, n);
}
void PlatformLinux::fclose(int id){
	close(id);
}

/** Time Handling */
void PlatformLinux::rstTime(){
	clock_gettime(CLOCK_MONOTONIC, &start);
}

Time PlatformLinux::getTime(){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	long long val = ts.tv_sec - start.tv_sec;
	val *= 1000000000;
	val += ts.tv_nsec - start.tv_nsec;
	return val;
}

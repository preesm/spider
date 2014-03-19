#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
//#include <sys/mman.h>

#include <fcntl.h>
//#include <sys/time.h>

#include "types.h"
//#include "../lrt/lrt.h"

#define DDR_BASE 	0x10000000
#define DDR_LENGTH 	0x10000000
#define DDR_FILE	"/dev/mem"

static void* ddr_address;
static int f;


void addMboxMem(){

}

void addShMem(){

}

void mboxMemInit(){

}

void shMemInit(UINT32 cpuId) {

}

void releaseMboxMemMx(){

}

void OS_ShMemInit() {
//	int f;

//	if (!(f=open(DDR_FILE,O_RDWR | O_SYNC))) {
//		fprintf(stderr, "%s: %s\n", DDR_FILE, strerror(errno));
//		exit(1);
//	}
//
//	ddr_address=mmap(NULL, DDR_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, f, (int)DDR_BASE);
//
//	if (ddr_address == (void *)-1) {
//		fprintf(stderr,"mmap(): %s\n",strerror(errno));
//		exit(1);
//	}
//	close(f);
}

UINT32 OS_ShMemRead(UINT32 address, void* data, UINT32 size) {
	void* start = ddr_address+(int)address-(int)DDR_BASE;
	if (DDR_BASE <= address
			&& DDR_BASE + DDR_LENGTH > address
			&& DDR_BASE <= address + size
			&& DDR_BASE + DDR_LENGTH > address + size) {
//		tmp = msync((void*)((int)start&(~4047)),4048, MS_INVALIDATE);
//		if (tmp == -1) {
//			fprintf(stderr,"msync(): %s\n",strerror(errno));
//			exit(1);
//		}
		memcpy(data,start, size);
//		lseek(f, address, SEEK_SET);
//		read(f, data, size);
		return size;
	}else
		printf("Memory not found 0x%x size 0x%x\n", (int)address, (int) size);
	return 0;
}

UINT32 OS_ShMemWrite(UINT32 address, void* data, UINT32 size) {
	void* start = ddr_address+(int)address-(int)DDR_BASE;
	if (DDR_BASE <= address
			&& DDR_BASE + DDR_LENGTH > address
			&& DDR_BASE <= address + size
			&& DDR_BASE + DDR_LENGTH > address + size) {

//		tmp = msync((void*)((int)start&(~4047)),4048, MS_SYNC);
//		if (tmp == -1) {
//			fprintf(stderr,"msync(): %s\n",strerror(errno));
//			exit(1);
//		}

				memcpy(start,data, size);
//		lseek(f, address, SEEK_SET);
//		write(f, data, size);
		return size;
	}else
		printf("Memory not found 0x%x size 0x%x\n", (int)address, (int) size);
	return 0;
}

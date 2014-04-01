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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

extern "C"{
#include <ti/platform/platform.h>
}

#include <platform_file.h>
#include <platform_types.h>

#define BUFFER_SIZE 100

static UINT8 crc=0;
static char buffer[BUFFER_SIZE];

void platform_file_init(){
    platform_init_flags  init_flags;
    platform_init_config init_config;

    init_flags.pll		= 1;
    init_flags.ddr		= 0;
	init_flags.tcsl		= 0;
	init_flags.phy		= 0;
	init_flags.ecc		= 0;
    init_config.pllm    = 0;

    if (platform_init(&init_flags, &init_config) != Platform_EOK) {
    	printf("Platform failed to initialize, errno = 0x%x \n", platform_errno);
    }

    printf("init\n");

    platform_uart_init();
    platform_uart_set_baudrate(115200);
}

void platform_fopen(const char* name){
	Uint8 idx = 0;
	crc = 0;
    platform_uart_write('\1');	// SOH

    /* Send name of file */
    while(name[idx] != '\0'){
		platform_uart_write(name[idx]);
		crc ^= name[idx++];
    }

    platform_uart_write('\2');	// STX
}

void platform_fprintf(const char* fmt, ...){
	Uint8 idx = 0;
	va_list ap;
	va_start(ap, fmt);

	idx = vsnprintf(buffer, BUFFER_SIZE, fmt, ap);
	if(idx > BUFFER_SIZE){
		printf("BUFFER_SIZE too small\n");
		abort();
	}

	idx=0;
	while(buffer[idx] != '\0'){
		platform_uart_write(buffer[idx]);
		crc ^= buffer[idx++];
	}
}

void platform_fclose(){
	Uint8 idx = 0;

	platform_uart_write('\3');	// ETX
	platform_uart_write(crc);

	if (platform_uart_read(&idx, 10000000) != Platform_EOK){
		printf("Error in file transmission : Response timeout\n");
		abort();
	}

	if(idx != '\6'){  // ACK
		printf("Error in file transmission : bad ACK\n");
	}

	platform_uart_write('\4');	// EOT
}

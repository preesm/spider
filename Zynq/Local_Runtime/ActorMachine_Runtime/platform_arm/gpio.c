/*
 * gpio.c
 *
 *  Created on: Sep 2, 2013
 *      Author: jheulot
 */

#include "gpio.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int fled;

void initGpio(){
	// Open the export file and write the PSGPIO number for LD9 (MIO 7)
	// to the export property, then close the file
	int fexport = open("/sys/class/gpio/export", O_WRONLY);
	if (fexport == -1){
		printf("Error opening /sys/class/gpio/export node\n");
		exit(-1);
	}
	write(fexport, "7", 2*sizeof(char));
	close(fexport);

	// Set the direction of LD9 (MIO 7)
	int fdir = open("/sys/class/gpio/gpio7/direction", O_WRONLY);
	if (fdir == -1){
		printf("Error opening /sys/class/gpio/gpio7/direction node\n");
		exit(-1);
	}
	write(fdir, "out", 4*sizeof(char));
	close(fdir);

	fled = open("/sys/class/gpio/gpio7/value", O_WRONLY);
	if (fdir == -1){
		printf("Error opening /sys/class/gpio/gpio7/value node\n");
		exit(-1);
	}
}

void setLed(BOOLEAN b){
	b ? write(fled, "1", 2*sizeof(char)) : write(fled, "0", 2*sizeof(char));
}

BOOLEAN getSw(){
	return TRUE;
//    XGpio_DiscreteRead(&gpio_leds, 1, b & 0x01);
}

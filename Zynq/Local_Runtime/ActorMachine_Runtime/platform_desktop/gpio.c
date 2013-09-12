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


void initGpio(){

}

void setLed(BOOLEAN b){

}

BOOLEAN getSw(){
	return TRUE;
//    XGpio_DiscreteRead(&gpio_leds, 1, b & 0x01);
}

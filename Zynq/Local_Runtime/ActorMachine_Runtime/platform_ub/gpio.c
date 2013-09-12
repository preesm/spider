/*
 * gpio.c
 *
 *  Created on: Sep 2, 2013
 *      Author: jheulot
 */

#include "gpio.h"
//#include <xgpio.h>
#include "platform.h"

//static XGpio gpio_leds;

void initGpio(){
	*(UINT32*)(GPIO_ADDR+0x04) = 0xFFFFFFFF;
	*(UINT32*)(GPIO_ADDR+0x0C) = 0xFFFFFFFE;
}

void setLed(BOOLEAN b){
	*(UINT32*)(GPIO_ADDR+0x08) = b?1:0;
}

BOOLEAN getSw(){
	return (*(UINT32*)(GPIO_ADDR+0x00))&0x1;
//    XGpio_DiscreteRead(&gpio_leds, 1, b & 0x01);
}

/*
 * ChipInit.h
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#ifndef CHIPINIT_H_
#define CHIPINIT_H_

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "PinDrivers.h"
#include "IoLines.h"
#include "macros.h"

void IO_config(void);
void UART_config(void);
void SPI_config(void);
void watchdog_init(void);
void counter0_init(uint8_t prescaler);
void config_chip(void);



#endif /* CHIPINIT_H_ */

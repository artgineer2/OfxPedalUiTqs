/*
 * utilityFuncts.h
 *
 *  Created on: Mar 21, 2016
 *      Author: mike
 */

#ifndef UTILITYFUNCTS_H_
#define UTILITYFUNCTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void delay(unsigned long delay);
void fillBuffer(char *buffer, char *string, uint8_t length);
void zero2Space(char* buffer, uint16_t length);

#endif /* UTILITYFUNCTS_H_ */

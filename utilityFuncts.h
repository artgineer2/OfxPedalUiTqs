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
void clearBuffer(char *buffer, int length);
void clearTextBuffer(char *buffer, int length);
void fillBuffer(char *buffer, char *string, uint8_t length);
void zero2Space(char* buffer, uint16_t length);


/*void clearDebugString(void);
void addDebugData(char *data);
void sendDebugString(void);*/
#endif /* UTILITYFUNCTS_H_ */

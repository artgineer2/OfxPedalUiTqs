/*
 * utilityFuncts.c
 *
 *  Created on: Mar 5, 2016
 *      Author: mike
 */

//#include <avr/pgmspace.h>
//#include <avr/interrupt.h>
#include "macros.h"
#include "utilityFuncts.h"


void delay(unsigned long delay)
{
 unsigned long i;

 for (i = 0; i < delay; i++)
 {
  NOP();
 }
}



void fillBuffer(char *buffer, char *string, uint8_t length)
{


	for(uint8_t i = 0; i < length; i++)
	{
	  if('0'<=string[i] && string[i]<='z') buffer[i] = string[i];
	  else buffer[i] = ' ';
	}
	buffer[length] = 0;

}

void zero2Space(char* buffer, uint16_t length)
{
	uint16_t i = 0;
	for(i = 0; i < length; i++)
	{
		if(buffer[i] == 0) buffer[i] = ' ';
	}
}



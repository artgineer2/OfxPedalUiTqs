/*
 * ChipInit.c
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */



#include "macros.h"
#include "ChipDefs.h"
#include "ChipInit.h"


/***********************************************************
*				Configuring the chip
***********************************************************/
void config_chip(void)
{
 CLI(); //disable all interrupts*/

 MCUCR = 0x00;
 EXTINT_CONA = BIT(ISC00); // enable active any-edge INT0
 TIMER1_IE = 0x00;
 TIMER2_IE = 0x00;
 //SEI();  interrupts re-enabled in main()
}

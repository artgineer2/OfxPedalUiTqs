/*
 * ChipDefs.h
 *
 *  Created on: Jul 26, 2020
 *      Author: mike
 */

#ifndef CHIPDEFS_H_
#define CHIPDEFS_H_

#include <stdint.h>
/************************************* MCU CONTROL *****************************************/
#define MCUCR  (*(volatile uint8_t *)0x55)
#define MCUSR  (*(volatile uint8_t *)0x54)



/************************************* PORTS *****************************************/


typedef struct
{
	uint8_t input;
	uint8_t pinDirection;
	uint8_t output;

} Port_t;


#define PORTA (*(volatile Port_t *)(0x20))
#define PORTB (*(volatile Port_t *)(0x23))
#define PORTC (*(volatile Port_t *)(0x26))
#define PORTD (*(volatile Port_t *)(0x29))
#define PORTE (*(volatile Port_t *)(0x2C))
#define PORTF (*(volatile Port_t *)(0x2F))
#define PORTG (*(volatile Port_t *)(0x32))



/************************************* SPI  *****************************************/


typedef struct
{
	uint8_t control;
	uint8_t status;
	uint8_t data;
} Spi_t;

enum SpiControl {SPR0,SPR1,CPHA,CPOL,MSTR,DORD,SPE,SPIE};
enum SpiStatus {SPI2X,WCOL=6,SPIF};

#define SPI (*(volatile Spi_t*)(0x4C))

/********************************** UNIVERSAL SERIAL INTERFACE ***********************/

typedef struct
{
	uint8_t control;
	uint8_t status;
	uint8_t data;
} Usi_t;

enum UsiControl {USITC,USICLK,USICS0,USICS1,USIWM0,USIWM1,USIOIE,USISIE};
enum UsiStatus {USICNT0,USICNT1,USICNT2,USICNT3,USIDC,USIPF,USIOIF,USISIF};


#define USI (*(volatile Usi_t *)(0xB8))


/************************************* TIMER *****************************************/


typedef struct
{
	uint8_t controlA;
	uint8_t pad1;
	uint8_t counter;
	uint8_t outputCompareA;

} Timer_t;

enum TimerControlA {CS00,CS01,CS02,WGM01,COM0A0,COM0A1,WGM00,FOC0A};
enum TimerInterruptEnable {TOIE0,OCIE0A};
enum TimerInterruptFlags {TOV0,OCF0A};


#define TIMER0 (*(volatile Timer_t *)(0x44))

#define TIMER0_IE  (*(volatile uint8_t *)0x6E)
#define TIMER0_IF  (*(volatile uint8_t *)0x35)

#define TIMER1_IE  (*(volatile uint8_t *)0x6F)
#define TIMER1_IF  (*(volatile uint8_t *)0x36)

#define TIMER2_IE  (*(volatile uint8_t *)0x70)
#define TIMER2_IF  (*(volatile uint8_t *)0x37)


/************************************* EXTERNAL INTERRUPTS *****************************************/

#define EXTINT_IE  (*(volatile uint8_t *)0x3D)
enum ExternalIntEnable {INT0,PCIE0=4,PCIE1,PCIE2,PCIE3};

#define EXTINT_IF  (*(volatile uint8_t *)0x3C)
enum ExternalIntFlags {INTF0,PCIF0=4,PCIF1,PCIF2,PCIF3};

#define EXTINT_CONA 	(*(volatile uint8_t *)0x69)
enum ExternalIntControlA {ISC00,ISC01};

#endif /* CHIPDEFS_H_ */

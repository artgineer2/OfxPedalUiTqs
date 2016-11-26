/*
 * ChipInit.c
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#include "ChipInit.h"
#include "GlobalVars.h"
#include <avr/iom645.h>

/***********************************************************
*		IO Configuration
***********************************************************/

void IO_config(void)
{	 	 //76543210

 DDRA  = 0b00000000;// button lines
 PORTA = 0b00000000;

 	     //76543210
 DDRB  = BIT(LCD_E)|BIT(LCD_RS)|BIT(LCD_RW)|BIT(MCU_MOSI)|BIT(MCU_SCLK)|BIT(MCU_nCS);
 PORTB = BIT(BUTTON_PUSHED)|BIT(MCU_nCS);

 	     //76543210
 DDRC  = 0b11111111; // LCD data lines
 PORTC = 0b00000000;

 	     //76543210
 DDRD  = BIT(nMCU_SM_ACCESS)|BIT(FSW2_LED)|BIT(FSW1_LED); // Compute modules
 PORTD = BIT(nMCU_SM_ACCESS)|BIT(FSW2_LED)|BIT(FSW1_LED);

 	     //76543210
 DDRE  = BIT(nMCU_DATA_RDY1)|BIT(nMCU_DATA_RDY0);
 PORTE = BIT(nMCU_DATA_RDY1)|BIT(nMCU_DATA_RDY0);

 	     //76543210
 DDRF  = BIT(USB_GP1_SPI_CONTROL_GRANTED)|BIT(nUSB_SM_ACCESS);  //USB_DATA_RDY pin is controlled by USB-SPI bridge chip
 PORTF = BIT(nUSB_SM_ACCESS);

 PORTG = BIT(nPOWER_OFF)|BIT(CMX_RUN);
 DDRG = BIT(nPOWER_OFF)|BIT(CMX_RUN);
}

/***********************************************************
*		UART Configuration
***********************************************************/


/********** CHANGE THIS TO USE USI (PORT E, BITS 4,5,6) *************/
void USART_config(void)
{
 UCSR0B = 0x00;
 UCSR0A = 0x00;//
 UCSR0C = BIT(UCSZ01)|BIT(UCSZ00)|BIT(UMSEL0);
 UBRR0H = 0x00;
 UBRR0L = 2; // 1 = 250Kbd, 12 = 38.4Kbd, 12 = 76.8Kbd w/ U2X0 = 1
 UCSR0B = BIT(TXCIE0)|BIT(RXEN0)|BIT(TXEN0);
}

/***********************************************************
*		SPI Configuration
***********************************************************/
void SPI_config(void)
{
 SPCR = BIT(SPE)/*|BIT(SPIE)|BIT(CPOL)|BIT(CPHA)*/|BIT(MSTR)|BIT(SPR1)|BIT(SPR0);
 SPSR = 0X00;
 uint8_t dummy = SPSR;
 dummy = SPDR;
}

/***********************************************************
*		USI Configuration (SPI mode)
***********************************************************/

void USI_config(void)
{
	USIDR = 0;
	USISR = 0;
	USICR = BIT(USIOIE)|BIT(USIWM0)|BIT(USICS1)|BIT(USICLK);
	 TCCR0A = BIT(WGM01)|2; // 1 -> 5
	 OCR0A = 50;
}

/**********************************************************
 *      Pin change configuration
 *********************************************************/
void PCINT_config(void)
{
	PCMSK0 = 0b11100000;
	PCMSK1 = 0b10000000;
}

//Watchdog initialize
// prescale: 512K
void watchdog_init(void)
{
 WDR(); //this prevents a timeout on enabling
 WDTCR |= (1<<WDCE) | (1<<WDE);/* 30-Oct-2006 Umesh*/
 WDTCR = 0x00;//0x0D; //WATCHDOG ENABLED - dont forget to issue WDRs

}

void counter0_init(uint8_t prescaler)
{
 TCNT0=0x40;
 TCCR0A = prescaler; // 1 -> 5
 TIMSK0 = BIT(TOIE0);  // enable overflow interrupt
}


/***********************************************************
*				Configuring the chip
***********************************************************/
void config_chip(void)
{
 CLI(); //disable all interrupts*/
 IO_config();
 //USART_config();
 SPI_config();
 //PCINT_config();
 //watchdog_init();
 counter0_init(5);

 MCUCR = 0x00;
 EICRA = 0x01; // enable active any-edge INT0
 //EICRA = 0x02; /*enable active falling-edge INT0 */
 EIMSK = 0x01; /*enable Ext interrupt*/
 //TIMSK0 = 0x00; /*timer interrupt sources*/
 TIMSK1 = 0x00;
 TIMSK2 = 0x00;
 //SEI(); /*re-enable interrupts*/
}

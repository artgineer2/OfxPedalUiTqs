/*
 * IoDrivers.c
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#include "PinDrivers.h"
#include "GlobalVars.h"
#include <avr/iom645.h>
//extern uint8_t rx_done;

extern void delay(unsigned long delay);
/*{
 unsigned long i;

 for (i = 0; i < delay; i++)
 {
  NOP();
 }
}*/

void LcdWrite(void)
{
 PORTB |= BIT(LCD_E);
 delay(800); //200
 PORTB &= ~BIT(LCD_E);
}

void LcdDataEn(void)	//Set R/S pin high
{
 PORTB |= BIT(LCD_RS);//(1 << (LCD_RS));
}

void LcdInstrEn(void)	  //Set R/S pin low
{
 PORTB &= ~BIT(LCD_RS);
}



/*****************************************************/
/*****************************************************/
/*****************************************************/


void SPI_Tx(uint8_t data)
{
 char dummy;
 SPDR = data;
 while(!(SPSR & BIT(SPIF))){} //Wait for Tx to complete
 dummy = SPDR; //access SPDR to clear SPIF bit
}

uint8_t SPI_Rx(void)
{
 uint8_t data;
 SPDR = 0x00;
 while(!(SPSR & BIT(SPIF))){} //Wait for Rx to complete
 data = SPDR;
 return data;
}

/***********************************************************
*                    Flash Routines
***********************************************************/
// Flash instructions
#define WREN   		0b00000110
#define WRDI   		0b00000100
#define RDSR   		0b00000101
#define WRSR   		0b00000001
#define READ   		0b00000011
#define WRITE  		0b00000010
#define ERASE4K		0b00100000
#define ERASE64K	0xD8
#define CHIPERASE 	0x60
#define SQWRITE 	0b10101101
#define PROTREAD	0x3C
//Flash Status Register
#define nRDY	0	// nReady/Busy Status
#define WEL		1	// Write Enable Latch Status
#define SWP0	2	// Software Protection Status, bit 0
#define SWP1	3	// Software Protection Status, bit 1
#define WPP		4	// Write Protect Pin Status (active low)
#define EPE		5	// Erase/Program Error
#define SPM		6	// Sequential Program Mode Status
#define SPRL	7 	// Section Protection Registers Locked


void Flash_En(void)
{
 PORTB &= ~BIT(0);
}

void Flash_Dis(void)
{
 PORTB |= BIT(0);
}

/***********************************************************
*		Setting flash Write Enable
*		(Do this before writing to the flash)
*
***********************************************************/

void flash_WREN(void)
{
 Flash_En();
 SPI_Tx(WREN);
 Flash_Dis();
}

/***********************************************************
*		Resetting flash Write Enable
*		(Do this after writing to the flash)
*
***********************************************************/
void flash_WRDI(void)
{
 Flash_En();
 SPI_Tx(WRDI);
 Flash_Dis();
}

/***********************************************************
*		Reading flash Status Register
***********************************************************/
uint8_t flash_RDSR(void)
{
 uint8_t x;
 //do{
  Flash_En();
  SPI_Tx(RDSR);
  x = SPI_Rx();
  Flash_Dis();
 //}while ((x & BIT(nRDY)) != 0x00); //nRDY bit is not set (low)
 return x;
}

uint8_t flash_RD_protection(unsigned long address)
{
	 uint8_t data;

	 Flash_En();
	 SPI_Tx(PROTREAD);		   // Read instruction
	 SPI_Tx((address >> 16) & 0x0000FF);	// MSB of address
	 SPI_Tx((address >> 8) & 0x0000FF);
	 SPI_Tx(address & 0x0000FF); // LSB of address
	 data = SPI_Rx();
	 Flash_Dis();

	 return data;

}
void flash_init(void)
{
	flash_WREN();
	Flash_En();
	SPI_Tx(WRSR);
	SPI_Tx(0x00);

	while(((BIT(nRDY)|BIT(SWP1)|BIT(SWP0)) & flash_RDSR()) != 0x00){NOP();}
	Flash_Dis();
}

/***********************************************************
*
*				Reading from the flash
*
*				data_status:  	0:1st uint8_t
*								1:sequence uint8_t
*								2:Last uint8_t
*
***********************************************************/
uint8_t flash_read(unsigned long address)
{
 uint8_t data;

 Flash_En();
 SPI_Tx(READ);		   // Read instruction
 SPI_Tx((address >> 16) & 0x0000FF);	// MSB of address
 SPI_Tx((address >> 8) & 0x0000FF);
 SPI_Tx(address & 0x0000FF); // LSB of address
 data = SPI_Rx();
 Flash_Dis();

 return data;
}

/***********************************************************
*
*				Writing to the flash
*
***********************************************************/
uint8_t flash_write(unsigned long address, uint8_t data)
{
	uint8_t status;

	flash_WREN();
	Flash_En();

	SPI_Tx(WRITE);		   // Write instruction
	SPI_Tx((address >> 16) & 0x0000FF);	// MSB of address
	SPI_Tx((address >> 8) & 0x0000FF);
	SPI_Tx(address & 0x0000FF); // LSB of address
	SPI_Tx(data);
	Flash_Dis();
	//delay(300);
	while((BIT(nRDY) & flash_RDSR()) != 0x00){NOP();} //Wait for nRDY bit to be set (low)

	if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;
	else status = 0;

	return status;
}


/***********************************************************
*
*				Writing sequence to the flash (from the PC)
*
***********************************************************/
uint8_t flash_seq_write(unsigned long start_address, uint8_t data_in, uint8_t uint8_t_status)
{
	unsigned int i;
	uint8_t status;

	status = 0;
	switch(uint8_t_status)
	{
		case 0:  // First uint8_t in sequence
				flash_WREN();
				Flash_En();
				SPI_Tx(SQWRITE);		   // Sequential Write instruction
				SPI_Tx((start_address >> 16) & 0x0000FF);	// MSB of address
				SPI_Tx((start_address >> 8) & 0x0000FF);
				SPI_Tx(start_address & 0x0000FF); // LSB of address
				SPI_Tx(data_in);
				Flash_Dis();
				while((BIT(nRDY) & flash_RDSR()) != 0x00){NOP();} //Wait for nRDY bit to be set (low)

				if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;

				break;
		case 1:  // Middle uint8_t in sequence
				Flash_En();
				SPI_Tx(SQWRITE);		   // Sequential Write instruction
				SPI_Tx(data_in);
				Flash_Dis();
				while((BIT(nRDY) & flash_RDSR()) != 0x00){NOP();} //Wait for nRDY bit to be set (low)

				if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;

				break;
		case 2: // Last uint8_t in sequence
				Flash_En();
				SPI_Tx(SQWRITE);		   // Sequential Write instruction
				SPI_Tx(data_in);
				Flash_Dis();
				while((BIT(nRDY) & flash_RDSR()) != 0x00){NOP();}//Wait for nRDY bit to be set (low)

				if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;

				flash_WRDI();
				break;
		default:
				break;
	}
	return status;
}


/***********************************************************
*
*				Reading array from the flash
*
***********************************************************/
uint8_t flash_array_read(unsigned long start_address, uint8_t *data_out, uint8_t array_size)
{
	uint8_t i,status;

	Flash_En();

	SPI_Tx(READ);		   // Write instruction
	SPI_Tx((start_address >> 16) & 0x0000FF);	// MSB of address
	SPI_Tx((start_address >> 8) & 0x0000FF);
	SPI_Tx(start_address & 0x0000FF); // LSB of address
	for(i = 0; i < array_size; i++)
	{
		data_out[i] = SPI_Rx();
	}
	data_out[array_size] = '\0';

	Flash_Dis();

	if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;
	else status = 0;

	return status;
}


/***********************************************************
*
*				Writing array to the flash
*
***********************************************************/
uint8_t flash_array_write(unsigned long start_address, uint8_t *data_in, uint8_t array_size)
{
	uint8_t i,status;

	flash_WREN();
	Flash_En();

	SPI_Tx(SQWRITE);		   // Sequential Write instruction
	SPI_Tx((start_address >> 16) & 0x0000FF);	// MSB of address
	SPI_Tx((start_address >> 8) & 0x0000FF);
	SPI_Tx(start_address & 0x0000FF); // LSB of address
	SPI_Tx(data_in[0]);
	Flash_Dis();
	delay(30);
	while((BIT(nRDY) & flash_RDSR()) != 0x00){NOP();} //Wait for nRDY bit to be set (low)

	for(i = 1; i < array_size; i++)
	{
		Flash_En();
		SPI_Tx(SQWRITE);
		SPI_Tx(data_in[i]);
		Flash_Dis();
		delay(30);  // try to use enough delay that the status reading isn't needed
		while((BIT(nRDY) & flash_RDSR()) != 0x00){NOP();} //Wait for nRDY bit to be set (low)
	}
	flash_WRDI();

	if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;
	else status = 0;

	return status;
}

/***********************************************************
*		Erasing 4K flash blocks
*		(erases entire FXCore Section)
***********************************************************/
uint8_t flash_4K_erase(unsigned long starting_address)
{
	unsigned long block_addr;
	uint8_t status;

	block_addr = starting_address;//block<<12;
	flash_WREN();
	delay(30);
	Flash_En();
	SPI_Tx(ERASE4K);		   // Erase instruction
	SPI_Tx((block_addr & 0xFF0000) >> 16);	// MSB of address
	SPI_Tx((block_addr  & 0x00FF00) >> 8);
	SPI_Tx(block_addr & 0x0000FF); // LSB of address
	Flash_Dis();
	delay(30);
	while((BIT(nRDY) & flash_RDSR()) != 0x00){NOP();} //Wait for nRDY bit to be set (low)
	if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;
	else status = 0;

	return status;
}

/***********************************************************
*		Erasing 64K flash blocks
*		(erases half of FPGA section)
***********************************************************/
uint8_t flash_64K_erase(unsigned long starting_address)
{
	unsigned long block_addr;
	uint8_t status;

	block_addr = starting_address;//block<<12;
	flash_WREN();
	delay(30);
	Flash_En();
	SPI_Tx(ERASE64K);		   // Erase instruction
	SPI_Tx((block_addr & 0xFF0000) >> 16);	// MSB of address
	SPI_Tx((block_addr  & 0x00FF00) >> 8);
	SPI_Tx(block_addr & 0x0000FF); // LSB of address
	Flash_Dis();
	delay(30);
	while((BIT(nRDY) & flash_RDSR()) != 0x00){NOP();} //Wait for nRDY bit to be set (low)
	if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;
	else status = 0;

	return status;
}


uint8_t flash_chip_erase(void)
{
	uint8_t status;

	flash_WREN();
	delay(30);
	Flash_En();
	SPI_Tx(CHIPERASE);		   // Erase instruction
	Flash_Dis();
	while(((BIT(nRDY)) & flash_RDSR()) != 0x00){NOP();} //Wait for nRDY bit to be set (low)
	if ((BIT(EPE) & flash_RDSR()) != 0x00) status = 1;
	else status = 0;

	return status;
}


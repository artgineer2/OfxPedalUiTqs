/*
 * ComputeModuleFuncts.c
 *
 *  Created on: Mar 10, 2016
 *      Author: mike
 */

#include "ComputeModuleFuncts.h"
#include "GlobalVars.h"
#include "IoLines.h"
#include "utilityFuncts.h"
#include "JsonFuncts.h"
#include <avr/iom645.h>

extern const char *testJson;

#define SHARED_MEMORY_SECTION_SIZE 4096
//#define SHARED_MEMORY_SUBSECTION_SIZE 2048

#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1

void enableMcuSharedMemoryInterface(void)
{
	//DDRE |= BIT(nMCU_DATA_RDY0); // USB/SPI bridge pin 3 will be at high-impedance here

	/***************************************************************************************
	 *  put an AND gate on the circuit board, connect the nDATA_RDY0 pin to the output,
	 *  and the MCU and USB/SPI bridge to the inputs.
	 *************************************************************************************/
}

void disableMcuSharedMemoryInterface(void)
{
	//DDRE &= ~BIT(nMCU_DATA_RDY0);

}

void enableMcuSharedMemorySectionAccess(void)
{
	PORTE |= BIT(nMCU_DATA_RDY0) | BIT(nMCU_DATA_RDY1);// first deactivate all active-low compute module select lines
	PORTD &= ~BIT(nMCU_SM_ACCESS);
	PORTB &= ~BIT(MCU_nCS);
}

void disableMcuSharedMemorySectionAccess(void)
{
	PORTD |= BIT(nMCU_SM_ACCESS);
	PORTB |= BIT(MCU_nCS);
}

/*****************************************************************************/

void enableUsbSharedMemoryAccess(void)
{
	PORTF &= ~BIT(nUSB_SM_ACCESS);
}

void disableUsbSharedMemoryAccess(void)
{
	PORTF |= BIT(nUSB_SM_ACCESS);
}



void enableComputeModuleSharedMemorySectionAccess(uint8_t index)
{
	PORTD |= BIT(nMCU_SM_ACCESS); // first deactivate MCU active-low select line
	switch(index)
	{
	case 0:
		PORTE &= ~BIT(nMCU_DATA_RDY0);
		break;

	case 1:
		//PORTE &= ~BIT(nMCU_DATA_RDY1);
		break;

	default:;
	}
}

void disableComputeModuleSharedMemorySectionAccess(uint8_t index)
{
	switch(index)
	{
	case 0:
		PORTE |= BIT(nMCU_DATA_RDY0);
		break;

	case 1:
		//PORTE |= BIT(nMCU_DATA_RDY1);
		break;

	default:;
	}

}

/*****************************************************************************/

void spiTx(uint8_t data)
{
	uint8_t dummy;
	SPDR = data;
	while(!(SPSR & BIT(SPIF))){} //Wait for Tx to complete
	dummy = SPDR; //access SPDR to clear SPIF bit
}


uint8_t spiRx(void)
{
	uint8_t data;
	SPDR = 0x00;
	while(!(SPSR & BIT(SPIF))){} //Wait for Rx to complete
	data = SPDR;
	return data;
}


void initSerialRam(void)
{
	enableMcuSharedMemorySectionAccess();
	// Write to status register
	spiTx(1);  // Write to status reg instruction
	spiTx(0x40);  // set to Sequential Read/Write
	disableMcuSharedMemorySectionAccess();
	//delay(100);
	enableMcuSharedMemorySectionAccess();
	// Read back contents
	spiTx(5);  //Read from status reg instruction
	spiRx();
	disableMcuSharedMemorySectionAccess();

}


/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/


void clearSerialRam(uint8_t cmIndex, uint16_t address, uint16_t length)
{
	uint8_t status = 0;
	uint16_t bufferIndex = 0;
	address += cmIndex*SHARED_MEMORY_SECTION_SIZE;
	uint8_t addressHigh = address>>8;
	uint8_t addressLow = 0x00FF & address;

	enableMcuSharedMemorySectionAccess();
	spiTx(2);  // Write command
	spiTx(addressHigh);
	spiTx(addressLow);
	for(bufferIndex = 0; bufferIndex < length; bufferIndex++)
	{
		spiTx(0);
	}
	spiTx(255);
	disableMcuSharedMemorySectionAccess();



}

void testSerialRam(uint8_t cmIndex, uint16_t address, uint16_t length)
{
	uint8_t status = 0;
	uint16_t bufferIndex = 0;
	address += cmIndex*SHARED_MEMORY_SECTION_SIZE;
	uint8_t addressHigh = address>>8;
	uint8_t addressLow = 0x00FF & address;
	char testBuffer[21];
	strcpy(testBuffer,"awertqwertqwertqwert");
	enableMcuSharedMemorySectionAccess();
	spiTx(2);  // Write command
	spiTx(addressHigh);
	spiTx(addressLow);
	for(bufferIndex = 0; bufferIndex < 21; bufferIndex++)
	{
		spiTx(testBuffer[bufferIndex]);
	}

	disableMcuSharedMemorySectionAccess();
	delay(100);

	enableMcuSharedMemorySectionAccess();
	spiTx(3);  // Read command
	spiTx(addressHigh);
	spiTx(addressLow);
	for(bufferIndex = 0; bufferIndex < 21; bufferIndex++)
	{
		spiTx(0);
	}

	disableMcuSharedMemorySectionAccess();

}


void giveComputeModuleSharedMemorySectionAccess(uint8_t index)  // tell ComputeModule it has new data in it's shared memory section
{
	/*PORTF |= 0x0F; // first deactivate all active-low CS lines
	PORTF &= ~BIT(index);*/
	enableComputeModuleSharedMemorySectionAccess(index);
}

void takeComputeModuleSharedMemorySectionAccess(uint8_t index)
{
	disableComputeModuleSharedMemorySectionAccess(index);//PORTF |= BIT(index);
}

uint8_t getComputeModuleDataProcessingStatus(uint8_t index)
{
	uint8_t status = 0;

	if(PINE & BIT(DATA_RXED0))
	{
		status = 1;
	}

	return status;
}


uint8_t sendDataToComputeModuleSharedMemorySection(uint8_t index, uint16_t address)
{
	uint8_t status = 0;

	//writeToComputeModuleSharedMemorySection(index,address, data, dataSize);


	if(computeModuleDataSendState == 0)
	{
		computeModuleDataSendState = 1;
	}
	else if(computeModuleDataSendState == 3)
	{
		computeModuleDataSendState = 0;
	}
	return status;
}


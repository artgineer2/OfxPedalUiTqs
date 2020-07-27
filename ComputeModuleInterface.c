/*
 * ComputeModuleFuncts.c
 *
 *  Created on: Mar 10, 2016
 *      Author: mike
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ChipDefs.h"
#include "macros.h"
#include "utilityFuncts.h"
#include "Typedefs.h"


#include "TaskQueue.h"
#include "UserInterfaceOps.h"
#include "UserInterfaceDrivers.h"
#include "ComputeModuleInterface.h"

/******************* COMPUTE MODULE INTERFACE FUNCTIONING OVERVIEW **************************************
 *
 *	Instead of having a direct SPI connection between the Compute Module (master) and the MCU (slave),
 *	the SPI ports on both were configured as masters and wrote to serial RAM chip between them.  Signal
 *	lines would then be used to give each side exclusive access to the serial RAM and inform each side
 *	that there was new data ready for it in the serial RAM.
 *
 *	WHY did I do it THIS way instead of a DIRECT connection??...that's another story.
 *
 *
 *	The protocol for writing to serial ram:
 *		byte 0: Command (2:Write, 3:Read)
 *		byte 1: Address (High Byte)
 *		byte 2: Address (Low Byte)
 *
 *	Timer0 ISR:
 *		1)	Managing the request/response process through the SPI ISR, and  MCU and CM RAM access lines.
 *		2)	Reading buttons and encoder
 *				These values are hidden in the UserInterfaceDrivers module and only accessible
 *				through getButtonPushed() and getEncoderDirection()
 *		3)	Create new tasks:
 *				Inserting tasks into the TaskQueue module via insertTask(uint8_t);
 *
 *
 *
 *
 ********************************************************************************************************/


#define CURRENT_DATA_UPDATE_TIMER_THRESHOLD 40

/*** PORTB ***/
#define MCU_MISO 	3
#define MCU_MOSI 	2
#define MCU_SCLK 	1
#define MCU_nCS  	0

/*** PORTD ***/
#define nMCU_SM_ACCESS		6

/*** PORTE ***/
#define nMCU_DATA_RDY1	6 // tell CM1 new data from MCU is ready in shared memory
#define DATA_RXED0	3 // status of data from MCU in CM0 shared memory
#define nMCU_DATA_RDY0 2 // tell CM0 new data is ready in shared memory
#define OFX_RDY0 1
#define CM_RUNNING0 0

/*** PORTG ***/
#define CMX_RUN 2

#define SHARED_MEMORY_SECTION_SIZE 4096

#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1

static uint8_t computeModuleDataRetrievalState;// 0: Idle, 1: Start, 2: In-process, 3: Stop
// NOT TO BE CONFUSED WITH CM PROCESSING STATE
static uint8_t computeModuleDataSendState;// 0: Idle, 1: Start, 2: In-process, 3: Stop

static char *sharedMemoryRxBufferPtr;
static uint16_t sharedMemoryRxBufferIndex;
static uint16_t sharedMemoryRxBufferCount;
static uint16_t sharedMemoryRxStartAddress;

static char *sharedMemoryTxBufferPtr;
static uint16_t sharedMemoryTxBufferIndex;
static uint16_t sharedMemoryTxBufferCount;
static uint16_t sharedMemoryTxStartAddress;

static uint16_t spiXferSizeLimit;
static uint16_t spiXferCount;
static uint8_t spiCommand[4];
static uint8_t spiCommandIndex;


static uint8_t requestStatus; //0: Idle, 1: Sending Request, 2: Waiting for response, 3: Getting Response, 4:Response Received
static uint8_t newSpiXferRequest;
static uint8_t getResponse;


static uint8_t currentDataUpdateTimer;



/*	@brief: Configure port pins used for SPI port, the pin used for accessing
 * 			serial RAM, and the pins for signaling to a Compute Module that
 * 			there is data ready for it from the MCU.
 *
 * 	@returns: none
 *
 */
static void initPortPins(void)
{

	PORTB.pinDirection |= BIT(MCU_MOSI)|BIT(MCU_SCLK)|BIT(MCU_nCS);
	PORTB.output |= BIT(MCU_nCS);

	PORTD.pinDirection  |= BIT(nMCU_SM_ACCESS);
	PORTD.output |= BIT(nMCU_SM_ACCESS);

	PORTE.pinDirection  |= BIT(nMCU_DATA_RDY1)|BIT(nMCU_DATA_RDY0);
	PORTE.output |= BIT(nMCU_DATA_RDY1)|BIT(nMCU_DATA_RDY0);

	PORTG.pinDirection |= BIT(CMX_RUN);
	PORTG.output |= BIT(CMX_RUN);;


}

/*	@brief: Configures the SPI port as master
 *
 * 	@returns: none
 */

static void initSpi(void)
{
	SPI.control = BIT(SPE)|BIT(MSTR)|BIT(SPR1)|BIT(SPR0);
	SPI.status = 0X00;
	uint8_t dummy = SPI.data;
	dummy = SPI.data;
}

/*	@brief: Configures Timer0 and enables Overflow interrupt.
 *
 * 	@param: prescalar: controls how much the MCU clock is
 * 			divided before using as the Timer0 clock.
 *
 * 	@returns: none
 */

static void initTimer0(uint8_t prescaler)
{
	TIMER0.counter=0x40;
	TIMER0.controlA = prescaler; // 1 -> 5
	TIMER0_IE = BIT(TOIE0);  // enable overflow interrupt
}


/*	@brief: Sends data from the SPI port
 *
 * 	@param: data: byte to be sent.
 *
 * 	@returns: none
 */

static void spiTx(uint8_t data)
{
	uint8_t dummy;
	SPI.data = data;
	while(!(SPI.status & BIT(SPIF))){} //Wait for Tx to complete
	dummy = SPI.data; //access SPI.data to clear SPIF bit
}

/*	@brief: Retrieves data to the SPI port
 *
 * 	@returns: byte retrieved
 */

static uint8_t spiRx(void)
{
	uint8_t data;
	SPI.data = 0x00;
	while(!(SPI.status & BIT(SPIF))){} //Wait for Rx to complete
	data = SPI.data;
	return data;
}


/*	@brief:	Enables the MCU to access its designated section of the
 * 			serial RAM.
 *
 * 	@return: none
 *
 */

static void enableMcuSharedMemorySectionAccess(void)
{
	PORTE.output |= BIT(nMCU_DATA_RDY0) | BIT(nMCU_DATA_RDY1);// first deactivate all active-low compute module select lines
	PORTD.output &= ~BIT(nMCU_SM_ACCESS);
	PORTB.output &= ~BIT(MCU_nCS);
}



/*	@brief:	Disables the MCU's access to its designated section of the
 * 			serial RAM.
 *
 * 	@return: none
 *
 */

static void disableMcuSharedMemorySectionAccess(void)
{
	PORTD.output |= BIT(nMCU_SM_ACCESS);
	PORTB.output |= BIT(MCU_nCS);
}


/*	@brief:	Enables the Compute Module to access its designated section of the
 * 			serial RAM.
 *
 * 	@param: index: controls which Compute Module gets access.
 *
 * 	@return: none
 *
 */

static void enableComputeModuleSharedMemorySectionAccess(uint8_t index)
{
	PORTD.output |= BIT(nMCU_SM_ACCESS); // first deactivate MCU active-low select line
	switch(index)
	{
	case 0:
		PORTE.output &= ~BIT(nMCU_DATA_RDY0);
		break;

	case 1:
		PORTE.output &= ~BIT(nMCU_DATA_RDY1);
		break;

	default:;
	}
}


/*	@brief:	Disables the Compute Module's access to its designated section of the
 * 			serial RAM.
 *
 * 	@param: index: controls which Compute Module access is disabled.
 *
 * 	@return: none
 *
 */


static  void disableComputeModuleSharedMemorySectionAccess(uint8_t index)
{
	switch(index)
	{
	case 0:
		PORTE.output |= BIT(nMCU_DATA_RDY0);
		break;

	case 1:
		PORTE.output |= BIT(nMCU_DATA_RDY1);
		break;

	default:;
	}

}


/**************************************** PUBLIC FUNCTIONS *************************************/

/*	@brief:	Initialize the Compute Module interface by configuring the
 * 			necessary peripherals and the serial RAM.
 *
 * 	@returns: none
 *
 */

void initComputeModuleInterface(void)
{
	initPortPins();
	initSpi();
	initTimer0(5);
	initSerialRam();
}


/*	@brief:	configure the serial RAM for sequential read/write
 *
 *  @returns: none
 *
 */

void initSerialRam(void)
{
	enableMcuSharedMemorySectionAccess();
	// Write to status register
	spiTx(1);  // Write to status reg instruction
	spiTx(0x40);  // set to Sequential Read/Write
	disableMcuSharedMemorySectionAccess();
	enableMcuSharedMemorySectionAccess();
	// Read back contents
	spiTx(5);  //Read from status reg instruction
	spiRx();
	disableMcuSharedMemorySectionAccess();

}


/*	@brief: Clears memory in the Compute Modules section of the serial RAM.
 *
 * 	@param: cmIndex: controls which Compute Module section is cleared.
 *
 * 	@param:	address: the base address of the memory to be cleared.
 *
 * 	@param: length: number of bytes to be cleared.
 *
 *
 * 	@returns: none
 */

void clearSerialRam(uint8_t cmIndex, uint16_t address, uint16_t length)
{

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


/*	@brief: Send a string to the Compute Module, using the Timer0 Overflow ISR
 * 			and the SPI ISR.
 *
 *	@param: sharedMemoryTxBufferPtr: pointer to data to send.

 *	@param: txSize: size of data to send.

 *	@param: sharedMemoryRxBufferPtr: pointer to data to get.

 *	@param: rxSize: size of data to get.
 *
 *	@returns: none
 */


void sendStringToComputeModule(char *sharedMemoryTxBufferPtr, uint8_t txSize, char *sharedMemoryRxBufferPtr, uint16_t rxSize)
{
	disablePowerOff();
	sharedMemoryTxBufferPtr[txSize-1]=255;
	sharedMemoryTxStartAddress = CM0_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;

	if(rxSize == 0)
	{
		getResponse = 0;
	}
	else
	{
		getResponse = 1;
		sharedMemoryRxStartAddress = MCU_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;
	}

	newSpiXferRequest = 1;
	while(requestStatus < 4);
	requestStatus = 0;

	enablePowerOff();
}



void resetCurrentDataUpdateTimer(void)
{
	currentDataUpdateTimer = 0;
}


/*	@brief:	Reset the Broadcom SOC running Linux and the main application
 *
 * 	returns: none.
 *
 */
void resetComputeModule(void)
{
	PORTG.output &= ~BIT(CMX_RUN);
	delay(10000);
	PORTG.output |= BIT(CMX_RUN);

}


/*	@brief: Gets the status of the Compute Module
 * 			(1: Running, 0: Reset/Off)
 *
 * 	@returns: run status of Compute Module
 */

uint8_t isComputeModuleRunning(void)
{
	return (((PORTE.input & (BIT(CM_RUNNING0))) == (BIT(CM_RUNNING0)))? 1 : 0);
}

/*	@brief: Gets the status of the main application running on
 * 			the Compute Module (1: Running main loop, 0: Starting up)
 *
 * 	@returns: run status of main application
 */

uint8_t isOfxMainRunning(void)
{
	return (((PORTE.input & (BIT(OFX_RDY0))) == (BIT(OFX_RDY0))) ? 1 :0 );
}
/*********************************** EVENTS ***********************************************/





/*************************************************
 *             SPI Interrupt Handler
 *
 **************************************************/

// SPI ISR
void __vector_12(void) __attribute__ ((interrupt));
void __vector_12(void)
{
	uint8_t temp = SPI.data;
	if((requestStatus == 3)&&(computeModuleDataRetrievalState == 3))
	{
		disableMcuSharedMemorySectionAccess();

		SPI.control &= ~BIT(SPIE); // disable SPI interrupt
		spiXferCount = 0;
	}
	/**************** END OF DATA TOTAL *********************/
	else if(((requestStatus == 1)&&(sharedMemoryTxBufferPtr[sharedMemoryTxBufferIndex]==255))|| // sending request
			((requestStatus == 3)&&(temp == 255))) // getting response
	{
		if(requestStatus == 3) // getting response
		{
			SPI.data = 0;
			sharedMemoryRxBufferCount = sharedMemoryRxBufferIndex;
			computeModuleDataRetrievalState = 3;
		}
		else if(requestStatus == 1)// sending request
		{
			SPI.data = sharedMemoryTxBufferPtr[sharedMemoryTxBufferIndex];
			sharedMemoryTxBufferCount = sharedMemoryTxBufferIndex;
			computeModuleDataSendState = 3; // sending request
		}
	}
	/************* END OF DATA BLOCK **********************/
	else if(spiXferCount >= spiXferSizeLimit)
	{
		if(requestStatus == 3)// getting response
		{
			computeModuleDataRetrievalState = 2;
			if(spiXferCount > 4 && ' ' <= temp && temp <= '~')
			{
				sharedMemoryRxBufferPtr[sharedMemoryRxBufferIndex++] = temp;
			}
		}
		else if(requestStatus == 1) // sending request
		{
			computeModuleDataSendState = 2;
			if(spiXferCount > 4) SPI.data = sharedMemoryTxBufferPtr[sharedMemoryTxBufferIndex++];
		}
		disableMcuSharedMemorySectionAccess();
		SPI.control &= ~BIT(SPIE); // disable SPI interrupt
		spiXferCount = 0;
	}
	else
	{
		if(requestStatus == 3)// getting response
		{
			if(spiCommandIndex < 4) SPI.data = spiCommand[spiCommandIndex++];
			else
			{
				if(' ' <= temp && temp <= '~') sharedMemoryRxBufferPtr[sharedMemoryRxBufferIndex++] = temp;
				SPI.data = 0;
			}
		}
		else if(requestStatus == 1) // sending request
		{
			if(spiCommandIndex < 4) SPI.data = spiCommand[spiCommandIndex++];
			else  SPI.data = sharedMemoryTxBufferPtr[sharedMemoryTxBufferIndex++];
		}
		spiXferCount++;
	}

}


// Timer0 Overflow ISR
void __vector_11(void) __attribute__ ((interrupt));
void __vector_11(void)
{
	static int statusRequestLoopCount;

	PORTE.output &= ~BIT(nMCU_DATA_RDY1);


	// ************ Enable rotary encoder EXT interrupts only if menuLevel = 3
	if(getMenuLevel() == 3)
	{
		EXTINT_IE = BIT(INT0); /*enable Ext interrupt*/
	}
	else
	{
		EXTINT_IE = 0x00; /*disable Ext interrupt*/
	}


	//updateDisplay(getPedalUiMode());

	/***************** REQUEST/RESPONSE functions ************************
	 *  COMPUTE MODULE DATA TRANSFER:
	 *
	 *****************************************************************************/



	if(newSpiXferRequest == 1 || requestStatus != 0)
	{
		switch(requestStatus)
		{
		case 0: // idle
			statusRequestLoopCount = 0;
			requestStatus = 1;
			break;
		case 1: // sending request
			newSpiXferRequest = 0;
			statusRequestLoopCount++;
			if(computeModuleDataSendState == 0)
			{
				computeModuleDataSendState = 1;
			}
			else if(computeModuleDataSendState == 1) // start request
			{
				uint8_t addressHigh = 0x00FF & (sharedMemoryTxStartAddress>>8);
				uint8_t addressLow = 0x00FF & sharedMemoryTxStartAddress;

				enableMcuSharedMemorySectionAccess();
				SPI.control &= ~BIT(SPIE); // disable SPI interrupt

				spiCommand[0]=2; // write command
				spiCommand[1]=addressHigh;
				spiCommand[2]=addressLow;
				spiCommand[3]=requestStatus;
				computeModuleDataSendState = 2;

				sharedMemoryTxBufferIndex = 0;
				SPI.control |= BIT(SPIE); // enable SPI interrupt
				spiXferCount = 0;
				spiCommandIndex = 0;
				SPI.data = spiCommand[spiCommandIndex++];
			}
			else if(computeModuleDataSendState == 2) // in-process
			{
				uint16_t tempAddress = sharedMemoryTxStartAddress + sharedMemoryTxBufferIndex;
				uint8_t addressHigh = 0x00FF & (tempAddress>>8);
				uint8_t addressLow = 0x00FF & tempAddress;

				enableMcuSharedMemorySectionAccess();
				SPI.control &= ~BIT(SPIE); // disable SPI interrupt
				spiCommand[0]=2; // write command
				spiCommand[1]=addressHigh;
				spiCommand[2]=addressLow;
				spiCommand[3]=requestStatus;
				SPI.control |= BIT(SPIE); // enable SPI interrupt
				spiXferCount = 0;
				spiCommandIndex = 0;
				SPI.data = spiCommand[spiCommandIndex++];
			}
			else if(computeModuleDataSendState == 3) // request sent
			{
				computeModuleDataSendState = 0;
				computeModuleDataRetrievalState = 0;
				enableComputeModuleSharedMemorySectionAccess(0);
				statusRequestLoopCount = 0;
				requestStatus = 2;
			}

			break;
		case 2: // waiting for response (may want timeout error functioning, here)
			statusRequestLoopCount++;
			if(PORTE.input & BIT(DATA_RXED0)) // processing complete
			{
				if(getResponse == 1)
				{
					requestStatus = 3;
				}
				else requestStatus = 4;
				statusRequestLoopCount = 0;
				currentDataUpdateTimer = 0;
				disableComputeModuleSharedMemorySectionAccess(0);
			}
			break;
		case 3:
			// getting response
			statusRequestLoopCount++;
			if(computeModuleDataRetrievalState == 0)
			{
				computeModuleDataRetrievalState = 1;
				disableComputeModuleSharedMemorySectionAccess(0);
			}
			else if(computeModuleDataRetrievalState == 1) // start retrieval
			{
				uint8_t addressHigh = 0x00FF & (sharedMemoryRxStartAddress>>8);
				uint8_t addressLow = 0x00FF & sharedMemoryRxStartAddress;

				enableMcuSharedMemorySectionAccess();

				SPI.control &= ~BIT(SPIE); // disable SPI interrupt

				spiCommand[0]=3;  // read command
				spiCommand[1]=addressHigh;
				spiCommand[2]=addressLow;
				spiCommand[3]=requestStatus;
				computeModuleDataRetrievalState = 2;
				sharedMemoryRxBufferIndex = 0;
				SPI.control |= BIT(SPIE); // enable SPI interrupt
				spiXferCount = 0;
				spiCommandIndex = 0;
				SPI.data = spiCommand[spiCommandIndex++];
			}
			else if(computeModuleDataRetrievalState == 2) // in-process
			{
				statusRequestLoopCount++;

				uint16_t tempAddress = sharedMemoryRxStartAddress + sharedMemoryRxBufferIndex +1;
				uint8_t addressHigh = 0x00FF & (tempAddress>>8);
				uint8_t addressLow = 0x00FF & tempAddress;

				enableMcuSharedMemorySectionAccess();

				SPI.control &= ~BIT(SPIE); // disable SPI interrupt
				spiCommand[0]=3;  // read command
				spiCommand[1]=addressHigh;
				spiCommand[2]=addressLow;
				spiCommand[3]=requestStatus;
				SPI.control |= BIT(SPIE); // enable SPI interrupt
				spiXferCount = 0;
				spiCommandIndex = 0;
				SPI.data = spiCommand[spiCommandIndex++];

			}
			else if(computeModuleDataRetrievalState == 3) // done
			{
				computeModuleDataRetrievalState = 0; // set to idle state
				statusRequestLoopCount = 0;
				requestStatus = 4;
			}
			break;
		case 4: // response received
			statusRequestLoopCount++;
			clearSerialRam(CM0_SHARED_MEMORY_SECTION_INDEX,0,sharedMemoryTxBufferCount+10);
			clearSerialRam(MCU_SHARED_MEMORY_SECTION_INDEX,0,sharedMemoryRxBufferCount+10);
			sharedMemoryRxBufferCount = 0;
			sharedMemoryTxBufferCount = 0;

			break;
		default:;
		}
	}
	else // NO TASKS ARE BEING PROCESSED, MCU IN IDLE STATE
	{
		if(currentDataUpdateTimer > CURRENT_DATA_UPDATE_TIMER_THRESHOLD)
		{
			currentDataUpdateTimer = 0;
			insertTask(RequestStatusUpdateFromCM);

		}
		else
		{
			// Read buttons and rotary encoder
			readButtons();
			readEncoder();

			createTasks();
		}
	}
	currentDataUpdateTimer++;



	PORTE.output |= BIT(nMCU_DATA_RDY1);

	/*********************************************************************/
	TIMER0.counter=0x00;

}


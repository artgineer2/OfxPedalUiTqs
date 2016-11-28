/*
 * events.c
 *
 *  Created on: Mar 5, 2016
 *      Author: mike
 */

#include "Events.h"
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/iom645.h>
#include "GlobalVars.h"
#include "macros.h"
#include "IoLines.h"
#include "ComputeModuleFuncts.h"
#include "utilityFuncts.h"

//#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
//#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define SHARED_MEMORY_SECTION_SIZE 4096
#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1
#define CURRENT_DATA_UPDATE_TIMER_THRESHOLD 40

extern uint8_t rx_done;

extern void delay(unsigned long delay);
extern void updateParamValues(void);
extern void updateStatus(void);

#define DEBOUNCE_DELAY 200

uint8_t transfer(uint8_t data) {
  SPDR = data;
  while (!(SPSR & _BV(SPIF)))
    ;
  return SPDR;
}


/*************************************************
*             External Interrupt Handler
*			  for rotary encoder
**************************************************/
//#pragma interrupt_handler Int0_Interrupt:2
//void INT0_vect(void)


//uint8_t aEdgeDetected = 0;  // 0:falling edge, 1: rising edge
uint8_t bEdgeDetected = 0;  // 0:falling edge, 1: rising edge
uint8_t edgeIndex = 0;
uint8_t level[2][2] = {{0,0},{0,0}};   // level[edge index][pin number]


ISR(INT0_vect)
{
 	 uint8_t Enc_Dir = 0;


 	 if((PIND & 0x02) == 0)// Falling edge detected
 	 {
 		 //if(edgeIndex == 0) // falling edge is valid rotary event
 		 {
 	 		 level[edgeIndex][1] = 0;
 	 		 level[edgeIndex][0] = PIND & 0x01;
 	 		 edgeIndex = 1;
 		 }
 		 /*else
 		 {
 			level[0][0] = 0; // reset array
 			level[0][1] = 0; // reset array
 			level[1][0] = 0; // reset array
 			level[1][1] = 0; // reset array
 		 }*/
 	 }
 	 else // Rising Edge detected
 	 {
 		//if(edgeIndex == 1) // rising edge is valid rotary event
 		{
 	 		level[edgeIndex][1] = 1;
 			level[edgeIndex][0] = PIND & 0x01;
 			edgeIndex = 0;

 			if((level[0][0] == level[0][1]) && (level[1][0] == level[1][1]))
 			{
 				  count_up = 0;
 				  count_down = 1;
 			}
 			else if((level[0][0] != level[0][1]) && (level[1][0] != level[1][1]))
 			{
 				  count_up = 1;
 				  count_down = 0;
 			}
 		}

		level[0][0] = 0; // reset array
		level[0][1] = 0; // reset array
		level[1][0] = 0; // reset array
		level[1][1] = 0; // reset array
 	 }

	 /*{
	  Enc_Dir = PIND & 0x01;
	 }
	 if (Enc_Dir == 0) // if Enc_Dir is low
	 {
	  count_up = 0;
	  count_down = 1;
	 }
	 else if (Enc_Dir == 1)// if Enc_Dir is high
	 {
	  count_up = 1;
	  count_down = 0;
	 }*/

}

/*************************************************
*             Pin Change Interrupt Handlers
*************************************************
//#pragma interrupt_handler PC_Interrupt:3
ISR(PCINT0_vect) // PCINT 0-7
{

	//strcpy(lcdBuffer[0], "PCINT0_vect");
	LCD_change = 1;
	//  Compute module finished processing data in its section of sharing memory?
	uint8_t CMIndex = 0;

	CMIndex = 0x07 & (PINE>>5);
	switch(CMIndex)
	{
	case 1:
		PORTF |= BIT(CM0_DATA_RDY);// set CM0_DATA_RDY line to high (inactive)
		computeModuleStatusArray[0].cmDataRecieved = 1;// flag CM0 shared memory section for processing
		break;
		break;
	default:;
	}

}*/

/*ISR(PCINT1_vect) // PCINT 8-15
{

	//  Button pushed?
		if(PINA != 0 && buttonPushed == 0)
		{

			//sprintf(lcdBuffer[0],"button:%d",buttonPushed);
			//delay(500);
			buttonPushed = PINA;
		}
		else
		{
			//buttonPushed = 0;
		}
	//  Footswitch pushed?
		if(PINF & (BIT(FOOTSWITCH1)|BIT(FOOTSWITCH2)))
		{
			if(PINF & (BIT(FOOTSWITCH1))) fsw1 = 1;
			if(PINF & (BIT(FOOTSWITCH2))) fsw2 = 1;
		}

}*/




/*************************************************
*             SPI Interrupt Handler
*
**************************************************/
//#pragma interrupt_handler SPI_Interrupt:13
ISR(SPI_STC_vect)
{
	uint8_t temp = SPDR;
	if((requestStatus == 3)&&(computeModuleDataRetrievalState == 3))
	{
		disableMcuSharedMemorySectionAccess();

		SPCR &= ~BIT(SPIE); // disable SPI interrupt
		spiXferCount = 0;

	}
	/**************** END OF DATA TOTAL *********************/
	else if(((requestStatus == 1)&&(sharedMemoryTxBuffer[sharedMemoryTxBufferIndex]==255))|| // sending request
			((requestStatus == 3)&&(temp == 255))) // getting response
	{
		if(requestStatus == 3) // getting response
		{
			//sharedMemoryRxBuffer[sharedMemoryRxBufferIndex] = temp;
			SPDR = 0;
			sharedMemoryRxBufferCount = sharedMemoryRxBufferIndex;
			computeModuleDataRetrievalState = 3;
		}
		else if(requestStatus == 1)// sending request
		{
			SPDR = sharedMemoryTxBuffer[sharedMemoryTxBufferIndex];
			sharedMemoryTxBufferCount = sharedMemoryTxBufferIndex;
			computeModuleDataSendState = 3; // sending request
		}
//		delay(50);

	}
	/************* END OF DATA BLOCK **********************/
	else if(spiXferCount >= spiXferSizeLimit)
	{
		if(requestStatus == 3)// getting response
		{
			computeModuleDataRetrievalState = 2;
			if(spiXferCount > 4 && ' ' <= temp && temp <= '~') sharedMemoryRxBuffer[sharedMemoryRxBufferIndex++] = temp;
		}
		else if(requestStatus == 1) // sending request
		{
			computeModuleDataSendState = 2;
			if(spiXferCount > 4) SPDR = sharedMemoryTxBuffer[sharedMemoryTxBufferIndex++];
		}
		disableMcuSharedMemorySectionAccess();
		SPCR &= ~BIT(SPIE); // disable SPI interrupt
		spiXferCount = 0;
	}
	else
	{
		/*if(spiCommandIndex < 4) SPDR = spiCommand[++spiCommandIndex];
		else SPDR = 0;*/

		if(requestStatus == 3)// getting response
		{
			if(spiCommandIndex < 4) SPDR = spiCommand[spiCommandIndex++];
			else
			{
				if(' ' <= temp && temp <= '~') sharedMemoryRxBuffer[sharedMemoryRxBufferIndex++] = temp;
				SPDR = 0;
			}

			//if(spiXferCount > 2) sharedMemoryRxBuffer[sharedMemoryRxBufferIndex++] = temp;

		}
		else if(requestStatus == 1) // sending request
		{
			if(spiCommandIndex < 4) SPDR = spiCommand[spiCommandIndex++];
			else /*if(spiXferCount > 2)*/ SPDR = sharedMemoryTxBuffer[sharedMemoryTxBufferIndex++];
		}

		spiXferCount++;
	}

}


ISR(USART0_TX_vect)
{
	UCSR0C &= ~BIT(UMSEL0);
}


ISR(TIMER0_OVF_vect)
{
	uint8_t command = 0;
	static uint8_t fswDebounceCount;
	static int statusRequestLoopCount;
	static powerOffCount;
	PORTE &= ~BIT(nMCU_DATA_RDY1);
	/*if(globalError.position != 0)
	{
		CLI();
		char position[5];
		sprintf(position,"%d", globalError.position);
		Display(globalError.file, globalError.function, position,globalError.desc);
		while(1);
	}*/
	/************************ BUTTON DEBOUNCE AND READ **************************/
#if 1
	if((PINA & 0x7F) != 0) // for de-bouncing switches
	{
		if(buttonDebounceCount == 0)
		{
			//buttonDebounceCount++;
			buttonPushed = 0;
			uiTempButtons = 0;
		}
		else if(buttonDebounceCount < 2)
		{
			buttonPushed = PINA;
			//uiTempButtons = PINA;

		}
		buttonDebounceCount++;
	}
	else
	{
		buttonPushed = 0;
		//buttonPushed = uiTempButtons;
		uiTempButtons = 0;

		buttonDebounceCount = 0;
	}

	if((PING & BIT(MCU_POWER_CONTROL)) == 0)
	{
		powerOffCount++;
		if(powerOffCount > 100)
		{
			// power off routine

			powerOffSignal = 1;
		}
	}
	else
	{
		powerOffCount = 0;
	}

	// ******************** Host/Pedal UI switch ****************************
	/*if((PINA & BIT(7)) == BIT(7)) //Host GUI
	{
		hostUiActive = 1;
	}
	else // Pedal UI*/
	{
		hostUiActive = 0;
	}
	/**********************************************************************
	 *
	 * 	Footswitches
	 *
	 **********************************************************************/
#endif

#if 1
	if(fswDebounceCount == 0)
	{
		if(PIND & BIT(FSW1))
		{
			if(fsw1.physicalStatus == 0)  // fsw1 just pressed
			{
				fsw1.value ^= 0x01;
				fsw1.physicalStatus = 1;
				fsw1.change = 1;
				valueChange = 1;
			}
		}
		else
		{
			if(fsw1.physicalStatus == 1)  // fsw1 just released
			{
				fsw1.physicalStatus = 0;
			}
		}

		if(PIND & BIT(FSW2))
		{
			if(fsw2.physicalStatus == 0)  // fsw2 just pressed
			{
				fsw2.value ^= 0x01;
				fsw2.physicalStatus = 1;
				fsw2.change = 1;
				valueChange = 1;
			}
		}
		else
		{
			if(fsw2.physicalStatus == 1)  // fsw2 just released
			{
				fsw2.physicalStatus = 0;
			}
		}


		if(fsw1.value) PORTD &= ~BIT(FSW1_LED);
		else PORTD |= BIT(FSW1_LED);

		if(fsw2.value) PORTD &= ~BIT(FSW2_LED);
		else PORTD |= BIT(FSW2_LED);

		fswDebounceCount = 5;
	}
	else fswDebounceCount--;

#endif
	/****************************************************************************
	 * 	Value updating
	 *
	 *
	 */
#if 1
	/*if(hostUiActive == 1) // host PC application not active
		currentDataUpdateTimer = 0;*/

	if(1)//hostUiActive == 0) // host PC application not active
	{
		if(requestStatus == 0) // requests are done
		{
			if(restoreFromHostUiMode == 0) // normal pedal UI running mode
			{
				//**************** Updating values from Pedal User Interface ***************
				if(valueChange == 1 && menuLevel == 3)
				{
					clearBuffer(sendBuffer,50);
					sendBuffer[49]=255;
					valueChange = 0;

					{
						sprintf(sendBuffer, "changeValue:%d=%d", nodeArray[currentNodeArrayIndex].paramIndex,
								nodeArray[currentNodeArrayIndex].value);
					}
					sendBuffer[49] = 255;
					sharedMemoryTxBuffer = sendBuffer;
					sharedMemoryTxStartAddress = CM0_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//MCU_SHARED_MEMORY_SECTION_ADDRESS;
					getResponse = 0;
					//requestStatus = 1;
					newRequest = 1;
				}
				// ******************* getting update from CM ************************
				else if((currentDataUpdateTimer > CURRENT_DATA_UPDATE_TIMER_THRESHOLD))
				{
					clearBuffer(sendBuffer,50);
					sendBuffer[49]=255;
					strcpy(sendBuffer,"getCurrentStatus");//strcpy(sendBuffer,"getCurrentValues");
					sharedMemoryRxBuffer = currentDataString;  // set sharedMemory pointer to currentDataString
					sharedMemoryRxStartAddress = MCU_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;
					sharedMemoryTxBuffer = sendBuffer;
					sharedMemoryTxStartAddress = CM0_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//MCU_SHARED_MEMORY_SECTION_ADDRESS;
					getResponse = 1;
					//requestStatus = 1;
					newRequest = 1;
					currentDataUpdateTimer = 0;
				}

				/*else if((PINF & BIT(USB_GP0_SPI_CONTROL_RQST)) == BIT(USB_GP0_SPI_CONTROL_RQST))
				// host PC application just turned on
				{
					//**************** Updating values from Host PC User Interface ***************
					currentDataUpdateTimer = 0;
					hostUiActive = 1;

					PORTF |= BIT(USB_GP1_SPI_CONTROL_GRANTED);

					//******************************************************************************
					// * make sure USB takes control back (USB input pin changed to high output pin)
					 //* before MCU releases it (MCU high output pin changed to input pin).
					 //*
					 //*****************************************************************************

					disableMcuSharedMemorySectionAccess();
					enableUsbSharedMemoryAccess();
				}*/
				currentDataUpdateTimer++;
			}
		}
	}
	else // host PC application active
	{
		currentDataUpdateTimer = 0;
		// requestStatus should be 0, here
		/*if((PINF & BIT(USB_GP0_SPI_CONTROL_RQST)) == 0) // host PC application just turned off, go back into pedal UI mode
		{
			restoreFromHostUiMode = 1;
			hostUiActive = 0;
			PORTF &= ~BIT(USB_GP1_SPI_CONTROL_GRANTED);
			//******************************************************************************
			 //* make sure MCU takes control back (MCU input pin changed to high output pin)
			 //* before USB releases it (USB high output pin changed to input pin).
			 //*
			 //*****************************************************************************

			disableUsbSharedMemoryAccess();
			enableMcuSharedMemorySectionAccess();
		}*/
	}



	/*if(PIND & BIT(USB_SPI_CONTROL_RQST) == BIT(USB_SPI_CONTROL_RQST)) tempHostUiActive = 1;
	else tempHostUiActive = 0;*/
		//restoreFromHostUiMode = 1;


	/*if(tempHostUiActive == 0 && hostUiActive == 1) // PC host has been deactivated
	{
		//comboIndex = atoi(comboIndexString);
		restoreFromHostUiMode = 1;
		//hostUiActive = 0;
		PORTF &= ~BIT(USB_SPI_CONTROL_GRANTED);

		//******************************************************************************
		// * make sure MCU takes control back (MCU input pin changed to high output pin)
		// * before USB releases it (USB high output pin changed to input pin).
		// *
		// *****************************************************************************

		enableMcuSharedMemoryInterface();

	}
	else if (requestStatus == 0 && tempHostUiActive == 1 && hostUiActive == 0) // PC host has been activated
	{
		hostUiActive = 1;

		PORTF |= BIT(USB_SPI_CONTROL_GRANTED);

		//******************************************************************************
		// * make sure USB takes control back (USB input pin changed to high output pin)
		// * before MCU releases it (MCU high output pin changed to input pin).
		// *
		// *****************************************************************************

		disableMcuSharedMemoryInterface();
	}*/


#endif

#if 1

	/***************** REQUEST/RESPONSE functions ************************
	 *  COMPUTE MODULE DATA TRANSFER:
	 *
	 *****************************************************************************/
	// Make sure MCU is in normal pedal UI mode before servicing new or current requests
	if((newRequest == 1 || requestStatus != 0) /*&& (hostUiActive == 0) && (restoreFromHostUiMode == 0)*/)
	{
		switch(requestStatus)
		{
			case 0: // idle
				statusRequestLoopCount = 0;
				requestStatus = 1;
				break;
			case 1: // sending request
				newRequest = 0;
				statusRequestLoopCount++;
				if(computeModuleDataSendState == 0)
				{
					computeModuleDataSendState = 1;
				}
				else if(computeModuleDataSendState == 1) // start request
				{
					//clearSerialRam(0, 0, 1000);
					uint8_t addressHigh = 0x00FF & (sharedMemoryTxStartAddress>>8);
					uint8_t addressLow = 0x00FF & sharedMemoryTxStartAddress;

					enableMcuSharedMemorySectionAccess();
					SPCR &= ~BIT(SPIE); // disable SPI interrupt

					spiCommand[0]=2; // write command
					spiCommand[1]=addressHigh;
					spiCommand[2]=addressLow;
					spiCommand[3]=requestStatus;//computeModuleDataSendState;
					computeModuleDataSendState = 2;

					sharedMemoryTxBufferIndex = 0;
					SPCR |= BIT(SPIE); // enable SPI interrupt
					spiXferCount = 0;
					spiCommandIndex = 0;
					SPDR = spiCommand[spiCommandIndex++];//SPDR = spiCommand[0];
				}
				else if(computeModuleDataSendState == 2) // in-process
				{
					uint16_t tempAddress = sharedMemoryTxStartAddress + sharedMemoryTxBufferIndex;
					uint8_t addressHigh = 0x00FF & (tempAddress>>8);
					uint8_t addressLow = 0x00FF & tempAddress;

					enableMcuSharedMemorySectionAccess();
					SPCR &= ~BIT(SPIE); // disable SPI interrupt
					spiCommand[0]=2; // write command
					spiCommand[1]=addressHigh;
					spiCommand[2]=addressLow;
					spiCommand[3]=requestStatus;//computeModuleDataSendState;
					SPCR |= BIT(SPIE); // enable SPI interrupt
					spiXferCount = 0;
					spiCommandIndex = 0;
					SPDR = spiCommand[spiCommandIndex++];//SPDR = spiCommand[0];
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
				if(PINE & BIT(DATA_RXED0)) // processing complete
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
				responseError = 0;
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
					//delay(50);
					SPCR &= ~BIT(SPIE); // disable SPI interrupt

					spiCommand[0]=3;  // read command
					spiCommand[1]=addressHigh;
					spiCommand[2]=addressLow;
					spiCommand[3]=requestStatus;//computeModuleDataRetrievalState;
					computeModuleDataRetrievalState = 2;
					sharedMemoryRxBufferIndex = 0;
					SPCR |= BIT(SPIE); // enable SPI interrupt
					spiXferCount = 0;
					spiCommandIndex = 0;
					SPDR = spiCommand[spiCommandIndex++];//SPDR = spiCommand[0];
				}
				else if(computeModuleDataRetrievalState == 2) // in-process
				{
					statusRequestLoopCount++;
					/*if(sharedMemoryRxBuffer[0] == 0  )  // buffer is empty or corrupted
					{
						computeModuleDataRetrievalState = 3;
						responseError = 1;
					}
					else*/
					{
						uint16_t tempAddress = sharedMemoryRxStartAddress + sharedMemoryRxBufferIndex;
						uint8_t addressHigh = 0x00FF & (tempAddress>>8);
						uint8_t addressLow = 0x00FF & tempAddress;

						enableMcuSharedMemorySectionAccess();
						//delay(50);
						SPCR &= ~BIT(SPIE); // disable SPI interrupt
						spiCommand[0]=3;  // read command
						spiCommand[1]=addressHigh;
						spiCommand[2]=addressLow;
						spiCommand[3]=requestStatus;//0;//computeModuleDataRetrievalState;
						SPCR |= BIT(SPIE); // enable SPI interrupt
						spiXferCount = 0;
						spiCommandIndex = 0;
						SPDR = spiCommand[spiCommandIndex++];//SPDR = spiCommand[0];
					}
				}
				else if(computeModuleDataRetrievalState == 3) // done
				{
					//clearSerialRam(CM0_SHARED_MEMORY_SECTION_INDEX,0,1000/*sharedMemoryRxBufferCount+10*/); // clear CM memory to prevent repeat of last command
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

				if(getResponse == 0) requestStatus = 0;
				else if(currentDataString[0] != 0) // status update from CM0 received
				{
					updateStatus();//updateParamValues();
					//restoreFromHostUiMode = 1;
					requestStatus = 0;
				}
				else requestStatus = 0; // regular non-status response received


				break;
			default:;
		}
	}
#endif
	/*else if(restoreFromHostUiMode == 0)//clear out old request data
	{
		newRequest = 0;
		requestStatus = 0;
		clearBuffer(sendBuffer,50);
	}*/
	/*if(statusRequestLoopCount >= 1000 && requestStatus != 0) // break a stuck cycle
	{
		//if(requestStatus == 4) requestStatus = 0;
		//else requestStatus++;
		statusRequestLoopCount = 0;
	}*/
	/************************* COMPUTE MODULE DATA PROCESSING *********************

	if(computeModuleDataProcessingState == 0)
	{
		//idle
	}
	else if(computeModuleDataProcessingState == 1)// waiting
	{
		enableComputeModuleSharedMemorySectionAccess(0);
		//computeModuleDataProcessingState = 1;
		if(PINE & BIT(CM0_DATA_RXED)) // processing complete
		{
			computeModuleDataProcessingState = 2;
		}
	}
	else if(computeModuleDataProcessingState == 2)// processing is done
	{
		disableComputeModuleSharedMemorySectionAccess(0);//PORTF |= BIT(CM0_DATA_RDY);// set CM0_DATA_RDY line to high (inactive)
	}*/


	//PORTD &= ~BIT(FSW_LED1);
	//PORTD &= ~BIT(FSW_LED2);
	PORTE |= BIT(nMCU_DATA_RDY1);

	/*********************************************************************/
	TCNT0=0x00;
	USICR |= BIT(USITC);
}

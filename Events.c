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
extern void insertTask(uint8_t taskNumber);
extern uint8_t getTask(void);

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
	/*if((hostUiActive == 0) && (restoreFromHostUiMode == 0))
	{
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

		if(((PING & BIT(MCU_POWER_CONTROL)) == 0) )
		{
			powerOffCount++;
			if((powerOffCount > 100) && (powerOffEnable == 1))
			{
				// power off routine

				powerOffSignal = 1;
			}
		}
		else
		{
			powerOffCount = 0;
		}
	}*/


	// ******************** Host/Pedal UI switch ****************************
	/*if((PINA & BIT(7)) == BIT(7)) //Host GUI
	{
		hostUiActive = 1;
	}
	else // Pedal UI
	{
		hostUiActive = 0;
	}*/
#endif

#if 1
	/**********************************************************************
	 *
	 * 	Footswitches
	 *
	 **********************************************************************/
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
	if(requestStatus == 0) // requests are done
	{
		if(restoreFromHostUiMode == 0) // normal pedal UI running mode
		{
			//**************** Updating values from Pedal User Interface ***************
			/*if(valueChange == 1 && menuLevel == 3)
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
				newSpiXferRequest = 1;
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
				newSpiXferRequest = 1;
				currentDataUpdateTimer = 0;
			}*/

			//currentDataUpdateTimer++;
		}
	}


#endif

#if 1

	/***************** REQUEST/RESPONSE functions ************************
	 *  COMPUTE MODULE DATA TRANSFER:
	 *
	 *****************************************************************************/
	// Make sure MCU is in normal pedal UI mode before servicing new or current requests
	if((newSpiXferRequest == 1 || requestStatus != 0) /*&& (hostUiActive == 0) && (restoreFromHostUiMode == 0)*/)
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

				/*if(getResponse == 0) requestStatus = 0;
				else if(currentDataString[0] != 0) // status update from CM0 received
				{
					updateStatus();//updateParamValues();
					//restoreFromHostUiMode = 1;
					requestStatus = 0;
				}
				else requestStatus = 0; // regular non-status response received*/
				//requestStatus = 0;

				break;
			default:;
		}
	}
	else // NO TASKS ARE BEING PROCESSED, MCU IN IDLE STATE
	{
		if(currentDataUpdateTimer > CURRENT_DATA_UPDATE_TIMER_THRESHOLD)
		{
			currentDataUpdateTimer = 0;
			periodicTask = 1;	//requestStatusUpdateFromCM
		}
		else if(restoreFromHostUiMode == 0  && hostUiActive == 0) // normal pedal UI running mode
		{
			if(uiChange > 0)
			{
				clearBuffer(lcdBuffer[0], 20);
				clearBuffer(lcdBuffer[1], 20);
				// Update LCD Display from prior task output
				// LINE 0
				strncpy(lcdInitBuffer, nodeArray[currentComboNodeArrayIndex].name,10);
				if(menuLevel > 0)
				{
					strncat(lcdInitBuffer, "->",2);
					strncat(lcdInitBuffer, nodeArray[nodeArray[currentNodeArrayIndex].up].abbr,4);
				}
				if(menuLevel > 1)
				{
					strncat(lcdInitBuffer, "->",2);
					strncat(lcdInitBuffer, nodeArray[currentNodeArrayIndex].abbr,4);
				}
				strncpy(lcdBuffer[0],lcdInitBuffer,19);

				// LINE 1
				if(menuLevel == 3)
				{
					updateParamValues();
				}
				// LINE 2
				if(comboIndex == currentComboIndex)
				{
					strncpy(lcdBuffer[2], ofxMainStatusString,19);
				}
				else
				{
					ofxMainStatusString[0] = 0;
				}

				// LINE 3
				if(menuLevel == 0)
				{
					strncpy(lcdBuffer[3], " Save",6);//nodeArray[currentComboNodeArrayIndex].name,10);
				}
				else if(menuLevel == 1)
				{
					updateSoftKeyLabels();
				}
				else if(menuLevel == 2)
				{
					updateSoftKeyLabels();
				}
				else if(menuLevel == 3)
				{
					lcdBuffer[3][0] = 0;
				}

				Display("","","","");
				uiChange = 0;
			}
			// Read buttons

			if((PINA & 0x7F) != 0) // for de-bouncing switches
			{
				if(buttonDebounceCount == 0)
				{
					//buttonDebounceCount++;
					buttonPushed = 0;
					//uiTempButtons = 0;
				}
				else if(buttonDebounceCount == 4)
				{
					buttonPushed = PINA;
					//uiTempButtons = PINA;
				}
				buttonDebounceCount++;
			}
			else
			{
				buttonPushed = 0;
				//uiTempButtons = 0;
				buttonDebounceCount = 0;
			}

			if(((PING & BIT(MCU_POWER_CONTROL)) == 0) )
			{
				powerOffCount++;
				if((powerOffCount > 100) && (powerOffEnable == 1))
				{
					// power off routine

					powerOffSignal = 1;
				}
			}
			else
			{
				powerOffCount = 0;
			}

			// Insert new tasks into task queue here
			if(buttonPushed > 0)
			{
				if(menuLevel == 0)
				{
					if(buttonPushed == BIT(SOFT_KEY_1))
					{
						nonperiodicTask = 5; // saveCombo
					}
					else if(buttonPushed == BIT(SOFT_KEY_2))
					{
						//nonperiodicTask = 3; // utility
					}
					else if(buttonPushed == BIT(ROTARY_BUTTON))
					{
						nonperiodicTask = 4; // loadCombo
					}
					else if((buttonPushed == BIT(LCD_LEFT)) || (buttonPushed == BIT(LCD_RIGHT)))
					{
						nonperiodicTask = 2;	// browseComboTitles
					}
				}
				else if((menuLevel == 1) || (menuLevel == 2))
				{
					nonperiodicTask = 6;	// browseComboEffectParameters
				}
				buttonPushed = 0;
			}
			else if((count_up == 1) || (count_down == 1))
			{
				nonperiodicTask = 7; //changeComboEffectParameter
			}
		}
		else
		{
			if(strncmp(lcdBuffer[0],"PC GUI",6) != 0)
			{
				clearBuffer(lcdBuffer[0], 20);
				clearBuffer(lcdBuffer[1], 20);
				clearBuffer(lcdBuffer[2], 20);
				clearBuffer(lcdBuffer[3], 20);
				sprintf(lcdBuffer[0],"PC GUI enabled");
				sprintf(lcdBuffer[1],"Pedal UI disabled");
			}
		}
	}
	currentDataUpdateTimer++;

	runTask = 1;
#endif

	//PORTD &= ~BIT(FSW_LED1);
	//PORTD &= ~BIT(FSW_LED2);
	PORTE |= BIT(nMCU_DATA_RDY1);

	/*********************************************************************/
	TCNT0=0x00;
	USICR |= BIT(USITC);
}

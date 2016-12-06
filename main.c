/*
 * main.c
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/iom645.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "macros.h"
#include "GlobalVars.h"

#include "PinDrivers.h"
#include "ChipInit.h"
#include "ComputeModuleFuncts.h"
#include "UserInterface.h"
#include "JsonFuncts.h"
#include "Events.h"
#include "utilityFuncts.h"
#include "Tasks.h"
#define NULL 0
/*const char *testJson =
	"{\"title\":\"Combo 1\",\"effects\":"
			"[{\"abbr\":\"fx0\",\"name\":\"effect 0\",\"params\":"
				"[{\"abbr\":\"gn00\",\"name\":\"gain0_0\",\"value\":90,\"type\":0},"
				"{\"abbr\":\"gn01\",\"name\":\"gain0_1\",\"value\":73,\"type\":0}]"
			"}]"
		"}";*/

/*const char *testJson =
		"{\"title\":\"combo 1\",\"effects\":"
				"[{\"abbr\":\"fx0\",\"name\":\"effect 0\",\"params\":"
					"[{\"abbr\":\"gn00\",\"name\":\"gain0_0\",\"value\":0,\"type\":0},"
					"{\"abbr\":\"gn01\",\"name\":\"gain0_1\",\"value\":83,\"type\":0},"
					"{\"abbr\":\"gn02\",\"name\":\"gain0_2\",\"value\":0,\"type\":0}]"
				"},"
				"{\"abbr\":\"fx1\",\"name\":\"effect 1\",\"params\":"
					"[{\"abbr\":\"gn10\",\"name\":\"gain1_0\",\"value\":0,\"type\":0},"
					"{\"abbr\":\"gn11\",\"name\":\"gain1_1\",\"value\":20,\"type\":0},"
					"{\"abbr\":\"gn12\",\"name\":\"gain1_2\",\"value\":0,\"type\":0}]"
				"}]"
			"}";*/


#define dbgUi 0

//#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
//#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096

#define SHARED_MEMORY_SECTION_SIZE 4096
#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1
#define TASK_QUEUE_SIZE 20
#define CM0_PRESENT 1

extern void breakButtonUi();
extern void flash_init();
extern void flash_WREN();

uint8_t rx_done;

extern void delay(unsigned long delay);
extern void clearBuffer(char *buffer, int length);
void insertTask(uint8_t taskNumber);
uint8_t getTask(void);

void main(void)
{
	uint8_t gitTestVar = 0;
	 uint8_t state,model,fsw,param,value,value1,value2,value3;
	 uint8_t temp;
	 unsigned int int_temp;
	 unsigned long long_temp;
	 uiButton = 0;
	 //uint8_t LCD_change;
	 //uint8_t new_val;
	 //uint8_t buttonPushed,prev_buttonPushed;
	 //uint8_t screen = 0;
	 //uint8_t test_buf[9];
	 //uint8_t test_buf2[9];
	 //uint8_t instr_count;
	 //uint8_t count1 = 0;
	 /*uint8_t ModelMax = 5;
	 uint8_t StageMax = 3;
	 uint8_t EffectMax = 3;
	 uint8_t ParamMax = 3;
	 uint8_t i8, j8, k8;
	 unsigned int i,j,k;
	 unsigned int address;
	 unsigned int addr1, addr2, data_in;
	 unsigned int data_out;
	 unsigned long word;*/
	 newSpiXferRequest = 0;
		/*struct _node *currentComboNode;
		struct _node *currentEffectNode;
		struct _node *currentParamNode;
		uint8_t numBuffer[50];*/

		timerTestCount = 0;

	 //SP = 0x10FF;	// Initialize stack-pointer
	#if(dbgUi == 0)
	 config_chip();
	 initSerialRam();


	 /********** Reset LAN **********

	 PORTF &= ~BIT(ETHER_nRST);
	 delay(10000);
	 PORTF |= BIT(ETHER_nRST);*/


	 while((PING & BIT(MCU_POWER_CONTROL)) == 0); // wait until power button is released



	 /********** Reset CM **********/

	 /*PORTG &= ~BIT(CMX_RUN);
	 delay(10000);
	 PORTG |= BIT(CMX_RUN);*/

	 LCD_init();
	  //param_lut_flash_pointer_init(0);
	 //stage_lut_flash_pointer_init(0,0);
	#endif

	 /*tx_count = 0;
	 rx_count = 0;*/
	 count_up = 0;
	 count_down = 0;


	 main_state = 0;
	 state_change=0;
	effect_change=0;
	value_change=0;
	param_change=0;
	 main_model=0;
	 main_effect=0;
	 main_param=0;
	 menuLevel=0;
	 nodeCount=0;
	 model_change = 1;
	 valueChange = 0;
	 currentComboIndex = 0;
	 comboIndex = 0;
	 LCD_change = 0;
	 buttonPushed = 0;
	 nodeArrayIndex = 0;
	 computeModuleDataRetrievalState = 0;
	 powerOffEnable = 0;
	 computeModuleDataSendState = 0;

	 computeModuleDataProcessingState = 0;

	 sharedMemoryRxStartAddress = MCU_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;
	 sharedMemoryTxStartAddress = 0;
	 spiXferSizeLimit = 200;
	 spiXferCount = 0;
	 loadMenu = 0;
	 //shareMemoryOutIndex = 0;
	 //clearBuffer(sharedMemoryOut,400);
	 /*rx_in_progress = 0;
	 rx_wait_timer = 0;*/
	 uiTempButtons = 0;
	 buttonDebounceCount = 0;
	 requestStatus = 0;
	 responseError = 0;
	 softkeyStringFrameIndex[0] = 0;
	 softkeyStringFrameIndex[1] = 0;
	 softkeyStringFrameCount = 0;
	 getResponse = 0;
	 hostUiActive = 0;
	 tempHostUiActive = 0;
	 restoreFromHostUiMode = 0;
	 lcdBufferHash[0]=0;
	 lcdBufferHash[1]=0;
	 lcdBufferHash[2]=0;
	 lcdBufferHash[3]=0;
	 fsw1.physicalStatus = 0;
	 fsw1.value = 0;
	 fsw1.change = 0;
	 fsw2.physicalStatus = 0;
	 fsw2.value = 0;
	 fsw2.change = 0;
	 /*strcpy(globalError.file, "");
	 strcpy(globalError.function, "");
	 globalError.position = 0;*/
	 uiChange = 0;
	 valueString[4] = 0;
	 powerOffSignal = 0;
	 newCombo = 1;
	 count_dir = 0;
	 taskWriteIndex = 0;
	 taskReadIndex = 0;
	 taskWriteIndexWrappedAround = 0;
	 periodicTask = 0;
	 nonperiodicTask = 0;
	 nextNonperiodicTask = 0;
	 runTask = 0;

	 strncpy(testNode.abbr, "ABBR", 5);
	 strncpy(testNode.name, "nodeName", 11);


	 strcpy(debugString,"0000000000");


	 clearBuffer(taskQueue, 20);

	 lcdBuffer[0][0] = 0;
	 strcpy(lcdBuffer[1], "      OFX      ");
	 strcpy(lcdBuffer[2], "Starting up....");
	 lcdBuffer[3][0] = 0;

	 Display("","","","");

#if CM0_PRESENT
	 while((PINE & (BIT(CM_RUNNING0))) == 0);
#endif
	 lcdBuffer[0][0] = 0;
	 strcpy(lcdBuffer[1], "      OFX      ");
	 strcpy(lcdBuffer[2], "Starting up1...");
	 lcdBuffer[3][0] = 0;

	 Display("","","","");

#if CM0_PRESENT
	 while((PINE & (BIT(OFX_RDY0))) == 0);  // wait for OfxMain on CM
#endif
	 lcdBuffer[0][0] = 0;
	 strcpy(lcdBuffer[1], "      OFX      ");
	 strcpy(lcdBuffer[2], "Starting up2...");
	 lcdBuffer[3][0] = 0;

	 Display("","","","");


	 /*char spiBuffer[20];
	 clearBuffer(spiBuffer, 20);

	 clearBuffer(jsonBuffer,JSON_BUFFER_SIZE);
	 clearBuffer(sendBuffer,50);
	 clearBuffer(getCombosBuffer,170);
	 sharedMemoryRxBuffer = getCombosBuffer;  // set sharedMemory pointer to jsonBuffer array
	 sharedMemoryRxStartAddress = MCU_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//CM0_SHARED_MEMORY_SECTION_ADDRESS;
	 sharedMemoryTxBuffer = sendBuffer;
	 sharedMemoryTxStartAddress = CM0_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//MCU_SHARED_MEMORY_SECTION_ADDRESS;
*/
	 SEI();
#if !CM0_PRESENT
	 while(powerOffSignal == 0)
	 {
		 SPI_Tx(0x55);
	 }
#else
	 /*strcpy(sendBuffer, "listCombos");
	 sendBuffer[49] = 255;

	 getResponse = 1;
	 newSpiXferRequest = 1;
	 //requestStatus = 1;
	 while(requestStatus < 4); // wait for response to listCombos request
	 requestStatus = 0;*/
	 lcdBuffer[0][0] = 0;
	 strcpy(lcdBuffer[1], "      OFX      ");
	 strcpy(lcdBuffer[2], "Starting up3...");
	 lcdBuffer[3][0] = 0;

	 Display("","","","");
	 comboCount = 0;
	 getComboList();
	 /*strcpy(comboTitle[comboCount], strtok(getCombosBuffer,","));

	 while(comboTitle[comboCount][0] != 0 && comboCount < 10)
	 {
		 comboCount++;
		 strcpy(comboTitle[comboCount], strtok(NULL,","));
	 }*/

	 /*lcdBuffer[0][0] = 0;
	 //strcpy(lcdBuffer[1], "-------------------"); // use this as width reference
	 strcpy(lcdBuffer[1], "        OFX       ");
	 strcpy(lcdBuffer[2], "loading init combo");
	 lcdBuffer[3][0] = 0;
	 Display("","","","");*/

	 loadCombo();


	 /*clearBuffer(lcdBuffer[0],19);
	 clearBuffer(lcdBuffer[1],19);
	 clearBuffer(lcdBuffer[2],19);
	 clearBuffer(lcdBuffer[3],19);*/
	 powerOffEnable = 1;
	 nonperiodicTask = 0;
	 while(powerOffSignal == 0) //while((PINE & (BIT(CM_RUNNING0))) != 0)
	 {
		 if(runTask == 1)
		 {
			 if(periodicTask > 0)
			 {
				 switch(periodicTask)
				 {
					 case 1:
						 requestStatusUpdateFromCM();
						 break;
					 case 2:
						 break;
					 case 3:
						 break;
					 case 4:
						 break;
					 default:;
				 }
				 periodicTask = 0;
			 }
			 else if(nonperiodicTask > 0)
			 {
				 switch(nonperiodicTask)
				 {
					 case 1:
						 getComboList();
						 break;
					 case 2:
						 browseComboTitles();
						 break;
					 case 3:
						 changeJackParameters();
						 break;
					 case 4:
						 loadCombo();
						 break;
					 case 5:
						 saveCombo();
						 break;
					 case 6:
						 browseComboEffectParameters();
						 break;
					 case 7:
						 changeComboEffectParameter();
						 break;
					 case 8:
						 requestStatusUpdateFromCM();
						 break;
					 case 9:

						 break;
					 case 10:
						 testTask1();
						 break;
					 case 11:
						 testTask2();
						 break;
					 case 12:
						 testTask3();
						 break;
					 case 13:
						 testTask4();
						 break;

					 default:;
				 }
				 if(nextNonperiodicTask == 0)
					 nonperiodicTask = 0;
				 else
				 {
					 nonperiodicTask = nextNonperiodicTask;
					 nextNonperiodicTask = 0;
				 }
				 runTask = 0;
			 }

		 }
		 /*processPedalUI();
		 delay(20000);

			//****************************************************************************
			// * 	Value updating
			// *
			// *
			// *

		if(loadMenu == 1)
		{
			jsonBuffer2UiMenu();
			loadMenu = 2;
			processPedalUI();
		}

		if (LCD_change == 1)
		{
			Display("","","","");
			//Display(lcdBuffer[0], lcdBuffer[1], lcdBuffer[2], lcdBuffer[3]);
			LCD_change = 0;
			buttonPushed = 0;
		}*/

	}
#endif
	 lcdBuffer[0][0] = 0;
	strncpy(lcdBuffer[1],"powering off     ",19);
	 lcdBuffer[2][0] = 0;
	 lcdBuffer[3][0] = 0;
	 Display("","","","");

	//if((PINE & (BIT(CM_RUNNING0))) != 0)
	{
			clearBuffer(sendBuffer,50);
			sendBuffer[49]=255;
			strcpy(sendBuffer,"powerOff");//strcpy(sendBuffer,"getCurrentValues");
			sharedMemoryRxBuffer = currentDataString;  // set sharedMemory pointer to currentDataString
			sharedMemoryRxStartAddress = MCU_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;
			sharedMemoryTxBuffer = sendBuffer;
			sharedMemoryTxStartAddress = CM0_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//MCU_SHARED_MEMORY_SECTION_ADDRESS;
			getResponse = 1;
			//requestStatus = 1;
			newSpiXferRequest = 1;
			while(requestStatus < 4);
	}
	while((PINE & (BIT(CM_RUNNING0))) != 0);
	 lcdBuffer[0][0] = 0;
	 strncpy(lcdBuffer[1], "powered off      ",19);
	 strncpy(lcdBuffer[1], "release button   ",19);
	 lcdBuffer[3][0] = 0;
	 Display("","","","");
	 while((PING & BIT(MCU_POWER_CONTROL)) == 0); // wait until power button is released
	 delay(1000);

	 PORTG &= ~BIT(nPOWER_OFF);

}


void insertTask(uint8_t taskNumber)
{
	if(((taskReadIndex > 0) && (taskWriteIndex == taskReadIndex-1)) ||
		((taskReadIndex == 0) && (taskWriteIndex == TASK_QUEUE_SIZE)))
	{
		// too many tasks in queue
	}
	else
	{
		taskQueue[taskWriteIndex] = taskNumber;

		if(taskWriteIndex < TASK_QUEUE_SIZE)
		{
			taskWriteIndex++;
		}
		else
		{
			taskWriteIndexWrappedAround = 1;
			taskWriteIndex = 0;
		}
	}
}

uint8_t getTask(void)
{
	uint8_t taskNumber = 0;

	if((taskReadIndex < taskWriteIndex) || ((taskReadIndex == 0) && (taskWriteIndex == TASK_QUEUE_SIZE)))
	{
		taskNumber = taskQueue[taskReadIndex];
		if(taskReadIndex < TASK_QUEUE_SIZE)
		{
			taskReadIndex++;
		}
		else
		{
			taskWriteIndexWrappedAround = 0;
			taskReadIndex = 0;
		}
	}
	else
	{
		// no tasks in queue
	}

	return taskNumber;
}

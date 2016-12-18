/*
 * Tasks.c
 *
 *  Created on: Dec 5, 2016
 *      Author: buildrooteclipse
 */

#include "utilityFuncts.h"
#include "GlobalVars.h"
//#include "values.h"
#include "Tasks.h"
#include <avr/iom645.h>
#define NULL 0
//#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
//#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define SHARED_MEMORY_SECTION_SIZE 4096
#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1

typedef uint8_t uint8_t;
extern void clearBuffer(char *buffer, int length);
extern void clearBuffer(char *buffer, int length);

uint8_t testTask1(void)
{
	uint8_t status = 0;

	strncpy(lcdBuffer[0], "test task 1", 19);
	uiChange = 1;
	nextNonperiodicTask = 11;
	return status;
}

uint8_t testTask2(void)
{
	uint8_t status = 0;

	strncpy(lcdBuffer[1], "test task 2", 19);
	uiChange = 1;
	nextNonperiodicTask = 12;

	return status;
}


uint8_t testTask3(void)
{
	uint8_t status = 0;

	strncpy(lcdBuffer[2], "test task 3", 19);
	uiChange = 1;
	nextNonperiodicTask = 13;

	return status;
}

uint8_t testTask4(void)
{
	uint8_t status = 0;

	strncpy(lcdBuffer[3], "idling", 19);
	uiChange = 1;
	nextNonperiodicTask = 0;

	return status;
}



uint8_t getComboList()
{
	uint8_t status = 0;
	//uint8_t comboCount = 0;

	 clearBuffer(sendBuffer, 50);
	 clearBuffer(jsonBuffer,JSON_BUFFER_SIZE);
	 clearBuffer(getCombosBuffer,170);
	 sharedMemoryRxBuffer = getCombosBuffer;  // set sharedMemory pointer to jsonBuffer array
	 sharedMemoryRxStartAddress = MCU_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//CM0_SHARED_MEMORY_SECTION_ADDRESS;
	 sharedMemoryTxBuffer = sendBuffer;
	 sharedMemoryTxStartAddress = CM0_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//MCU_SHARED_MEMORY_SECTION_ADDRESS;

	 strcpy(sendBuffer, "listCombos");
	 sendBuffer[49] = 255;

	 getResponse = 1;
	 newSpiXferRequest = 1;
	 //requestStatus = 1;
	 while(requestStatus < 4); // wait for response to listCombos request
	 requestStatus = 0;

	 /*strcpy(comboTitle[comboCount], strtok(getCombosBuffer,","));

	 while(comboTitle[comboCount][0] != 0 && comboCount < 10)
	 {
		 comboCount++;
		 strcpy(comboTitle[comboCount], strtok(NULL,","));
	 }*/

	 for(comboCount = 0;  comboCount < 10; comboCount++)
	 {
		 if(comboCount == 0)
		 {
			 strcpy(comboTitle[comboCount], strtok(getCombosBuffer,","));
		 }
		 else
		 {
			 strcpy(comboTitle[comboCount], strtok(NULL,","));
		 }
		 if(comboTitle[comboCount][0] == 0) break;
	 }

	return status;
}

uint8_t browseComboTitles(void)
{
	uint8_t status = 0;

	if(hostUiActive == 0)
	{

		switch(buttonPushed)
		{
			case BIT(LCD_LEFT): // moveLeftLcd
				if(comboIndex > 0)
				{
					comboIndex--;
					getComboName(comboIndex);
					//getCombo(comboIndex);
				}
			break;
			case BIT(LCD_RIGHT): // moveRightLcd
				if(comboIndex < (comboCount-1))  // limit to comboCount-1 because incrementing beyond will go beyond count.
				{
					comboIndex++;
					getComboName(comboIndex);
				}

			break;
			default:;
		}
		uiChange = 1;
		buttonPushed = 0;
	}
	return status;
}

uint8_t changeJackParameters(void)
{
	uint8_t status = 0;


	return status;
}

uint8_t loadCombo(void)
{
	uint8_t status = 0;

	currentComboIndex = comboIndex;
	getCombo(currentComboIndex);

	if(jsonBuffer2UiMenu() != 0)
	{
		strcpy(ofxMainStatusString,"parse error");
	}

	uiChange = 1;
	return status;
}

uint8_t saveCombo(void)
{
	uint8_t status = 0;

	powerOffEnable = 0;
	clearBuffer(sendBuffer,50);
	strncpy(sendBuffer, "saveCombo", 9);
	sendBuffer[49] = 255;
	sharedMemoryTxBuffer = sendBuffer;
	sharedMemoryTxStartAddress = CM0_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//MCU_SHARED_MEMORY_SECTION_ADDRESS;
	getResponse = 1;

	newSpiXferRequest = 1;
	//requestStatus = 1;
	while(requestStatus < 4); // wait for response to listCombos request
	requestStatus = 0;
	//clearBuffer(lcdBuffer[2],20);
	//strncpy(lcdBuffer[2],"saving combo", 12);
	strncpy(ofxMainStatusString,"saving combo", 12);
	//Display(0,0,0,0);
	powerOffEnable = 1;
	uiChange = 1;
	return status;
}

uint8_t browseComboEffectParameters(void)
{
	uint8_t status = 0;

	switch(buttonPushed)
	{
		case BIT(LCD_LEFT): // moveLeftLcd
			if(menuLevel == 1)
			{
				if(softkeyStringFrameIndex[0] > 0) softkeyStringFrameIndex[0]--;
			}
			else if(menuLevel == 2)
			{
				if(softkeyStringFrameIndex[1] > 0) softkeyStringFrameIndex[1]--;
			}
		break;
		case BIT(SOFT_KEY_1): // softKey1
			if(goDown(0+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiButton = BIT(SOFT_KEY_1);
		break;
		case BIT(SOFT_KEY_2): // softKey2
			if(goDown(1+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiButton = BIT(SOFT_KEY_2);
		break;
		case BIT(SOFT_KEY_3): // softKey3
			if(goDown(2+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiButton = BIT(SOFT_KEY_3);
		break;
		case BIT(SOFT_KEY_4): // softKey4
			if(goDown(3+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiButton = BIT(SOFT_KEY_4);
		break;
		case BIT(ROTARY_BUTTON): // combo select: push parameter knob
			if(goUp() == 0) uiButton = BIT(ROTARY_BUTTON);
		break;
		case BIT(LCD_RIGHT): // moveRightLcd
			if(menuLevel == 1)
			{
				if(softkeyStringFrameIndex[0] < softkeyStringFrameCount) softkeyStringFrameIndex[0]++;
			}
			else if(menuLevel == 2)
			{
				if(softkeyStringFrameIndex[1] < softkeyStringFrameCount) softkeyStringFrameIndex[1]++;
			}
		break;
		default:;
	}

	uiChange = 1;
	//buttonPushed = 0;
	return status;
}

/*uint8_t selectParameter(void)
{
	uint8_t status = 0;

	switch(buttonPushed)
	{
		case BIT(SOFT_KEY_1): // softKey1
			if(goDown(0+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiButton = BIT(SOFT_KEY_1);
		break;
		case BIT(SOFT_KEY_2): // softKey2
			if(goDown(1+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiButton = BIT(SOFT_KEY_2);
		break;
		case BIT(SOFT_KEY_3): // softKey3
			if(goDown(2+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiButton = BIT(SOFT_KEY_3);
		break;
		case BIT(SOFT_KEY_4): // softKey4
			if(goDown(3+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiButton = BIT(SOFT_KEY_4);
		break;
		default:;
	}

	uiChange = 1;

	return status;
}

uint8_t goBack(void)
{
	uint8_t status = 0;

	goUp();

	return status;
}*/

uint8_t changeComboEffectParameter(void)
{
	uint8_t status = 0;

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
			newSpiXferRequest = 1;
			while(requestStatus < 4); // wait for response to listCombos request
			requestStatus = 0;
		}
	}

	return status;
}

uint8_t requestStatusUpdateFromCM(void)
{
	//uint8_t status = 0;

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
	while(requestStatus < 4); // wait for response to listCombos request
	requestStatus = 0;

	return updateStatus();
}


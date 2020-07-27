/*
 * UserInterfaceOps.c
 *
 *  Created on: Jul 19, 2020
 *      Author: mike
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "macros.h"
#include "utilityFuncts.h"
#include "UserInterfaceDrivers.h"
#include "UserInterfaceOps.h"
#include "ComboNodes.h"
#include "TaskQueue.h"
#include "Typedefs.h"


#define LCD_LINE_0_DELIMITER ':'
#define TEMP_STATUS_STRING_LENGTH 50
#define COMBO_LIST_BUFFER_LENGTH 150


/*** PORTA ***/
// button lines
#define LCD_LEFT 	6
#define SOFT_KEY_1	5
#define SOFT_KEY_2	4
#define ROTARY_BUTTON 3
#define SOFT_KEY_3	2
#define SOFT_KEY_4	1
#define LCD_RIGHT 	0




static uint8_t menuLevel;

static char lcdInitBuffer[25];
static char lcdBuffer[4][20];
static uint8_t uiButton;
static char valueString[5];


static uint8_t hostUiActive;
static uint8_t restoreFromHostUiMode;

static char softkeyString[100];
static uint8_t softkeyStringFrameIndex[2];
static uint8_t softkeyStringFrameCount;



/*
 * 0: no buttons pressed
 * 1: button pressed, LCD not updated
 * 2: button released, LCD not updated
 * 3: button not released, LCD updated
 * 4: button released, LCD updated
 *
 */
static uint8_t uiChange;
static char ofxMainStatusString[20];
static char comboTitle[15][11];
static uint8_t comboIndex;
static uint8_t currentComboIndex;
static uint8_t hostComboIndex;
static uint8_t comboCount;

static char comboListBuffer[COMBO_LIST_BUFFER_LENGTH];


uint8_t browseComboTitles(void)
{
	uint8_t status = 0;

	if(hostUiActive == 0)
	{
		switch(getButtonPushed())
		{
		case BIT(LCD_LEFT): // moveLeftLcd
			if(comboIndex > 0)
			{
				comboIndex--;
				getComboName(comboIndex);
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

	}
	return status;
}



void initUserInterfaceOps(void)
{

	menuLevel = 0;
	memset(lcdInitBuffer,0,sizeof(char)*25);
	memset(lcdBuffer,0,sizeof(char)*80);
	uiButton = 0;
	memset(valueString,0,sizeof(char)*5);

	hostUiActive = 0;
	restoreFromHostUiMode = 0;

	memset(softkeyString,0,sizeof(char)*100);
	softkeyStringFrameIndex[0] = 0;
	softkeyStringFrameIndex[1] = 0;
	softkeyStringFrameCount = 0;
	uiChange = 0;
	memset(ofxMainStatusString,0,sizeof(char)*20);
	memset(comboTitle,0,sizeof(char)*15*11);
	comboIndex = 0;
	currentComboIndex = 0;
	hostComboIndex = 0;
	comboCount = 0;

}

uint8_t browseComboEffectParameters(void)
{
	uint8_t status = 0;

	switch(getButtonPushed())
	{
	uint8_t tempMenuLevel = 0;
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
		if((tempMenuLevel = incrementMenuLevel(0+softkeyStringFrameIndex[menuLevel-1]*4,menuLevel)) != 255)
		{
			uiButton = BIT(SOFT_KEY_1);
		}
	break;
	case BIT(SOFT_KEY_2): // softKey2
		if((tempMenuLevel = incrementMenuLevel(1+softkeyStringFrameIndex[menuLevel-1]*4, menuLevel)) != 255)
		{
			uiButton = BIT(SOFT_KEY_2);
			menuLevel = tempMenuLevel;
		}
	break;
	case BIT(SOFT_KEY_3): // softKey3
		if((tempMenuLevel = incrementMenuLevel(2+softkeyStringFrameIndex[menuLevel-1]*4,menuLevel)) != 255)
		{
			uiButton = BIT(SOFT_KEY_3);
			menuLevel = tempMenuLevel;
		}
	break;
	case BIT(SOFT_KEY_4): // softKey4
		if((tempMenuLevel = incrementMenuLevel(3+softkeyStringFrameIndex[menuLevel-1]*4,menuLevel)) != 255)
		{
			uiButton = BIT(SOFT_KEY_4);
			menuLevel = tempMenuLevel;
		}
	break;
	case BIT(ROTARY_BUTTON): // combo select: push parameter knob
		if((tempMenuLevel = decrementMenuLevel(menuLevel)) != 255)
		{
			uiButton = BIT(ROTARY_BUTTON);
			menuLevel = tempMenuLevel;
		}
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
	return status;
}


void updateSoftKeyLabels(void)
{
	Node_t currentNode = getCurrentNode();
	uint8_t  tempNodeIndexArray = currentNode.down;

	if(tempNodeIndexArray != 255) // fill soft key LCD info
	{

		/************* group softkey items into softkeyGroupArray *******************/

		char abbrString[5];
		if((menuLevel != 0) && (menuLevel != 3))
		{
			uint8_t paramCount = 0;
			memset(softkeyString, 0, sizeof(char)*100);
			for( ; (tempNodeIndexArray != 255); tempNodeIndexArray = getNode(tempNodeIndexArray).right)
			{
				fillBuffer(abbrString, getNode(tempNodeIndexArray).abbr, 4); //fills in unused bytes with spaces to keep labels aligned with soft keys
				strncat(softkeyString, abbrString, 4);
				if(getNode(tempNodeIndexArray).right != 255)
				{
					softkeyString[strlen(softkeyString)] = ' ';
				}
				paramCount++;
			}
			softkeyStringFrameCount = paramCount/4;
			strcpy(lcdBuffer[3], " ");
			strncat(lcdBuffer[3], softkeyString+softkeyStringFrameIndex[menuLevel-1]*20, 19);
		}

	}

}


char *getComboName(uint8_t index)
{
	return comboTitle[index];
}


void parseComboList(void)
{
	for(comboCount = 0;  comboCount < 10; comboCount++)
	{
		if(comboCount == 0)
		{
			strcpy(comboTitle[comboCount], strtok(comboListBuffer,","));
		}
		else
		{
			strcpy(comboTitle[comboCount], strtok(NULL,","));
		}
		if(comboTitle[comboCount][0] == 0) break;
	}
}


char *getComboListBufferPtr(void)
{
	return comboListBuffer;
}


uint8_t getMenuLevel(void)
{
	return menuLevel;
}


void setMenuLevel(uint8_t menuLvl)
{
	menuLevel = menuLvl;
}


void updateDisplay(uint8_t hostUiActive)
{
	if(uiChange > 0)
	{
		if(hostUiActive == 0)
		{
			Node_t currentNode = getCurrentNode();
			Node_t parentNode = getParentNode();
			memset(lcdBuffer[0], 0, sizeof(char)*20);
			memset(lcdBuffer[1], 0, sizeof(char)*20);
			memset(lcdInitBuffer, 0, sizeof(char)*25);
			// Update LCD Display from prior task output
			// ***************************** LINE 0 **********************************
			strncpy(lcdInitBuffer, getComboNode().name,10);
			if(menuLevel == 2)
			{
				lcdInitBuffer[strlen(lcdInitBuffer)] = LCD_LINE_0_DELIMITER;
				strncat(lcdInitBuffer, currentNode.abbr,4);
			}
			else if(menuLevel == 3)
			{
				lcdInitBuffer[strlen(lcdInitBuffer)] = LCD_LINE_0_DELIMITER;
				strncat(lcdInitBuffer, parentNode.abbr,4);
				lcdInitBuffer[strlen(lcdInitBuffer)] = LCD_LINE_0_DELIMITER;
				strncat(lcdInitBuffer, currentNode.abbr,4);
			}
			strncpy(lcdBuffer[0],lcdInitBuffer,19);

			// ***************************** LINE 1 *****************************
			if(menuLevel == 3)
			{
				updateParamValues();
			}
			// ***************************** LINE 2 *****************************
			if(menuLevel == 0)
			{
				snprintf(lcdBuffer[2],19,"%s",ofxMainStatusString);
			}
			else
			{
				lcdBuffer[2][0] = 0;
			}
			// ***************************** LINE 3 *****************************
			if(menuLevel == 0)
			{
				memset(lcdBuffer[3], 0, sizeof(char)*19);
				strcpy(lcdBuffer[3], " Save");
				lcdBuffer[3][strlen(lcdBuffer[3])] = ' ';
				strcat(lcdBuffer[3], "Host");

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
			writeDisplay(lcdBuffer[0],lcdBuffer[1],lcdBuffer[2],lcdBuffer[3]);
		}
		else
		{
			writeDisplay("PC GUI enabled","","","      Pdl ");
		}

		uiChange = 0;
	}
}

uint8_t updateParamValues(void)
{
	uint8_t status = 0;

	Node_t currentNode = getCurrentNode();
	strncpy(lcdInitBuffer, currentNode.name,13);
	strncat(lcdInitBuffer,":",1);
	memset(valueString, 0, sizeof(char)*5);
	getValueString(currentNode.value,currentNode.valueType, valueString);
	strncat(lcdInitBuffer, valueString ,4);
	strncpy(lcdBuffer[1],lcdInitBuffer,19);

	uint8_t valueIndexMax;
	if(currentNode.valueType == 0) valueIndexMax = 99;
	else if(currentNode.valueType == 1) valueIndexMax = 74;
	else if(currentNode.valueType == 2) valueIndexMax = 99;
	else if(currentNode.valueType == 3) valueIndexMax = 99;

	if(getEncoderDirection() == 1 && currentNode.value < valueIndexMax)
	{
		currentNode.value++;
	}
	else if(getEncoderDirection() == -1 && currentNode.value > 0)
	{
		currentNode.value--;
	}
	setValueChange(1);

	return status;
}

char parsedCurrentDataString[3][30];

void updateStatus(char *statusString)
{

	char comboIndexString[2];
	char tempOfxMainStatusString[15];
	char tempStatusString[100];
	memset(tempStatusString, 0, sizeof(char)*100);
	strncpy(tempStatusString,statusString,100);

	strcpy(parsedCurrentDataString[0], strtok(tempStatusString,"|"));
	strcpy(parsedCurrentDataString[1], strtok(NULL,"|"));
	strcpy(parsedCurrentDataString[2], strtok(NULL,"\0"));

	strtok(parsedCurrentDataString[0],":");
	strcpy(comboIndexString, strtok(NULL,"\0"));
	hostComboIndex = atoi(comboIndexString);

	if(comboIndex == currentComboIndex && menuLevel == 0)
	{
		strtok(parsedCurrentDataString[1],":");
		strncpy(tempOfxMainStatusString,strtok(NULL,"\0"),15);
		if(strlen(tempOfxMainStatusString) > 2 && strncmp(tempOfxMainStatusString, ofxMainStatusString,15) != 0)
		{
			strncpy(ofxMainStatusString, tempOfxMainStatusString, 15);
			uiChange = 1;
		}
	}
}


uint8_t getPedalUiMode(void)
{
	if(hostUiActive == 0) return 1;
	else return 0;
}


uint8_t isPedalGoingBackToNormalUiMode(void)
{
	return restoreFromHostUiMode;
}

void setPedalUiMode(void)
{
	restoreFromHostUiMode = 0;
}


void createTasks(void)
{
	uint8_t buttonPushed = getButtonPushed();
	uint8_t encoderDirection = getEncoderDirection();
	if(restoreFromHostUiMode == 0  && hostUiActive == 0) // normal pedal UI running mode
	{
		// Insert new tasks into task queue here

		if(buttonPushed > 0)
		{
			if(menuLevel == 0)
			{
				if(buttonPushed == BIT(SOFT_KEY_1))
				{
					insertTask(SaveCombo); // saveCombo
				}
				else if(buttonPushed == BIT(SOFT_KEY_2)) // enable host connection
				{
					hostUiActive = 1;
					uiChange = 1;
					buttonPushed = 0;
				}
				else if(buttonPushed == BIT(ROTARY_BUTTON))
				{
					if(comboIndex != currentComboIndex)
						insertTask(LoadCombo); // loadCombo
					else
						buttonPushed = 0; // no task assigned, so reset to 0

					menuLevel = 1;
				}
				else if((buttonPushed == BIT(LCD_LEFT)) || (buttonPushed == BIT(LCD_RIGHT)))
				{
					browseComboTitles();
				}
			}
			else if((menuLevel == 1))
			{
				if(buttonPushed == BIT(ROTARY_BUTTON))
				{
					menuLevel = 0;
					buttonPushed = 0; // no task assigned, so reset to 0
				}
			}

			uiChange = 1;
		}
		else if(encoderDirection != 0)//((count_up == 1) || (count_down == 1))
		{
			insertTask(ChangeComboEffectParameter);
			uiChange = 1;
		}
	}
	else // in Host PC mode
	{
		// Insert new tasks into task queue here
		if(buttonPushed > 0)
		{
			if(menuLevel == 0)
			{
				if(buttonPushed == BIT(SOFT_KEY_2)) // disable host connection
				{
					restoreFromHostUiMode = 1;
					comboIndex = hostComboIndex;
					currentComboIndex = hostComboIndex;
					insertTask(GetComboList);  //getComboList followed by getCombo
					insertTask(LoadCombo);
					hostUiActive = 0;
				}
			}
		}
	}

}


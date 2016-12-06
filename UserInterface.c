/*
 * UserInterface.c
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */
#include "UserInterface.h"
//#include "ComputeModuleFuncts.h"
#include "utilityFuncts.h"
#include "GlobalVars.h"
#include "values.h"
#include <avr/iom645.h>
#define NULL 0
//#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
//#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define SHARED_MEMORY_SECTION_SIZE 4096
#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1

typedef uint8_t uint8_t;
extern void clearBuffer(char *buffer, int length);
//extern void clearBuffer(char *buffer, int length);

#define dbgUi 0
extern const char *testJson;
//uint8_t uiBtn;

/*static void breakButtonUi(char *buffer1, char *buffer2, char *buffer3, char *buffer4)
{

	char tempBuffer[4][20];
	clearBuffer(tempBuffer[0],20);
	clearBuffer(tempBuffer[1],20);
	clearBuffer(tempBuffer[2],20);
	clearBuffer(tempBuffer[3],20);

	strncpy(tempBuffer[0],buffer1,20);
	strncpy(tempBuffer[1],buffer2,20);
	strncpy(tempBuffer[2],buffer3,20);
	strncpy(tempBuffer[3],buffer4,20);

	CLI();

	clearBuffer(lcdBuffer[0],16);
	clearBuffer(lcdBuffer[1],16);
	clearBuffer(lcdBuffer[2],16);
	clearBuffer(lcdBuffer[3],16);

	strncpy(lcdBuffer[0], tempBuffer[0],16);
	strncpy(lcdBuffer[1], tempBuffer[1],16);
	strncpy(lcdBuffer[2], tempBuffer[2],16);
	strncpy(lcdBuffer[3], tempBuffer[3],16);
	Display(lcdBuffer[0], lcdBuffer[1], lcdBuffer[2], lcdBuffer[3]);

	while(PINA == 0);
	while(PINA != 0);
	SEI();

}*/


extern uint8_t rx_done;

extern void delay(unsigned long delay);
/*{
 unsigned long i;

 for (i = 0; i < delay; i++)
 {
  NOP();
 }
}*/

/*void clearBuffer(char *buffer, uint8_t size)
{
	for(uint8_t i = 0; i < size; i++) buffer[i] = 0;
}*/

#if(dbgUi == 0)
/***********************************************************
 *			Writing data uint8_ts to LCD Display
 ***********************************************************/
void LCD_data(uint8_t data)
{
	//if('0' <= data && data <= '~')
	{
		PORTC = data;
		LcdDataEn();
		LcdWrite();
		//  LcdInstrEn();
		delay(10);  //60
		PORTC = 0x00;
	}
}

/***********************************************************
 *			Writing instructions to LCD Display
 ***********************************************************/
void LCD_instr(uint8_t data)
{
	PORTC = data;
	LcdInstrEn();
	LcdWrite();
	delay(50);  //300
	PORTC = 0x00;

}



/*uint8_t RotaryDir(void)
{
	uint8_t dir;

	if (count_up == 1)
	{
		dir = 1;
	}
	else if (count_down == 1)
	{
		dir = 2;
	}
	count_up = 0;
	count_down = 0;

	return dir;
}*/


/***********************************************************
 *		Inc/Dec effect parameter value
 *		count MUST be a global variable
 ***********************************************************/
void RotCount(uint8_t dir, uint8_t *count)
{
	uint8_t change;

	if ((dir == 1) && (*count < 100))
	{
		(*count)++;
	}
	else if ((dir == 2) && (*count > 0))
	{
		(*count)--;
	}
}


/***********************************************************
 *					Initializing LCD Display
 ***********************************************************/
void LCD_init(void)
{
	delay(2000); //1000
	LCD_instr(0x030);
	delay(666); //333
	LCD_instr(0x030);
	delay(200); //100
	LCD_instr(0x030);
	delay(200); //100
	LCD_instr(0b00111000);
	LCD_instr(0x010);
	LCD_instr(0x001);
	LCD_instr(0b00000110);
	LCD_instr(0b00001100);
}
#endif

/*void UI_IndBuf_load(uint8_t state, uint8_t index, uint8_t index_max)
{
	uint8_t i,result,remainder,num_abbr;

	result = index/3;
	remainder = index - 3*result;
	IndBuf[0] = '\0';



	switch(state)
	{
		case 2: // effect ABBRs
				for(i = 0; i < 3; i++)
				{
					if ((3*result+i) < index_max)
					{
						//flash_array_read(stage[main_stage].fx[3*result+i].flash_label_addr + 2,abbr_buffer,4);
						strcat(IndBuf," ");
						strcat(IndBuf,abbr_buffer);
					}
					else
					{
						strcat(IndBuf,"     ");
					}
				}
				break;
		case 3:	// parameter ABBRs
				for(i = 0; i < 3; i++)
				{
					if ((3*result+i) < index_max)
					{
						//flash_array_read(stage[main_stage].fx[main_effect].param[3*result+i].flash_label_addr + 2,abbr_buffer,4);
						strcat(IndBuf," ");
						strcat(IndBuf,abbr_buffer);
					}
					else
					{
						strcat(IndBuf,"     ");
					}
				}
				break;
		default:
				break;
	}
}


void CapInd(char *target, char *source, uint8_t select)
{
	 uint8_t char_count, sp_count, i;
	 sp_count = 0;

	while(select >= 3)
	{
		select = select - 3;
	}

	for(char_count = 0; char_count < 16; char_count++)
	{
		if (isalpha(source[char_count]))
		{
			if(sp_count == (select + 1))
			{
				target[char_count] = source[char_count] - 32;
			}
			else
			{
				target[char_count] = source[char_count];
			}
		}
		else if (source[char_count] == ' ')
		{
			sp_count++;
			target[char_count] = source[char_count];
		}
		else if ((source[char_count] == '-') && (sp_count == (select + 1)))
		{
			target[char_count] = '*';
		}
		else
		{
			target[char_count] = source[char_count];
		}
	}
 	target[16] = '\0';
}*/


/***********************************************************
 *		Writing strings to LCD Display
 ***********************************************************/
void Display(uint8_t *line_1, uint8_t *line_2, uint8_t *line_3, uint8_t *line_4)
{
	uint8_t n=0;
	uint8_t i=0;
	uint8_t line1[20];
	uint8_t line2[20];
	uint8_t line3[20];
	uint8_t line4[20];

	strncpy(line1,lcdBuffer[0],20);
	strncpy(line2,lcdBuffer[1],20);
	strncpy(line3,lcdBuffer[2],20);
	strncpy(line4,lcdBuffer[3],20);
	int lineHash;

#if(dbgUi == 0)
	lineHash = 0;
	for(i = 0; line1[i] != 0; i++)
	{
		lineHash += line1[i];
	}
	if(line1 != 0)//if(lineHash != lcdBufferHash[0])
	{
		lcdBufferHash[0] = lineHash;
		LCD_instr(0x01);
		LCD_instr(0b00000110);
		while((*(line1 + n) != '\0') && (n < 20))
		{
			LCD_data(*(line1 + n));
			//delay(500);
			n++;
		}
		/*for(i = n; i < 20; i++)
	 {
		 LCD_data(' ');
	 }*/
	}

	lineHash = 0;
	for(i = 0; line2[i] != 0; i++)
	{
		lineHash += line2[i];
	}
	if(line2 != 0)//if(lineHash != lcdBufferHash[1])
	{
		lcdBufferHash[1] = lineHash;
		n=0;
		LCD_instr(SET_DDRAM_ADDRESS + LCD_LINE2_ADDR);
		while((*(line2 + n) != '\0') && (n < 20))
		{
			LCD_data(*(line2 + n));
			//delay(200);
			n++;
		}
		/*for(i = n; i < 20; i++)
	 {
		 LCD_data(' ');
	 }*/
	}

	lineHash = 0;
	for(i = 0; line3[i] != 0; i++)
	{
		lineHash += line3[i];
	}
	if(line3 != 0)//if(lineHash != lcdBufferHash[2])
	{
		lcdBufferHash[2] = lineHash;
		n=0;
		LCD_instr(SET_DDRAM_ADDRESS + LCD_LINE3_ADDR);
		while (*(line3 + n) != '\0')
		{
			LCD_data(*(line3 + n));
			//delay(200);
			n++;
		}
		//for(i = n; i < 20; i++)
		//{
		//	 LCD_data(' ');
		//}
	}

	lineHash = 0;
	for(i = 0; line4[i] != 0; i++)
	{
		lineHash += line4[i];
	}
	if(line4 != 0)//if(lineHash != lcdBufferHash[3])
	{
		lcdBufferHash[3] = lineHash;
		n=0;
		LCD_instr(SET_DDRAM_ADDRESS + LCD_LINE4_ADDR);
		while((*(line4 + n) != '\0') && (n < 20))
		{
			LCD_data(*(line4 + n));
			//delay(200);
			n++;
		}
		/*for(i = n; i < 20; i++)
	 {
		 LCD_data(' ');
	 }*/
	}

#else
	printf("***********************************************");
	printf("LCD0: %s\n", line1);
	printf("LCD1: %s\n", line2);
	printf("***********************************************");
#endif
}


/***********************************************************
 *		Writing strings to LCD Display
 ***********************************************************/
void DisplayStatus(uint8_t *line3)
{
	uint8_t n=0;
	uint8_t i=0;

	int lineHash;

	lineHash = 0;
	for(i = 0; line3[i] != 0; i++)
	{
		lineHash += line3[i];
	}
	//if(lineHash != lcdBufferHash[2])
	{
		lcdBufferHash[2] = lineHash;
		n=0;
		LCD_instr(SET_DDRAM_ADDRESS + LCD_LINE3_ADDR);
		while((*(line3 + n) != '\0') && (n < 20))
		{
			LCD_data(*(line3 + n));
			//delay(200);
			n++;
		}
		//for(i = n; i < 20; i++)
		//{
		//	 LCD_data(' ');
		//}
	}


}

uint8_t goUp(void)
{
	uint8_t status = 0;

	if(nodeArray[currentNodeArrayIndex].up != 255)
	{
		currentNodeArrayIndex = nodeArray[currentNodeArrayIndex].up;
		menuLevel--;

		if(menuLevel == 2) // effect
		{
			currentEffectNodeArrayIndex = currentNodeArrayIndex;
			currentParamNodeArrayIndex = 0;
		}
		else if(menuLevel == 3) // param
		{
			currentParamNodeArrayIndex = currentNodeArrayIndex;
		}
	}
	else status = 1;

	return status;
}
uint8_t goDown(uint8_t index)
{
	uint8_t i;
	uint8_t status = 0;

	if(nodeArray[currentNodeArrayIndex].down != 255)
	{
		currentNodeArrayIndex = nodeArray[currentNodeArrayIndex].down;
		for(i = 0; i<index; i++)
		{
			if(nodeArray[currentNodeArrayIndex].right != 255)
				currentNodeArrayIndex = nodeArray[currentNodeArrayIndex].right;
		}
		menuLevel++;

		if(menuLevel == 2) // effect
		{
			currentEffectNodeArrayIndex = currentNodeArrayIndex;
		}
		else if(menuLevel == 3) // param
		{
			currentParamNodeArrayIndex = currentNodeArrayIndex;
		}

		softkeyStringFrameIndex[menuLevel-1] = 0;
	}
	else status = 1;

	return status;
}

void getValueString(uint8_t valueIndex, uint8_t valueType, char *valueStr)
{
	//char *valueStr;

	PGM_P p;
	int i;

	if(valueType == 0)
	{
		memcpy_P(&p, &amp[valueIndex], sizeof(PGM_P));
		strncpy_P(valueStr, p, 4);
		//strncpy(valueStr,amp[valueNode->value],4);
	}

	else if(valueType == 1)
	{
		//strncpy_P(valueStr, (PGM_P)pgm_read_word(&(freq[valueIndex])),5);
		memcpy_P(&p, &freq[valueIndex], sizeof(PGM_P));
		strncpy_P(valueStr, p, 4);
		//strncpy(valueStr,freq[valueNode->value],4);
	}

	else if(valueType == 2)
	{
		//strncpy_P(valueStr, (PGM_P)pgm_read_word(&(time[valueIndex])),5);
		memcpy_P(&p, &time[valueIndex], sizeof(PGM_P));
		strncpy_P(valueStr, p, 4);
		//strncpy(valueStr,time[valueNode->value],4);
	}

	//Display(valueStr," "," "," ");

	//return valueStr;
}

void updateSoftKeyLabels(void)
{
	uint8_t  tempNodeIndexArray = nodeArray[currentNodeArrayIndex].down;

	if(tempNodeIndexArray != 255) // fill soft key LCD info
	{
		/************* group softkey items into softkeyGroupArray *******************/

		char abbrString[5];
		if(menuLevel == 0 || menuLevel == 3)
		{
			/*strncpy(fsw1Abbr,"fsw1",4);
	  strncpy(fsw2Abbr,"fsw2",4);

	  sprintf(lcdBuffer[3], "%s|%s", fsw1Abbr, fsw2Abbr);*/
		}
		else
		{
			uint8_t paramCount = 0;
			clearBuffer(softkeyString,100);
			for( ; (tempNodeIndexArray != 255); tempNodeIndexArray = nodeArray[tempNodeIndexArray].right)
			{
				fillBuffer(abbrString, nodeArray[tempNodeIndexArray].abbr, 4); //fills in unused bytes with spaces to keep labels aligned with soft keys
				strncat(softkeyString, abbrString, 4);
				//strncat(softkeyString, nodeArray[tempNodeIndexArray].abbr, 4);
				if(nodeArray[tempNodeIndexArray].right != 255) strncat(softkeyString,"|",1);
				paramCount++;
			}
			softkeyStringFrameCount = paramCount/4;
			//clearBuffer(lcdBuffer[3], 20);
			strcpy(lcdBuffer[3], " ");
			strncat(lcdBuffer[3], softkeyString+softkeyStringFrameIndex[menuLevel-1]*20, 19);
		}
	}

}

void getComboName(uint8_t index)
{
	menuLevel = 0;
	//uiBtn = BIT(CHANGE_COMBO);
	strcpy(nodeArray[0].name, comboTitle[index]);
}


void getCombo(uint8_t index)
{
	powerOffEnable = 0;
	debugString[0] = '1';
	//strcpy(nodeArray[0].name, comboTitle[index]);
	//send "getCombo" request to OfxMain via shared memory
	clearBuffer(jsonBuffer,JSON_BUFFER_SIZE);
	clearBuffer(sendBuffer,50);
	//clearSerialRam(0, 0, 1000);
	strncpy(sendBuffer, "getCombo:", 9);
	strncat(sendBuffer, comboTitle[index], 10);

	sendBuffer[49] = 255;
	jsonBuffer[JSON_BUFFER_SIZE-1] = 255;
	sharedMemoryTxBuffer = sendBuffer;
	sharedMemoryTxStartAddress = CM0_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//MCU_SHARED_MEMORY_SECTION_ADDRESS;
	sharedMemoryRxBuffer = jsonBuffer;  // set sharedMemory pointer to jsonBuffer array
	sharedMemoryRxStartAddress = MCU_SHARED_MEMORY_SECTION_INDEX*SHARED_MEMORY_SECTION_SIZE;//CM0_SHARED_MEMORY_SECTION_ADDRESS;
	getResponse = 1;
	//while(requestStatus != 0);
	newSpiXferRequest = 1;
	//requestStatus = 1;
	//clearBuffer(lcdBuffer[2],20);
	//strncpy(lcdBuffer[2],"loading combo", 19);
	strncpy(ofxMainStatusString,"loading combo", 19);
	//Display(0,0,0,0);
	while(requestStatus < 4); // wait for response to getCombo request
	requestStatus = 0;
	//parse combo data into node array
	//clearSerialRam(0, 0, 1000);
	menuLevel = 0;
	loadMenu = 1;
	powerOffEnable = 1;
	//LCD_change = 1;
}

/*void saveCombo(void)
{
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
	clearBuffer(lcdBuffer[2],20);
	strncpy(lcdBuffer[2],"saving combo", 12);
	strncpy(ofxMainStatusString,"saving combo", 12);
	Display(0,0,0,0);
	powerOffEnable = 1;
	//while(requestStatus < 4); // wait for response to listCombos request
	//requestStatus = 0;
}*/

uint8_t selectCombo = 100;

/*void processPedalUI()
{
	char string[50];

	//uint8_t uiButton;

	static uint8_t uiBtn;
	if(hostUiActive == 0)
	{
//	  clearBuffer(lcdBuffer[0], 20);
//	  clearBuffer(lcdBuffer[1], 20);
//	  clearBuffer(lcdBuffer[2], 20);
//	  clearBuffer(lcdBuffer[3], 20);

		if(restoreFromHostUiMode == 1)
		{
			uiBtn = BIT(ROTARY_BUTTON);
			menuLevel = 0;
			uiChange = 1;
			//restoreFromHostUiMode = 0;  //change to zero in Rotary Encoder button case statement
		}
		else
		{
			uiButton = buttonPushed;
		}

		switch(uiButton)
		{
			case BIT(LCD_LEFT): // moveLeftLcd
						uiBtn = BIT(LCD_LEFT);
				if(menuLevel == 0)
				{
					loadMenu = 0;
					if(comboIndex > 0)
					{
						comboIndex--;
						getComboName(comboIndex);
						//getCombo(comboIndex);
					}
				}
				else if(menuLevel == 1)
				{
					if(softkeyStringFrameIndex[0] > 0) softkeyStringFrameIndex[0]--;
					//softkeyStringFrameIndex[1] = 0;
				}
				else if(menuLevel == 2)
				{
					if(softkeyStringFrameIndex[1] > 0) softkeyStringFrameIndex[1]--;
				}
				else
				{

				}
			uiChange = 1;
			break;
			case BIT(SOFT_KEY_1): // softKey1
				if(menuLevel == 0)
				{
					saveCombo();
				}
				else//if(menuLevel != 0)
				{
					if(goDown(0+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiBtn = BIT(SOFT_KEY_1);
					//softkeyStringFrameIndex[0] = 0;
				}
				uiChange = 1;
			break;
			case BIT(SOFT_KEY_2): // softKey2
				if(menuLevel != 0)
				{

					if(goDown(1+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiBtn = BIT(SOFT_KEY_2);
					//softkeyStringFrameIndex[0] = 0;
				}
				uiChange = 1;
			break;
			case BIT(SOFT_KEY_3): // softKey3
				if(menuLevel != 0)
				{

					if(goDown(2+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiBtn = BIT(SOFT_KEY_3);
					//softkeyStringFrameIndex[0] = 0;
				}
				uiChange = 1;
			break;
			case BIT(SOFT_KEY_4): // softKey4
				if(menuLevel != 0)
				{

					if(goDown(3+softkeyStringFrameIndex[menuLevel-1]*4) == 0) uiBtn = BIT(SOFT_KEY_4);
					//softkeyStringFrameIndex[0] = 0;
				}
				uiChange = 1;
			break;
			case BIT(LCD_RIGHT): // moveRightLcd
				uiBtn = BIT(LCD_RIGHT);
				if(menuLevel == 0)
				{
					loadMenu = 0;
					if(comboIndex < (comboCount-1))  // limit to comboCount-1 because incrementing beyond will go beyond count.
					{
						comboIndex++;
						getComboName(comboIndex);
						//getCombo(comboIndex);
					}
				}
				else if(menuLevel == 1)
				{
					if(softkeyStringFrameIndex[0] < softkeyStringFrameCount) softkeyStringFrameIndex[0]++;
					//softkeyStringFrameIndex[1] = 0;
				}
				else if(menuLevel == 2)
				{
					if(softkeyStringFrameIndex[1] < softkeyStringFrameCount) softkeyStringFrameIndex[1]++;
				}

				uiChange = 1;
			break;
			case BIT(ROTARY_BUTTON): // lcdUp
				uiBtn = BIT(ROTARY_BUTTON);
				//softkeyStringFrameIndex[0] = 0;
				if(menuLevel == 0)
				{
					if(restoreFromHostUiMode == 1)
					{
						currentComboIndex = comboIndex;
						getCombo(currentComboIndex);
						restoreFromHostUiMode = 0;
					}
					else if(comboIndex != currentComboIndex)
					{
						currentComboIndex = comboIndex;
						getCombo(currentComboIndex);
					}
					else
					{
						menuLevel = 1;//goDown(0);
//						if(comboIndex != currentComboIndex)
//						{
//							currentComboIndex = comboIndex;
//							getCombo(currentComboIndex);
//						}
					}
				}
				else if(menuLevel == 1)
				{
					menuLevel = 0;//goUp();
				}
				else
				{
					goUp();
				}

				uiChange = 1;
			break;
			default:;
		}

		if(newCombo == 1)
		{
			menuLevel = 0;
			uiChange = 1;
		}
		//PORTF |= BIT(CM1_DATA_RDY);
		if(uiChange == 1)//if(((uiBtn < 200) && (uiBtn > 0)) || (restoreFromHostUiMode == 1))
		{
			clearBuffer(lcdBuffer[0], 20);
			clearBuffer(lcdBuffer[1], 20);
			//snprintf(lcdBuffer[2],"node count: %d", nodeCount);

			switch(menuLevel)
			{
				case 0:
					strncpy(lcdBuffer[0], nodeArray[currentComboNodeArrayIndex].name,10);
					//snprintf(lcdBuffer[0],"%d:%s", menuLevel, nodeArray[currentComboNodeArrayIndex].name, 20);
					//sprintf(lcdBuffer[2],"loadMenu: %d", loadMenu);
					if(comboIndex == currentComboIndex)
					{
						strncpy(lcdBuffer[2], ofxMainStatusString,19);
					}
					else
					{
						ofxMainStatusString[0] = 0;
					}
					strncpy(lcdBuffer[3], " Save",6);//nodeArray[currentComboNodeArrayIndex].name,10);
					//snprintf(lcdBuffer[3]," Save",6);
					break;
				case 1:
					strncpy(lcdBuffer[0], nodeArray[currentComboNodeArrayIndex].name,10);
					//strcpy(lcdBuffer[0], nodeArray[currentComboNodeArrayIndex].name);
					//snprintf(lcdBuffer[0],"%d:%s", menuLevel, nodeArray[currentComboNodeArrayIndex].name, 19);
					break;
				case 2:
					//clearBuffer(lcdBuffer[1], 20);
					//sprintf(lcdBuffer[0],"%s->%s", nodeArray[currentComboNodeArrayIndex].name,nodeArray[currentNodeArrayIndex].abbr);
					strncpy(lcdInitBuffer, nodeArray[currentComboNodeArrayIndex].name,10);
					strncat(lcdInitBuffer, "->",2);
					strncat(lcdInitBuffer, nodeArray[currentNodeArrayIndex].abbr,4);
					strncpy(lcdBuffer[0],lcdInitBuffer,19);
					break;
				case 3:
					clearBuffer(lcdBuffer[3], 20);
					//sprintf(lcdBuffer[0],"%s->%s->%s", nodeArray[currentComboNodeArrayIndex].name,nodeArray[nodeArray[currentNodeArrayIndex].up].abbr, nodeArray[currentNodeArrayIndex].abbr);
					strncpy(lcdInitBuffer, nodeArray[currentComboNodeArrayIndex].name,10);
					strncat(lcdInitBuffer, "->",2);
					strncat(lcdInitBuffer, nodeArray[nodeArray[currentNodeArrayIndex].up].abbr,4);
					strncat(lcdInitBuffer, "->",2);
					strncat(lcdInitBuffer, nodeArray[currentNodeArrayIndex].abbr,4);
					strncpy(lcdBuffer[0],lcdInitBuffer,19);

					strncpy(lcdInitBuffer, nodeArray[currentNodeArrayIndex].name,15);
					strncat(lcdInitBuffer,":",1);
					clearBuffer(valueString,5);
					getValueString(nodeArray[currentNodeArrayIndex].value,
							nodeArray[currentNodeArrayIndex].valueType, valueString);
					strncat(lcdInitBuffer, valueString ,4);
					strncpy(lcdBuffer[1],lcdInitBuffer,19);
					break;
				default:;
			}
			rotaryValue = nodeArray[currentComboNodeArrayIndex].value;
			uiBtn = 200;
			//buttonPushed = 0;
			//clearBuffer(lcdBuffer[3], 20);

			updateSoftKeyLabels();
//			uint8_t  tempNodeIndexArray = nodeArray[currentNodeArrayIndex].down;
//
//			if(tempNodeIndexArray != 255) // fill soft key LCD info
//			{
//				//************* group softkey items into softkeyGroupArray *******************
//
//				char abbrString[5];
//				if(menuLevel == 0 || menuLevel == 3)
//				{
//				}
//				else
//				{
//					uint8_t paramCount = 0;
//					clearBuffer(softkeyString,100);
//					for( ; (tempNodeIndexArray != 255); tempNodeIndexArray = nodeArray[tempNodeIndexArray].right)
//					{
//						fillBuffer(abbrString, nodeArray[tempNodeIndexArray].abbr, 4); //fills in unused bytes with spaces to keep labels aligned with soft keys
//						strncat(softkeyString, abbrString, 4);
//						//strncat(softkeyString, nodeArray[tempNodeIndexArray].abbr, 4);
//						if(nodeArray[tempNodeIndexArray].right != 255) strncat(softkeyString,"|",1);
//						paramCount++;
//					}
//					softkeyStringFrameCount = paramCount/4;
//					//clearBuffer(lcdBuffer[3], 20);
//					strcpy(lcdBuffer[3], " ");
//					strncat(lcdBuffer[3], softkeyString+softkeyStringFrameIndex[menuLevel-1]*20, 19);
//				}
//			}
			uiChange = 0;

			LCD_change = 1;
			newCombo = 0;

		}

		//breakButton(1);
		else if((count_up || count_down) && menuLevel == 3)  // rotary encoder value change
		{
			updateParamValues();
//			strncpy(lcdInitBuffer, nodeArray[currentComboNodeArrayIndex].name,10);
//			strncat(lcdInitBuffer, "->",2);
//			strncat(lcdInitBuffer, nodeArray[nodeArray[currentNodeArrayIndex].up].abbr,4);
//			strncat(lcdInitBuffer, "->",2);
//			strncat(lcdInitBuffer, nodeArray[currentNodeArrayIndex].abbr,4);
//			strncpy(lcdBuffer[0],lcdInitBuffer,19);
//
//			strncpy(lcdInitBuffer, nodeArray[currentNodeArrayIndex].name,13);
//			strncat(lcdInitBuffer,":",1);
//			clearBuffer(valueString,5);
//			getValueString(nodeArray[currentNodeArrayIndex].value,
//					nodeArray[currentNodeArrayIndex].valueType, valueString);
//			strncat(lcdInitBuffer, valueString ,4);
//			strncpy(lcdBuffer[1],lcdInitBuffer,19);
//
//			uint8_t valueIndexMax;
//			if(nodeArray[currentNodeArrayIndex].valueType == 0) valueIndexMax = 99;
//			else if(nodeArray[currentNodeArrayIndex].valueType == 1) valueIndexMax = 74;
//			else if(nodeArray[currentNodeArrayIndex].valueType == 2) valueIndexMax = 99;
//			else if(nodeArray[currentNodeArrayIndex].valueType == 3) valueIndexMax = 99;
//
//			if(count_up == 1 && count_down == 0 && nodeArray[currentNodeArrayIndex].value < valueIndexMax) nodeArray[currentNodeArrayIndex].value++;
//			else if(count_up == 0 && count_down == 1 && nodeArray[currentNodeArrayIndex].value > 0) nodeArray[currentNodeArrayIndex].value--;
//
//			count_up = 0;
//			count_down = 0;
//
//			valueChange = 1;
//			LCD_change = 1;
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
			//delay(100000);
			LCD_change = 1;
		}
	}
}*/


uint8_t updateParamValues(void)
{
	uint8_t status = 0;

	strncpy(lcdInitBuffer, nodeArray[currentNodeArrayIndex].name,13);
	strncat(lcdInitBuffer,":",1);
	clearBuffer(valueString,5);
	getValueString(nodeArray[currentNodeArrayIndex].value,
			nodeArray[currentNodeArrayIndex].valueType, valueString);
	strncat(lcdInitBuffer, valueString ,4);
	strncpy(lcdBuffer[1],lcdInitBuffer,19);

	uint8_t valueIndexMax;
	if(nodeArray[currentNodeArrayIndex].valueType == 0) valueIndexMax = 99;
	else if(nodeArray[currentNodeArrayIndex].valueType == 1) valueIndexMax = 74;
	else if(nodeArray[currentNodeArrayIndex].valueType == 2) valueIndexMax = 99;
	else if(nodeArray[currentNodeArrayIndex].valueType == 3) valueIndexMax = 99;

	if(count_up == 1 && count_down == 0 && nodeArray[currentNodeArrayIndex].value < valueIndexMax) nodeArray[currentNodeArrayIndex].value++;
	else if(count_up == 0 && count_down == 1 && nodeArray[currentNodeArrayIndex].value > 0) nodeArray[currentNodeArrayIndex].value--;

	count_up = 0;
	count_down = 0;
	valueChange = 1;


	return status;
}

char parsedCurrentDataString[3][30];

uint8_t updateStatus(void)
{
	uint8_t status = 0;
	//char parsedCurrentDataString[50];
	char comboIndexString[2];
	tempHostUiActive = 0;
	char tempOfxMainStatusString[15];
	//char comboName[15];
	//uint8_t comboIndex = 0;

	strcpy(parsedCurrentDataString[0], strtok(currentDataString,"|"));
	strcpy(parsedCurrentDataString[1], strtok(NULL,"|"));
	strcpy(parsedCurrentDataString[2], strtok(NULL,"\0"));
	//strcpy(parsedCurrentDataString, strtok(currentDataString,"|"));

	strtok(parsedCurrentDataString[0],":");
	strcpy(comboIndexString, strtok(NULL,"\0"));
	//comboIndex = atoi(comboIndexString);

	//sprintf(lcdBuffer[2],"index:%d current:%d", comboIndex, currentComboIndex);

	//strcpy(parsedCurrentDataString, strtok(currentDataString,"|"));

	//strcpy(parsedCurrentDataString, strtok(currentDataString,"|"));
	/*strtok(parsedCurrentDataString[1],":");
	char *intString;
	for(int i = 0; i < 20; i++)
	{
		intString = strtok(NULL,",");
		if(intString == 0) break;
		nodeArray[paramIndex2nodeArrayIndex[i]].value = atoi(intString);
	}*/

	//if(comboIndex == currentComboIndex && menuLevel == 0)
	{
		strtok(parsedCurrentDataString[1],":");
		strncpy(tempOfxMainStatusString,strtok(NULL,"\0"),15);
		if(strlen(tempOfxMainStatusString) > 2 && strncmp(tempOfxMainStatusString, ofxMainStatusString,15) != 0)
		{
			strncpy(ofxMainStatusString, tempOfxMainStatusString, 15);
			uiChange = 1;//LCD_change = 1;
		}
		//strncpy(lcdBuffer[2], ofxMainStatusString,19);

	}


	//strcpy(parsedCurrentDataString, strtok(currentDataString,"|"));
	strtok(parsedCurrentDataString[2],":");
	tempHostUiActive = atoi(strtok(NULL,"\0"));
	if(tempHostUiActive == 0 && hostUiActive == 1) // web app has been deactivated
	{
		comboIndex = atoi(comboIndexString);
		restoreFromHostUiMode = 1;
		hostUiActive = 0;
	}
	else if (tempHostUiActive == 1 && hostUiActive == 0) // web app has been activated
	{
		hostUiActive = 1;

	}
	for(uint8_t i = 0; i < 100; i++) currentDataString[i] = 0;

	return status;

}

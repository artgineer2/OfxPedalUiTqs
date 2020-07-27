/*
 * UserInterface.c
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ChipDefs.h"
#include "utilityFuncts.h"
#include "Typedefs.h"
#include "values.h"
#include "macros.h"
#include "UserInterfaceDrivers.h"



#define SHARED_MEMORY_SECTION_SIZE 4096
#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1


#define LCD_LINE1_ADDR 0x00
#define LCD_LINE2_ADDR 0x40
#define LCD_LINE3_ADDR 0x14
#define LCD_LINE4_ADDR 0x54
#define SET_DDRAM_ADDRESS 0x80

/*** PORTB ***/
#define LCD_RS		6
#define LCD_RW		5
#define LCD_E		4

/*** PORTD ***/
#define FSW2 		5
#define FSW1 		4
#define FSW2_LED		3
#define FSW1_LED		2
#define ROT_A		1
#define ROT_B		0

/*** PORTG ***/
#define nPOWER_OFF 0
#define MCU_POWER_CONTROL 1
#define CMX_RUN 2


//static uint8_t EdgeDetected = 0;  // 0:falling edge, 1: rising edge
static uint8_t edgeIndex = 0;
static uint8_t level[2][2] = {{0,0},{0,0}};

static uint8_t valueChange;
static uint8_t buttonPushed;
static int8_t encoderDirection;
static uint8_t buttonDebounceCount;
static uint8_t powerOffSignal;
static uint8_t powerOffEnable;
static Fsw_t fsw1;
static Fsw_t fsw2;

static uint8_t count_up;
static uint8_t count_down;

#define dbgUi 0

#if(dbgUi == 0)



/*	@brief: Initialize ports for the UI buttons, LCD, footswitches, and power button and control line
 *
 * 	@returns: none
 */
void initPortPins(void)
{

	PORTA.pinDirection  = 0b00000000;// button lines
	PORTA.output = 0b00000000;

	PORTB.pinDirection  |= BIT(LCD_E)|BIT(LCD_RS)|BIT(LCD_RW);
	PORTB.output = 0;

	PORTC.pinDirection  = 0b11111111; // LCD data lines
	PORTC.output = 0b00000000;

	PORTD.pinDirection  |= BIT(FSW2_LED)|BIT(FSW1_LED);
	PORTD.output |= BIT(FSW2_LED)|BIT(FSW1_LED);

	PORTG.pinDirection |= BIT(MCU_POWER_CONTROL)|BIT(nPOWER_OFF);
	PORTG.output |= BIT(MCU_POWER_CONTROL)|BIT(nPOWER_OFF);

}

// @brief: pulses the E line to execute LCD commands
void writeLcd(void)
{
	PORTB.output |= BIT(LCD_E);
	delay(800); //200
	PORTB.output &= ~BIT(LCD_E);
}

void enableLcdData(void)	//Set R/S pin high
{
	PORTB.output |= BIT(LCD_RS);
}

void enableLcdInstr(void)	  //Set R/S pin low
{
	PORTB.output &= ~BIT(LCD_RS);
}

/***********************************************************
 *			Writing data uint8_ts to LCD Display
 ***********************************************************/
void writeLcdData(uint8_t data)
{
	PORTC.output = data;
	enableLcdData();
	writeLcd();
	delay(10);  //60
	PORTC.output = 0x00;
}

/***********************************************************
 *			Writing instructions to LCD Display
 ***********************************************************/
void writeLcdInstr(uint8_t data)
{
	PORTC.output = data;
	enableLcdInstr();
	writeLcd();
	delay(50);  //300
	PORTC.output = 0x00;

}


/***********************************************************
 *					Initializing LCD Display
 ***********************************************************/
void initLcdDisplay(void)
{
	delay(2000); //1000
	writeLcdInstr(0x030);
	delay(666); //333
	writeLcdInstr(0x030);
	delay(200); //100
	writeLcdInstr(0x030);
	delay(200); //100
	writeLcdInstr(0b00111000);
	writeLcdInstr(0x010);
	writeLcdInstr(0x001);
	writeLcdInstr(0b00000110);
	writeLcdInstr(0b00001100);
}
#endif


/*	@brief: Initialize peripherals and connected components for User Interface module
 *
 * 	@returns: none
 */
void initUserInterfaceDrivers(void)
{

	initPortPins();
	initLcdDisplay();

	valueChange = 0;
	buttonPushed = 0;
	buttonDebounceCount = 0;
	powerOffSignal = 0;
	powerOffEnable = 0;

	memset(&fsw1,0,sizeof(Fsw_t));
	memset(&fsw2,0,sizeof(Fsw_t));

	count_up = 0;
	count_down = 0;

}


void readButtons(void)
{
	static uint8_t uiBtn;
	if(((PORTA.input & 0x7F) != 0)) // for de-bouncing switches
	{
		if(uiBtn == 0) // one-push-one-action functioning
		{
			if(buttonDebounceCount == 0)
			{
				buttonPushed = 0;
			}
			else if(buttonDebounceCount < 2)
			{
				buttonPushed = PORTA.input;
				uiBtn = 1;
			}
			buttonDebounceCount++;
		}
	}
	else
	{
		buttonPushed = 0;
		uiBtn = 0;
		buttonDebounceCount = 0;
	}
}

uint8_t getButtonPushed(void)
{
	return buttonPushed;
}

void readEncoder(void)
{


	if(count_up == 1 && count_down == 0) encoderDirection = 1;
	else if(count_up == 0 && count_down == 1)  encoderDirection = -1;

	count_up = 0;
	count_down = 0;


}

void getValueString(uint8_t valueIndex, uint8_t valueType, char *valueStr)
{
	PGM_P p;

	if(valueType == 0)
	{
		memcpy_P(&p, &amp[valueIndex], sizeof(PGM_P));
		strncpy_P(valueStr, p, 4);
	}

	else if(valueType == 1)
	{
		memcpy_P(&p, &freq[valueIndex], sizeof(PGM_P));
		strncpy_P(valueStr, p, 4);
	}

	else if(valueType == 2)
	{
		memcpy_P(&p, &time[valueIndex], sizeof(PGM_P));
		strncpy_P(valueStr, p, 4);
	}
}

int8_t getEncoderDirection(void)
{
	return encoderDirection;
}

uint8_t readPowerButton(void)
{
	static powerOffCount;
	if(((PORTG.input & BIT(MCU_POWER_CONTROL)) == 0) )
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
	return powerOffSignal;
}

void updateFootswitchLeds(void)
{
	static uint8_t fswDebounceCount;

	if(fswDebounceCount == 0)
	{
		if(PORTD.input & BIT(FSW1))
		{
			if(fsw1.physicalStatus == 0)  // fsw1 just pressed
			{
				fsw1.value ^= 0x01;
				fsw1.physicalStatus = 1;
				fsw1.change = 1;

			}
		}
		else
		{
			if(fsw1.physicalStatus == 1)  // fsw1 just released
			{
				fsw1.physicalStatus = 0;
			}
		}

		if(PORTD.input & BIT(FSW2))
		{
			if(fsw2.physicalStatus == 0)  // fsw2 just pressed
			{
				fsw2.value ^= 0x01;
				fsw2.physicalStatus = 1;
				fsw2.change = 1;

			}
		}
		else
		{
			if(fsw2.physicalStatus == 1)  // fsw2 just released
			{
				fsw2.physicalStatus = 0;
			}
		}

		if(fsw1.value) PORTD.output &= ~BIT(FSW1_LED);
		else PORTD.output |= BIT(FSW1_LED);

		if(fsw2.value) PORTD.output &= ~BIT(FSW2_LED);
		else PORTD.output |= BIT(FSW2_LED);

		fswDebounceCount = 5;
	}
	else fswDebounceCount--;

}

void disablePowerOff(void)
{
	powerOffEnable = 0;
}
void enablePowerOff(void)
{
	powerOffEnable = 1;
}

void powerOff(void)
{
	PORTG.output &= ~BIT(nPOWER_OFF);
}


/***********************************************************
 *		Writing strings to LCD Display
 ***********************************************************/
void DisplayStatus(uint8_t *line3)
{
	uint8_t n=0;
	uint8_t i=0;

	n=0;
	writeLcdInstr(SET_DDRAM_ADDRESS + LCD_LINE3_ADDR);
	while((*(line3 + n) != '\0') && (n < 20))
	{
		LCD_data(*(line3 + n));
		n++;
	}

}

/***********************************************************
 *		Writing strings to LCD Display
 ***********************************************************/
void writeDisplay(uint8_t *line1, uint8_t *line2, uint8_t *line3, uint8_t *line4)
{
	uint8_t n=0;
	uint8_t i=0;


	if(line1 != 0)
	{
		writeLcdInstr(0x01);
		writeLcdInstr(0b00000110);
		while((*(line1 + n) != '\0') && (n < 20))
		{
			writeLcdData(*(line1 + n));
			n++;
		}
	}

	if(line2 != 0)
	{
		n=0;
		writeLcdInstr(SET_DDRAM_ADDRESS + LCD_LINE2_ADDR);
		while((*(line2 + n) != '\0') && (n < 20))
		{
			writeLcdData(*(line2 + n));
			n++;
		}
	}

	if(line3 != 0)
	{
		n=0;
		writeLcdInstr(SET_DDRAM_ADDRESS + LCD_LINE3_ADDR);
		while (*(line3 + n) != '\0')
		{
			writeLcdData(*(line3 + n));
			n++;
		}
	}

	if(line4 != 0)
	{
		n=0;
		writeLcdInstr(SET_DDRAM_ADDRESS + LCD_LINE4_ADDR);
		while((*(line4 + n) != '\0') && (n < 20))
		{
			writeLcdData(*(line4 + n));
			n++;
		}
	}


}

void writeDisplayLine(uint8_t lineNumber, uint8_t *lineStr)
{
	uint8_t n=0;
	uint8_t i=0;

	char tempStr[21];
	memset(tempStr,0,sizeof(char)*21);
	strncpy(tempStr,lineStr,20);

	switch(lineNumber)
	{
	case 1:
		writeLcdInstr(SET_DDRAM_ADDRESS + LCD_LINE1_ADDR);
		break;
	case 2:
		writeLcdInstr(SET_DDRAM_ADDRESS + LCD_LINE2_ADDR);
		break;
	case 3:
		writeLcdInstr(SET_DDRAM_ADDRESS + LCD_LINE3_ADDR);
		break;
	case 4:
		writeLcdInstr(SET_DDRAM_ADDRESS + LCD_LINE4_ADDR);
		break;
	default:;
	}

	while((*(tempStr + n) != '\0') && (n < 20))
	{
		writeLcdData(*(tempStr + n));
		n++;
	}



}

uint8_t getValueChange(void)
{
	return valueChange;
}

void setValueChange(uint8_t value)
{
	valueChange = value;
}
/************************************ EVENTS ************************************************/


// Rotary Encoder ISR
void __vector_1(void) __attribute__ ((interrupt));
void __vector_1(void)
{
	if((PORTD.input & BIT(ROT_A)) == 0)// Falling edge detected
	{
		{
			level[edgeIndex][1] = 0;
			level[edgeIndex][0] = PORTD.input & BIT(ROT_B);
			edgeIndex = 1;
		}
	}
	else // Rising Edge detected
	{

		level[edgeIndex][1] = 1;
		level[edgeIndex][0] = PORTD.input & BIT(ROT_B);
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

		level[0][0] = 0; // reset array
		level[0][1] = 0; // reset array
		level[1][0] = 0; // reset array
		level[1][1] = 0; // reset array
	}


}

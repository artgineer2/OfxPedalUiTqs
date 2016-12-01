/*
 * GlobalVars.h
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#ifndef GLOBALVARS_H_
#define GLOBALVARS_H_

#include <ctype.h>
#include <stdint.h>
//typedef uint8_t uint8_t;

#define TX_BUFFER_SIZE  50
#define RX_BUFFER_SIZE  50
#define JSON_BUFFER_SIZE 800

/********************** SERIAL TX ************************
char tx_buffer[TX_BUFFER_SIZE+10];
unsigned int tx_count;

/********************** SERIAL RX ************************
char rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_type;
unsigned long rx_total_high;
unsigned long rx_total_mid;
unsigned long rx_total_low;
unsigned long rx_total;
unsigned long rx_count;
unsigned int rx_wait_timer;
uint8_t rx_in_progress;

unsigned long rx_start_addr_high;
unsigned long rx_start_addr_mid;
unsigned long rx_start_addr_low;
unsigned long rx_start_addr;*/

//#ifndef rx_done
//uint8_t rx_done = 0;
//#endif


uint8_t main_state;
uint8_t main_model;
uint8_t main_section;
/*unsigned int main_fxcore_address;
unsigned long main_flash_address;
unsigned long prev_flash_address;*/
uint8_t main_fsw;
uint8_t main_stage;
uint8_t main_effect;
uint8_t main_param;
uint8_t num_effects, num_parameters;
uint8_t state_change,model_change,effect_change,param_change,value_change,LCD_change;
uint8_t menuLevel;
uint8_t valueChange;
uint8_t comboChange;
uint8_t newCombo;
/*struct {
	char file[20];
	char function[20];
	uint8_t position;
	char desc[20];
} globalError;*/
/****************** UI LCD and Buttons ***********************/
char debugString[20];

/*char buffer1[16];
char buffer2[16];
char IndBuf[16];*/
char lcdInitBuffer[25];
char lcdBuffer[4][20];
int lcdBufferHash[4];
uint8_t uiTempButtons;

uint8_t buttonPushed;
uint8_t buttonReleased;
uint8_t buttonDebounceCount;
uint8_t rotaryValue;
uint8_t powerOffSignal;
uint8_t powerOffEnable;
char valueString[5];
/*char fsw1Abbr[5];
char fsw2Abbr[5];*/

struct fsw{
	uint8_t physicalStatus; // is footswitch physically pressed down by user's foot?
	uint8_t value;	// variable to be toggled
	uint8_t change;
};

struct fsw fsw1;
struct fsw fsw2;

uint8_t timerTestCount;
uint8_t count_up;
uint8_t count_down;
uint8_t count_dir;

uint8_t loadMenu;
uint8_t hostUiActive;
uint8_t tempHostUiActive;
uint8_t restoreFromHostUiMode;
/*struct softkey{
	char abbr[6];
	uint8_t arrayNodeIndex;
};

struct softkey softkeyGroupArray[12];
uint8_t softkeyGroupArrayIndex[2];*/
char softkeyString[100];
uint8_t softkeyStringFrameIndex[2];
uint8_t softkeyStringFrameCount;
uint8_t uiChange;
char ofxMainStatusString[20];
/**********************************************************/

uint8_t rx_model;
uint8_t rx_section;
unsigned int rx_section_address;
uint8_t rx_stage;
unsigned int rx_stage_address;

uint8_t abbr_buffer[4];
//uint8_t name_buffer[15];

/******************** Shared Memory **************************/
/*struct smStatus {
	uint8_t mcuDataSent;

	uint8_t cmDataQueried;
	uint8_t cmDataRecieved;

};*/
uint8_t computeModuleDataRetrievalState;// 0: Idle, 1: Start, 2: In-process, 3: Stop
											// NOT TO BE CONFUSED WITH CM PROCESSING STATE
uint8_t computeModuleDataSendState;// 0: Idle, 1: Start, 2: In-process, 3: Stop
uint8_t computeModuleDataProcessingState; // 0: Idle, 1: Waiting, 2: Done
//uint8_t computeModuleStatusArray[2];

char *sharedMemoryRxBuffer;//[500];
uint16_t sharedMemoryRxBufferIndex;
uint16_t sharedMemoryRxBufferCount;
uint16_t sharedMemoryRxStartAddress;

char *sharedMemoryTxBuffer;
uint16_t sharedMemoryTxBufferIndex;
uint16_t sharedMemoryTxBufferCount;
uint16_t sharedMemoryTxStartAddress;

uint16_t spiXferSizeLimit;
uint16_t spiXferCount;
uint8_t spiCommand[4];
uint8_t spiCommandIndex;
char jsonBuffer[JSON_BUFFER_SIZE];
char sendBuffer[50];
char getCombosBuffer[170];
uint8_t requestStatus; //0: Idle, 1: Sending Request, 2: Waiting for response, 3: Getting Response, 4:Response Received
uint8_t newRequest;
uint8_t responseError;
uint8_t getResponse;

struct _node{
	uint8_t nodeType; // 0=comb0, 1=effect, 2=parameter
	char abbr[4];
	char name[15];
	uint8_t value;
	//char value[4];
	uint8_t valueType;
	uint8_t paramIndex;
	uint8_t up;		// array indexes for up,left,right, and down nodes.
	uint8_t left;
	uint8_t right;
	uint8_t down;
	/*uint8_t numChildNodes;
	uint8_t currentChildNode;*/
};

uint8_t paramIndex2nodeArrayIndex[30];
char currentDataString[100];
uint8_t currentDataUpdateTimer;
uint8_t nodeCount;
//struct _node *Model;
struct _node nodeArray[22]; // was 17 nodes
struct _node testNode;
uint8_t nodeArrayIndex;
uint8_t newNodeArrayIndex;
uint8_t currentNodeArrayIndex;
uint8_t currentComboNodeArrayIndex;
uint8_t currentEffectNodeArrayIndex;
uint8_t currentParamNodeArrayIndex;
uint16_t nodeAddress;

char comboTitle[10][11];
uint8_t comboIndex;
uint8_t currentComboIndex;
uint8_t comboCount;
//char debugSpiString[500];

char effectElementString[100];
uint8_t effectElementIndex;

char paramElementString[100];
uint8_t paramElementIndex;



#endif /* GLOBALVARS_H_ */

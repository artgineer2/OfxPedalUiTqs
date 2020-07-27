/*
 * main.c
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#undef __AVR_ATmega645__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "macros.h"
#include "ChipInit.h"
#include "ComputeModuleInterface.h"
#include "TaskQueue.h"
#include "Tasks.h"
#include "ComboNodes.h"
#include "utilityFuncts.h"
#include "UserInterfaceDrivers.h"
#include "UserInterfaceOps.h"

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

#define SHARED_MEMORY_SECTION_SIZE 4096
#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1
#define TASK_QUEUE_SIZE 20
#define CM0_PRESENT 1


void main(void)
{

	 uint8_t taskNumber = 0;

	 config_chip();

	 while(readPowerButton() == 1); // wait until power button is released

	 /************ Initialize driver modules *******************/
	 initUserInterfaceDrivers();
	 initComputeModuleInterface();

	 /**************** Initialize other modules **********************/
	 initUserInterfaceOps();
	 initComboNodes();
	 initTaskQueue();

	 /********** Reset CM **********/
	 resetComputeModule();

	 writeDisplay("","      OFX      ","Starting up....","");

#if CM0_PRESENT
	 while(isComputeModuleRunning() == 0);
#endif
	 writeDisplay("","      OFX      ","Starting up1...","");

#if CM0_PRESENT
	 while(isOfxMainRunning() == 0);  // wait for OfxMain on CM
#endif
	 writeDisplay("","      OFX      ","Starting up2...","");

	 SEI();
#if !CM0_PRESENT
	 while(powerOffSignal == 0)
	 {
		 SPI_Tx(0x55);
	 }
#else
	 writeDisplay("","      OFX      ","Starting up3...","");

	 insertTask(GetComboList);
	 insertTask(LoadCombo);

	 while(readPowerButton() == 0)
	 {
		 taskNumber = getTask();
		 if(taskNumber > 0)
		 {
			 switch(taskNumber)
			 {
				 case 1:
					 getComboList();
					 break;
				 case 2:
					 loadCombo();
					 break;
				 case 3:
					 saveCombo();
					 break;
				 case 4:
					 changeComboEffectParameter();
					 break;
				 case 5:
					 requestStatusUpdateFromCM();
					 break;
				 default:;
			 }
		 }
	 }
#endif
	 powerOffCM();
	 writeDisplay("","powering off     ","","");

	 while(isComputeModuleRunning() == 1);

	 writeDisplay("","powered off      ","release button   ","");

	 while(readPowerButton() == 0); // wait until power button is released
	 delay(1000);

	 powerOff();
}



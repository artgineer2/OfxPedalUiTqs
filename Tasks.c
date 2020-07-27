/*
 * Tasks.c
 *
 *  Created on: Jul 19, 2020
 *      Author: mike
 */

#include <stdint.h>
#include <string.h>
#include "Tasks.h"
#include "Typedefs.h"
#include "ComboNodes.h"
#include "UserInterfaceOps.h"
#include "UserInterfaceDrivers.h"
#include "ComputeModuleInterface.h"


/****************************** TASKS FUNCTIONING OVERVIEW **********************************************
 *
 * 	When a task is run, it starts by sending a request string to the main application running on
 * 	the Compute Module via the rqstBuffer argument in "sendStringToComputeModule".
 * 	The application processes the request and then sends the response back to the memory section
 * 	pointed to in the third argument.
 *
 * 	Further processing may be done .after the response has been send back
 *
 ********************************************************************************************************/

#define SHARED_MEMORY_SECTION_SIZE 4096
#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1

static char rqstBuffer[50];
static char responseBuffer[100];


/*	@brief:	Get list of combos from the main application running
 * 			on the Compute Module.
 *
 *	@returns: none
 */


void getComboList(void)
{
	memset(rqstBuffer, 0, sizeof(char)*50);
	memset(responseBuffer, 0, sizeof(char)*100);

	strcpy(rqstBuffer, "listCombos");
	rqstBuffer[strlen(rqstBuffer)] = 255;
	sendStringToComputeModule(rqstBuffer, 50, getComboListBufferPtr(), 150);
}




/*	@brief:	Get JSON-formatted combo UI data from the main application running
 * 			on the Compute Module and load it into the UI
 *
 *	@returns: none
 */


void loadCombo(void)
{
	uint8_t comboIndex = getCurrentComboIndex();
	memset(rqstBuffer, 0, sizeof(char)*50);
	strncpy(rqstBuffer, "getCombo:", 9);
	strncat(rqstBuffer, getComboName(comboIndex), 10);
	rqstBuffer[49] = 255;

	sendStringToComputeModule(rqstBuffer, 50, getJsonBufferPtr(), 800);
	setMenuLevel(0);
	jsonBuffer2UiMenu();
	updateDisplay(1);
}


/*	brief:	Save changes made to combo parameters.
 *
 *
 * 	returns: none
 */


void saveCombo(void)
{
	memset(rqstBuffer, 0, sizeof(char)*50);
	strncpy(rqstBuffer, "saveCombo", 9);
	rqstBuffer[49] = 255;

	sendStringToComputeModule(rqstBuffer, 50, getComboListBufferPtr(), 150);
	updateDisplay(1);
}


/*	brief:	Change effect parameter in combo currently setup in the
 * 			 main application running on Compute Module
 *
 * 	returns: none
 */

void changeComboEffectParameter(void)
{
	Node_t currentNode;
	if(getPedalUiMode() == 1) // normal pedal UI running mode
	{
		currentNode = getCurrentNode();
		//**************** Updating values from Pedal User Interface ***************
		if(getValueChange() == 1 && getMenuLevel() == 3)
		{
			sprintf(rqstBuffer, "changeValue:%d=%d", currentNode.paramIndex,
					currentNode.value);
			setValueChange(0);

			sendStringToComputeModule(rqstBuffer, 50, NULL,0);
			updateDisplay(1);
		}
	}
}

/*	@brief: Get status update from main application on CM.
 *
 * 	@returns: none
 */
void requestStatusUpdateFromCM(void)
{

	memset(rqstBuffer, 0, sizeof(char)*50);
	rqstBuffer[49]=255;

	strcpy(rqstBuffer,"getCurrentStatus:");
	if(getPedalUiMode() == 1)
	{
		strcat(rqstBuffer,"pedal");
	}
	else
	{
		strcat(rqstBuffer,"host");
	}

	resetCurrentDataUpdateTimer();
	sendStringToComputeModule(rqstBuffer, 50, responseBuffer, 100);
	updateStatus(responseBuffer);
}

/*	@brief:	Main application will shutdown and signal the OS
 * 			to power off the CM.
 *
 *	@returns: none
 */
void powerOffCM(void)
{
	memset(rqstBuffer, 0, sizeof(char)*50);
	rqstBuffer[49]=255;
	strcpy(rqstBuffer,"powerOff");

	sendStringToComputeModule(rqstBuffer, 50, responseBuffer,100);
}

/*
 * ComboNodes.c
 *
 *  Created on: Jul 19, 2020
 *      Author: mike
 */

#include <stdint.h>

#include "Typedefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "UserInterfaceDrivers.h"
#include "macros.h"


#define JSON_BUFFER_SIZE 800

static Node_t nodeArray[22]; // was 17 nodes
static uint8_t nodeArrayIndex;
static uint8_t newNodeArrayIndex;
static uint8_t currentNodeArrayIndex;
static uint8_t currentComboNodeArrayIndex;
static uint8_t currentEffectNodeArrayIndex;
static uint8_t currentParamNodeArrayIndex;
static char effectElementString[150];
static uint8_t effectElementIndex;
static char paramElementString[50];
static uint8_t paramElementIndex;
static char jsonBuffer[JSON_BUFFER_SIZE];
static char jsonBufferError[20];



/*	@brief: Sets the combo name and node type in the root node of the combo UI array
 *
 * 	@return: Combo node index (0)
 *
 */
static uint8_t setComboNode(char *string)
{
	char *token = 0;
	char *name = 0;

	token = strtok(string,":");
	name = strtok(NULL,",");
	strncpy(nodeArray[nodeArrayIndex].name,(const char *)name,10);
	nodeArray[nodeArrayIndex].nodeType = 0;
	nodeArrayIndex++;
	return 0;
}


/*	@brief: Sets the effect name, abbreviation, and node type in the effect nodes of the combo UI array
 *
 *  @param: string: String containing the effect UI information.
 *
 * 	@return: array index of the node
 *
 */

static uint8_t setEffectNode(char *string)
{
	char *token = 0;
	char *abbr = 0;
	char *name = 0;

	token = strtok(string,":");
	abbr = strtok(NULL,",");
	token = strtok(NULL,":");
	name = strtok(NULL,",");
	strncpy(nodeArray[nodeArrayIndex].abbr,(const char *)abbr,4);
	strncpy(nodeArray[nodeArrayIndex].name,(const char *)name,15);
	nodeArray[nodeArrayIndex].nodeType = 1;
	nodeArrayIndex++;
	return nodeArrayIndex-1;
}

/*	@brief: Sets the parameter name, abbreviation and node type in the effect nodes of the combo UI array
 *
 * 	@param: string: String containing the parameter UI information.
 *
 * 	@return: array index of the node
 */

static uint8_t setParamNode(char *string)
{
	char *token = 0;
	char *abbr = 0;
	char *name = 0;
	char *value = 0;
	char *type = 0;
	char *index = 0;

	string++; // increment past open brace
	abbr = strtok(string,",");
	name = strtok(NULL,",");
	value = strtok(NULL,",");
	type = strtok(NULL,",");
	index = strtok(NULL,"}");
	strncpy(nodeArray[nodeArrayIndex].abbr,(const char *)abbr,4);
	strncpy(nodeArray[nodeArrayIndex].name,(const char *)name,15);
	nodeArray[nodeArrayIndex].value = atoi(value);
	nodeArray[nodeArrayIndex].valueType = *type-48;
	nodeArray[nodeArrayIndex].paramIndex = atoi(index);
	nodeArray[nodeArrayIndex].nodeType = 2;
	nodeArrayIndex++;
	return nodeArrayIndex-1;
}


/*	@brief:	Converts the contents of jsonBuffer array into the ComboNode array.
 *
 *	@returns: JSON parse error level (1: Effect, 2: Parameter)
 */
uint8_t jsonBuffer2UiMenu(void)
{
	uint8_t level = 0;

	uint8_t i,j,k;

	char *openBracketPtr = 0;
	char *closeBracketPtr = 0;
	char *openBracePtr = 0;
	char *closeBracePtr = 0;
	char commaBracket = 0;

	char *bufferPtr = 0;
	char *jsonBufferStart = 0;
	char *bufferEnd = 0;
	char tempBuffer[100];
	uint8_t jsonParseError = 0;
	nodeArrayIndex = 0;
	newNodeArrayIndex = 0;
	currentNodeArrayIndex = 0;
	currentComboNodeArrayIndex = 0;
	currentEffectNodeArrayIndex = 0;
	currentParamNodeArrayIndex = 0;
	effectElementIndex = 0;
	paramElementIndex = 0;

	bufferEnd = (char *)(jsonBuffer + strlen(jsonBuffer));

	uint8_t done = 0;
	bufferPtr = jsonBuffer;
	jsonBufferStart = jsonBuffer;
	for(i = 0; i < 22; i++)
	{
		memset(nodeArray[i].name, 0, sizeof(char)*15);
		memset(nodeArray[i].abbr, 0, sizeof(char)*4);
		nodeArray[i].nodeType = 0;
		nodeArray[i].value = 0;
		nodeArray[i].paramIndex = 0;
		nodeArray[i].up = 255;
		nodeArray[i].left = 255;
		nodeArray[i].right = 255;
		nodeArray[i].down = 255;
	}
	while(done == 0 && jsonParseError == 0)
	{
		/************ information processing state machine ***********/

		switch(level)
		{
		case 0:
			openBracketPtr = strchr(bufferPtr,'[');
			closeBracketPtr = strchr(bufferPtr,']');
			openBracePtr = strchr(bufferPtr,'{');
			closeBracePtr = strchr(bufferPtr,'}');
			bufferPtr = openBracePtr;
			memset(tempBuffer, 0, sizeof(char)*100);
			strncpy(tempBuffer,(const char *)bufferPtr,openBracketPtr-bufferPtr);
			currentComboNodeArrayIndex = setComboNode(tempBuffer);
			newNodeArrayIndex = currentComboNodeArrayIndex;
			nodeArray[currentComboNodeArrayIndex].down = 255;
			nodeArray[currentComboNodeArrayIndex].left = 255;
			nodeArray[currentComboNodeArrayIndex].right = 255;
			nodeArray[currentComboNodeArrayIndex].up = 255;
			openBracketPtr = strchr(bufferPtr,'[');
			bufferPtr = openBracketPtr+1;
			level = 1;
			break;
		case 1:

			openBracketPtr = strchr(bufferPtr,'[');
			closeBracketPtr = strchr(bufferPtr,']');
			openBracePtr = strchr(bufferPtr,'{');
			closeBracePtr = strchr(bufferPtr,'}');
			char *tempPtr;

			if(strchr(bufferPtr,'[')==0)
			{
				done = 1;
				break;
			}
			bufferPtr = openBracePtr;
			tempPtr = openBracketPtr;
			memset(effectElementString, 0, sizeof(char)*150);
			if(openBracketPtr <= bufferPtr)
			{
				sprintf(jsonBufferError,"%d:%d:%d,%d", effectElementIndex, paramElementIndex, tempPtr, bufferPtr);
				writeDisplayLine(3,jsonBufferError);
				sprintf(jsonBufferError,"%d,%d,%d,%d,%d", jsonBufferStart, openBracketPtr, closeBracketPtr, openBracePtr, closeBracePtr);
				writeDisplayLine(4,jsonBufferError);
				jsonParseError = 1;
				break;
			}
			else
			{
				strncpy(effectElementString,(const char *)bufferPtr,openBracketPtr-bufferPtr);
			}



			// create node
			newNodeArrayIndex = setEffectNode(effectElementString);
			// connect nodes
			if(effectElementIndex == 0)
			{
				currentEffectNodeArrayIndex = newNodeArrayIndex;
				nodeArray[currentComboNodeArrayIndex].down = currentEffectNodeArrayIndex;
			}
			else
			{
				nodeArray[currentEffectNodeArrayIndex].right = newNodeArrayIndex;
				nodeArray[newNodeArrayIndex].left = currentEffectNodeArrayIndex;
				currentEffectNodeArrayIndex = nodeArray[currentEffectNodeArrayIndex].right;
			}
			nodeArray[currentEffectNodeArrayIndex].up = currentComboNodeArrayIndex;

			bufferPtr = strchr(bufferPtr,'[')+1;
			commaBracket = *(strchr(bufferPtr,'}')+1);

			level = 2;

			paramElementIndex=0;


			break;
		case 2:
			openBracketPtr = strchr(bufferPtr,'[');
			closeBracketPtr = strchr(bufferPtr,']');
			openBracePtr = strchr(bufferPtr,'{');
			closeBracePtr = strchr(bufferPtr,'}');
			commaBracket = *(strchr(bufferPtr,'}')+1);
			tempPtr = closeBracePtr+1;
			memset(paramElementString, 0, sizeof(char)*50);

			if(tempPtr <= bufferPtr)
			{
				sprintf(jsonBufferError,"%d:%d:%d,%d", effectElementIndex, paramElementIndex, tempPtr, strlen(bufferPtr));
				writeDisplayLine(3,jsonBufferError);
				strncpy(jsonBufferError,bufferPtr,19);
				writeDisplayLine(4,jsonBufferError);
				jsonParseError = 2;
				break;
			}
			else
			{
				strncpy(paramElementString,(const char *)bufferPtr,tempPtr-bufferPtr);
			}

			strncpy(paramElementString,(const char *)bufferPtr,tempPtr-bufferPtr);


			newNodeArrayIndex = setParamNode(paramElementString);
			//connect nodes
			if(paramElementIndex == 0)
			{
				currentParamNodeArrayIndex = newNodeArrayIndex;
				nodeArray[currentEffectNodeArrayIndex].down = currentParamNodeArrayIndex;
			}
			else
			{
				nodeArray[currentParamNodeArrayIndex].right = newNodeArrayIndex;
				nodeArray[newNodeArrayIndex].left = currentParamNodeArrayIndex;
				currentParamNodeArrayIndex = nodeArray[currentParamNodeArrayIndex].right;
			}
			nodeArray[currentParamNodeArrayIndex].up = currentEffectNodeArrayIndex;

			if(commaBracket == ',')
			{
				bufferPtr = strchr(bufferPtr,'}'); // move to element end
				bufferPtr = strchr(bufferPtr,'{'); // move to beginning of next element
				level = 2;
			}
			else
			{
				bufferPtr = strchr(bufferPtr,'}'); // move to element end
				bufferPtr = strchr(bufferPtr,'}'); //move to end of effect
				level = 1;
				effectElementIndex++;
			}

			paramElementIndex++;
			break;
		default:;

		}

	}
	currentNodeArrayIndex = 0;
	currentComboNodeArrayIndex = 0;
	currentEffectNodeArrayIndex = 0;
	currentParamNodeArrayIndex = 0;

	return jsonParseError;
}

/*********************************** PUBLIC FUNCTIONS ********************************************/


/*	@brief: Clears all the variables and data structures.
 *
 *  @return: none
 *
 */
void initComboNodes(void)
{
	memset(nodeArray, 0, sizeof( Node_t)*22);
	nodeArrayIndex = 0;
	newNodeArrayIndex = 0;
	currentNodeArrayIndex = 0;
	currentComboNodeArrayIndex = 0;
	currentEffectNodeArrayIndex = 0;
	currentParamNodeArrayIndex = 0;
	memset(effectElementString, 0, sizeof(char)*150);
	effectElementIndex = 0;
	memset(paramElementString, 0, sizeof(char)*50);
	paramElementIndex = 0;
	memset(jsonBuffer, 0, sizeof(char)*JSON_BUFFER_SIZE);

}

/*	@brief: Checks to see if there is a node to go down to and, if so, goes to it.
 * 			This actually moves towards the root (menuLevel 0), so decrementing the
 * 			level means go up one level
 *
 *	@returns: new menu level
 */

uint8_t decrementMenuLevel(uint8_t initialMenuLevel)
{
	uint8_t newMenuLevel = initialMenuLevel;

	if(nodeArray[currentNodeArrayIndex].up != 255)
	{
		currentNodeArrayIndex = nodeArray[currentNodeArrayIndex].up;
		newMenuLevel--;

		if(newMenuLevel == 2) // effect
		{
			currentEffectNodeArrayIndex = currentNodeArrayIndex;
			currentParamNodeArrayIndex = 0;
		}
		else if(newMenuLevel == 3) // param
		{
			currentParamNodeArrayIndex = currentNodeArrayIndex;
		}
	}
	else newMenuLevel = 255;

	return newMenuLevel;
}

/*	@brief: Checks to see if there is a node to go up to and, if so, goes to it.
 * 			This actually moves away from the root (menuLevel 0), so incrementing the
 * 			level means go down one level
 *
 *	@returns: new menu level
 */

uint8_t incrementMenuLevel(uint8_t index, uint8_t initialMenuLevel)
{
	uint8_t i;
	uint8_t newMenuLevel = initialMenuLevel;

	if(nodeArray[currentNodeArrayIndex].down != 255)
	{
		currentNodeArrayIndex = nodeArray[currentNodeArrayIndex].down;
		for(i = 0; i<index; i++)
		{
			if(nodeArray[currentNodeArrayIndex].right != 255)
				currentNodeArrayIndex = nodeArray[currentNodeArrayIndex].right;
		}
		newMenuLevel++;

		if(newMenuLevel == 2) // effect
		{
			currentEffectNodeArrayIndex = currentNodeArrayIndex;
		}
		else if(newMenuLevel == 3) // param
		{
			currentParamNodeArrayIndex = currentNodeArrayIndex;
		}

		//softkeyStringFrameIndex[newMenuLevel-1] = 0;
	}
	else newMenuLevel = 255;

	return newMenuLevel;
}

/*	@brief:	Gets the node at the array index
 *
 * 	@param:	nodeIndex: index in ComboNode
 *
 * 	@returns: Node
 */


Node_t getNode(uint8_t nodeIndex)
{
	return nodeArray[nodeIndex];
}

/*	@brief:	Gets the node at the array index
 *
 * 	@param:	nodeIndex: index in ComboNode
 *
 * 	@returns: Node
 */

Node_t getComboNode(void)
{
	return nodeArray[0];
}


Node_t getCurrentNode(void)
{
	return nodeArray[currentNodeArrayIndex];
}


Node_t getParentNode(void)
{
	return nodeArray[nodeArray[currentNodeArrayIndex].up];
}

char *getJsonBufferPtr(void)
{
	memset(jsonBuffer, 0, sizeof(char)*JSON_BUFFER_SIZE);
	jsonBuffer[JSON_BUFFER_SIZE-1] = 255;
	return jsonBuffer;
}


uint8_t getCurrentComboIndex(void)
{
	return currentComboNodeArrayIndex;
}

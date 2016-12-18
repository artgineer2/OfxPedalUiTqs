/*

 * JsonFuncts.c
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */


#include "JsonFuncts.h"
#include "GlobalVars.h"
#include "utilityFuncts.h"

#define NULL 0
struct _tag{
	char key[10];
	char value[15];
};

extern void clearBuffer(char *buffer, int length);
extern const char *testJson;
extern uint8_t read_switch();
void Display(uint8_t *line1, uint8_t *line2, uint8_t *line3, uint8_t *line4);



int8_t breakpoint = 0;
static void breakButtonJson(char *buffer1, char *buffer2, char *buffer3, char *buffer4)
{
/*	CLI();
	clearBuffer(lcdBuffer[0],16);
	clearBuffer(lcdBuffer[1],16);
	clearBuffer(lcdBuffer[2],16);
	clearBuffer(lcdBuffer[3],16);

	strncpy(lcdBuffer[0], buffer1,16);
	strncpy(lcdBuffer[1], buffer2,16);
	strncpy(lcdBuffer[2], buffer3,16);
	strncpy(lcdBuffer[3], buffer4,16);
	Display(lcdBuffer[0], lcdBuffer[1], lcdBuffer[2], lcdBuffer[3]);
	breakpoint++;
	while(PINA == 0);
	Display(lcdBuffer[0], "pushed", lcdBuffer[2], lcdBuffer[3]);
	while(PINA != 0);
	Display(lcdBuffer[0], "released", lcdBuffer[2], lcdBuffer[3]);
	SEI();*/
}


uint16_t findChar(const char *string, char target)
{
	uint16_t i = 0;

	while(string[i++] != target);

	return i;
}

char *getKeyVal(char *string)
{
	char *keyValString = 0;

	keyValString = strtok(string,"\"");
	strtok(NULL,"\"");

	return keyValString;
}

uint8_t getComboNode(char *string)
{
	char *token = 0;
	char *abbr = 0;
	char *name = 0;

	//struct _node node = (struct _node*)calloc(1, sizeof(struct _node));
	//token = strtok(string,"{");
	token = strtok(string,":");
	name = strtok(NULL,",");
	//strncpy(name, token,12);
	strncpy(nodeArray[nodeArrayIndex].name,(const char *)name/*(getKeyVal(name))*/,10);
	//nodeArray[nodeArrayIndex].name[10] = 0;
	nodeArray[nodeArrayIndex].nodeType = 0;
	//breakButtonUi(node, 1);
	//return node;
	nodeArrayIndex++;
	return 0;
}

uint8_t getEffectNode(char *string)
{
	char *token = 0;
	char *abbr = 0;
	char *name = 0;

	//struct _node *node = (struct _node*)calloc(1, sizeof(struct _node));
	//token = strtok(string,"{");
	token = strtok(string,":");
	abbr = strtok(NULL,",");
	token = strtok(NULL,":");
	name = strtok(NULL,",");
	//strncpy(abbr, token,6);
	strncpy(nodeArray[nodeArrayIndex].abbr,(const char *)abbr/*(getKeyVal(abbr))*/,4);
	//nodeArray[nodeArrayIndex].abbr[4] = 0;
	//strncpy(name, token,12);
	strncpy(nodeArray[nodeArrayIndex].name,(const char *)name/*(getKeyVal(name))*/,15);
	//nodeArray[nodeArrayIndex].name[10] = 0;
	/*node->down = 0;
	node->left = 0;
	node->right = 0;
	node->up = 0;*/
	nodeArray[nodeArrayIndex].nodeType = 1;
	//breakButtonUi(node, 2);
	nodeArrayIndex++;
	return nodeArrayIndex-1;
}

uint8_t getParamNode(char *string)
{
	char *token = 0;
	char *abbr = 0;
	char *name = 0;
	char *value = 0;
	char *type = 0;
	char *index = 0;

	//struct _node *node = (struct _node*)calloc(1, sizeof(struct _node));

	//token = strtok(string,"{");
	//token = strtok(string,":");
	string++; // increment past open brace
	abbr = strtok(string,",");
	//token = strtok(NULL,":");
	name = strtok(NULL,",");
	//token = strtok(NULL,":");
	value = strtok(NULL,",");
	//token = strtok(NULL,":");
	type = strtok(NULL,",");
	//token = strtok(NULL,":");
	index = strtok(NULL,"}");
	strncpy(nodeArray[nodeArrayIndex].abbr,(const char *)abbr/*(getKeyVal(abbr))*/,4);
	strncpy(nodeArray[nodeArrayIndex].name,(const char *)name/*(getKeyVal(name))*/,15);
	nodeArray[nodeArrayIndex].value = atoi(value);
	nodeArray[nodeArrayIndex].valueType = *type-48;
	nodeArray[nodeArrayIndex].paramIndex = atoi(index);
	//node->valueType = type);
	nodeArray[nodeArrayIndex].nodeType = 2;
	//breakButtonUi(node, 3);
	nodeArrayIndex++;
	return nodeArrayIndex-1;
}


/*struct _tag getTag(char *tagString)
{
	struct _tag tempTag;

	char *tempStrKey = 0;
	char *tempStrValue = 0;

	tagString = strtok(tagString,"{");
	tempStrKey = strtok(tagString,":");
	strncpy(tempTag.key, (const char*)(tempStrKey),10);
	tempStrValue = strtok(NULL,",");
	strncpy(tempTag.value , (const char*)(tempStrValue),15);

	strncpy(tempTag.key, strtok(tempTag.key,"\""),10);
	//tempStrValue = strtok(NULL,"\"");
	strncpy(tempTag.value, (const char*)strtok(tempTag.value,"\""),15);
	//strtok(NULL,"\"");


	return tempTag;
}*/

uint8_t jsonBuffer2UiMenu(void)
{
	uint8_t level = 0;

	uint8_t i,j,k;

	char *openBracketPtr = 0;
	char *closeBracketPtr = 0;
	char *openBracePtr = 0;
	char *closeBracePtr = 0;
	char *commaPtr = 0;
	char commaBracket;

	char *tagBuffer = NULL;

	/*struct _node *previousNode;
	struct _node *newNode;
	struct _node *nextNode;*/
	struct _tag tag;

	struct _tag tagArray[20];

	char *parsedString[5];

	//char buffer[1000];  //use jsonBuffer[1000]
	char *bufferPtr = 0;
	char *jsonBufferStart = 0;
	char *bufferEnd = 0;
	char tempBuffer[100];
	char *levelInfoPtr = 0;
	char *nextLevelInfoPtr = 0;
	uint8_t jsonParseError = 0;
	nodeArrayIndex = 0;
	newNodeArrayIndex = 0;
	currentNodeArrayIndex = 0;
	currentComboNodeArrayIndex = 0;
	currentEffectNodeArrayIndex = 0;
	currentParamNodeArrayIndex = 0;
	effectElementIndex = 0;
	paramElementIndex = 0;


	//strcpy(jsonBuffer,jsonString);

	bufferEnd = (char *)(jsonBuffer + strlen(jsonBuffer));

	uint8_t done = 0;
	bufferPtr = jsonBuffer;
	jsonBufferStart = jsonBuffer;
	for(i = 0; i < 22; i++)
	{
		clearBuffer(nodeArray[i].name,15);
		clearBuffer(nodeArray[i].abbr,4);
		nodeArray[i].nodeType = 0;
		nodeArray[i].value = 0;
		nodeArray[i].paramIndex = 0;
		nodeArray[i].up = 255;
		nodeArray[i].left = 255;
		nodeArray[i].right = 255;
		nodeArray[i].down = 255;
	}
	for(i = 0; i < 30; i++)
	{
		paramIndex2nodeArrayIndex[i] = 0;
	}
	while(done == 0 && jsonParseError == 0)
	{

		/************ information processing state machine ***********/

		switch(level)
		{
			case 0:
				//clearDebugString();
				openBracketPtr = strchr(bufferPtr,'[');
				closeBracketPtr = strchr(bufferPtr,']');
				openBracePtr = strchr(bufferPtr,'{');
				closeBracePtr = strchr(bufferPtr,'}');
				bufferPtr = openBracePtr;
				clearBuffer(tempBuffer,100);
				/*strncpy(lcdBuffer[0],jsonBuffer,19);
				strncpy(lcdBuffer[1],bufferPtr,19);
				sprintf(lcdBuffer[2],"%d",openBracketPtr);
				sprintf(lcdBuffer[3],"%d",bufferPtr);*/
				//strncpy(tempBuffer,(const char *)bufferPtr,90);
				/*addDebugData(tempBuffer);*/
				strncpy(tempBuffer,(const char *)bufferPtr,openBracketPtr-bufferPtr);


				currentComboNodeArrayIndex = getComboNode(tempBuffer);
				newNodeArrayIndex = currentComboNodeArrayIndex;

				nodeArray[currentComboNodeArrayIndex].down = 255;
				nodeArray[currentComboNodeArrayIndex].left = 255;
				nodeArray[currentComboNodeArrayIndex].right = 255;
				nodeArray[currentComboNodeArrayIndex].up = 255;

				openBracketPtr = strchr(bufferPtr,'[');
				bufferPtr = /*strchr(bufferPtr,'[')*/openBracketPtr+1;
				//if(openBracketPtr < closeBracketPtr)
				{
					level = 1;
				}
				//sendDebugString();

				break;
			case 1:

				//clearDebugString();
				openBracketPtr = strchr(bufferPtr,'[');
				closeBracketPtr = strchr(bufferPtr,']');
				openBracePtr = strchr(bufferPtr,'{');
				closeBracePtr = strchr(bufferPtr,'}');
				char *tempPtr;
				/*char tempRead[20];

				strncpy(tempRead,bufferPtr,20);*/

				if(strchr(bufferPtr,'[')==0)
				{
					done = 1;
					break;
				}
				bufferPtr = openBracePtr;
				tempPtr = openBracketPtr;
				/*strncpy(tempBuffer,(const char *)bufferPtr,90);
				addDebugData(tempBuffer);*/
				clearBuffer(effectElementString,100);
				//sendDebugString();
				if(openBracketPtr <= bufferPtr)
				{
					//sprintf(lcdBuffer[2],"%d:%d:%d,%d", effectElementIndex, paramElementIndex, tempPtr, bufferPtr);
					//sprintf(lcdBuffer[3],"%d,%d,%d,%d,%d", jsonBufferStart, openBracketPtr, closeBracketPtr, openBracePtr, closeBracePtr);
					jsonParseError = 1;
					break;
				}

				strncpy(effectElementString,(const char *)bufferPtr,openBracketPtr-bufferPtr);


// create node
				newNodeArrayIndex = getEffectNode(effectElementString);
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
				//if(openBracketPtr < closeBracketPtr)
				commaBracket = *(strchr(bufferPtr,'}')+1);

				/*if(commaBracket == ']')
				{
					level = 0;
					//closeBracePtr = strchr(bufferPtr,'}');
				}
				else*/
				{
					level = 2;
				}
				paramElementIndex=0;


				break;
			case 2:
				openBracketPtr = strchr(bufferPtr,'[');
				closeBracketPtr = strchr(bufferPtr,']');
				openBracePtr = strchr(bufferPtr,'{');
				closeBracePtr = strchr(bufferPtr,'}');
				//char *tempPtr;
				//if(closeBracketPtr < openBracketPtr)
				commaBracket = *(strchr(bufferPtr,'}')+1);
				tempPtr = closeBracePtr+1;
				clearBuffer(paramElementString,100);

				if(tempPtr <= bufferPtr)
				{
					//sprintf(lcdBuffer[2],"%d:%d:%d,%d", effectElementIndex, paramElementIndex, tempPtr, bufferPtr);
					jsonParseError = 1;
					break;
				}

				strncpy(paramElementString,(const char *)bufferPtr,tempPtr-bufferPtr);
				newNodeArrayIndex = getParamNode(paramElementString);

				//paramIndex2nodeArrayIndex[nodeArray[newNodeArrayIndex].paramIndex] = newNodeArrayIndex;
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
	//clearDebugString();
	currentNodeArrayIndex = currentComboNodeArrayIndex;
	currentComboNodeArrayIndex = 0;
	currentEffectNodeArrayIndex = 0;
	currentParamNodeArrayIndex = 0;
	nodeCount = newNodeArrayIndex;
	/************** Fill paramIndex2nodeArrayIndex array *********************/
	uint8_t tempParamIndex;
	uint8_t tempNodeArrayIndex;
	for(tempParamIndex = 0; tempParamIndex < 20; tempParamIndex++)
	{
		for(tempNodeArrayIndex = 0; tempNodeArrayIndex < 20; tempNodeArrayIndex++)
		{
			if(nodeArray[tempNodeArrayIndex].nodeType == 2  && nodeArray[tempNodeArrayIndex].paramIndex == tempParamIndex)
			{
				paramIndex2nodeArrayIndex[tempParamIndex] = tempNodeArrayIndex;
				break;
			}
		}
	}
	/*sprintf(tempBuffer,"nodeArrayIndex: %d|", nodeArrayIndex);
	//addDebugData(tempBuffer);
	for(i = 0; i < 15; i++)
	{
		sprintf(tempBuffer,"%d:%s:%s:%d,%d,%d,%d|", i, nodeArray[i].name, nodeArray[i].abbr, nodeArray[i].up,
				nodeArray[i].left, nodeArray[i].right, nodeArray[i].down);
		//addDebugData(tempBuffer);
	}*/
	//sendDebugString();

	return jsonParseError;
}


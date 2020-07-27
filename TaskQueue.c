/*
 * Tasks.c
 *
 *  Created on: Dec 5, 2016
 *      Author: buildrooteclipse
 */

#include "TaskQueue.h"

#include "utilityFuncts.h"
#include <stddef.h>
#include <limits.h>
#include <stdlib.h>




#define QUEUE_SIZE 20

static uint8_t taskQueue[QUEUE_SIZE];
static uint8_t taskQueueRear;
static uint8_t taskQueueFront;
static uint8_t taskQueueSize;



int isFull(void)
{
	return (taskQueueSize == QUEUE_SIZE);
}

// Queue is empty when size is 0
int isEmpty(void)
{
	return (taskQueueSize == 0);
}


void initTaskQueue(void)
{
	taskQueueRear = 0;
	taskQueueFront = 0;
	taskQueueSize = 0;
}

void insertTask(uint8_t taskNumber)
{
	if (isFull())
		return;
	taskQueueRear = (taskQueueRear + 1) % QUEUE_SIZE;
	taskQueue[taskQueueRear] = taskNumber;
	taskQueueSize = taskQueueSize + 1;
}

// Function to remove an item from queue.
// It changes front and size
uint8_t getTask(void)
{
	if (isEmpty())
		return 0;
	uint8_t item = taskQueue[taskQueueFront];
	taskQueueFront = (taskQueueFront + 1) % QUEUE_SIZE;
	taskQueueSize = taskQueueSize - 1;
	return item;
}










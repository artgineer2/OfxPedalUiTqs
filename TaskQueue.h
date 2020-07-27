/*
 * Tasks.h
 *
 *  Created on: Dec 5, 2016
 *      Author: buildrooteclipse
 */

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <stdint.h>


extern void initTaskQueue(void);
extern void insertTask(uint8_t taskNumber);
extern uint8_t getTask(void);


#endif /* TASKQUEUE_H_ */

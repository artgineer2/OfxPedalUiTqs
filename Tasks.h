/*
 * Tasks.h
 *
 *  Created on: Dec 5, 2016
 *      Author: buildrooteclipse
 */

#ifndef TASKS_H_
#define TASKS_H_

#include "JsonFuncts.h"
#include "UserInterface.h"

uint8_t testTask1(void);		// Task number: 10
uint8_t testTask2(void);		// Task number:11
uint8_t testTask3(void);		// Task number:12
uint8_t testTask4(void);		// Task number:13

uint8_t getComboList(void);		// Task number:1
uint8_t browseComboTitles(void);		// Task number:2
uint8_t changeJackParameters(void);		// Task number:3
uint8_t loadCombo(void);		// Task number:4
uint8_t saveCombo(void);		// Task number:5
uint8_t browseComboEffectParameters(void);		// Task number:6
//uint8_t goBack(void);// Task number:6
//uint8_t selectParameter(void);		// Task number:6
uint8_t changeComboEffectParameter(void);		// Task number:7


uint8_t requestStatusUpdateFromCM(void);		// Task number:1


#endif /* TASKS_H_ */

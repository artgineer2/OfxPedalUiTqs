/*
 * Typedefs.h
 *
 *  Created on: Jul 18, 2020
 *      Author: mike
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdint.h>
//#define NULL 0

typedef struct fsw{
	uint8_t physicalStatus; // is footswitch physically pressed down by user's foot?
	uint8_t value;	// variable to be toggled
	uint8_t change;
} Fsw_t;


typedef struct {
	uint8_t nodeType; // 0=comb0, 1=effect, 2=parameter
	char abbr[4];
	char name[15];
	uint8_t value;
	uint8_t valueType;
	uint8_t paramIndex;
	uint8_t up;		// array indexes for up,left,right, and down nodes.
	uint8_t left;
	uint8_t right;
	uint8_t down;
} Node_t;

typedef enum
{
	GetComboList = 1,
	//BrowseComboTitles,
	LoadCombo,
	SaveCombo,
	ChangeComboEffectParameter,
	RequestStatusUpdateFromCM
} TaskNumbers_t;


#endif /* TYPEDEFS_H_ */

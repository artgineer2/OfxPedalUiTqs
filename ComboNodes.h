/*
 * ComboNodes.h
 *
 *  Created on: Jul 19, 2020
 *      Author: mike
 */

#ifndef COMBONODES_H_
#define COMBONODES_H_

#include <stdint.h>
#include "Typedefs.h"


extern void initComboNodes(void);
extern uint8_t jsonBuffer2UiMenu(void);
extern uint8_t decrementMenuLevel(uint8_t currentMenuLevel);
extern uint8_t incrementMenuLevel(uint8_t index, uint8_t currentMenuLevel);

extern Node_t getNode(uint8_t nodeIndex);
extern Node_t getComboNode(void);
extern Node_t getCurrentNode(void);
extern Node_t getParentNode(void);
extern char *getJsonBufferPtr(void);
extern uint8_t getCurrentComboIndex(void);


#endif /* COMBONODES_H_ */

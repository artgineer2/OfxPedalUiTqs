/*
 * UserInterfaceOps.h
 *
 *  Created on: Jul 19, 2020
 *      Author: mike
 */

#ifndef USERINTERFACEOPS_H_
#define USERINTERFACEOPS_H_

#include <stdint.h>

extern void initUserInterfaceOps(void);
extern uint8_t browseComboEffectParameters(void);
extern void updateSoftKeyLabels(void);
extern void parseComboList(void);
extern char *getComboName(uint8_t index);
extern uint8_t getMenuLevel(void);
extern void setMenuLevel(uint8_t menuLevel);
extern void updateDisplay(uint8_t hostUiActive);
extern void createTasks(void);
extern char *getComboListBufferPtr(void);
extern uint8_t getPedalUiMode(void);
extern uint8_t isPedalGoingBackToNormalUiMode(void);
extern void setPedalUiMode(void);
extern void updateStatus(char *statusString);
#endif /* USERINTERFACEOPS_H_ */

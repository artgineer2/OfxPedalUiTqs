/*
 * UserInterface.h
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#ifndef USERINTERFACEDRIVERS_H_
#define USERINTERFACEDRIVERS_H_


#include <stdint.h>



extern void initUserInterfaceDrivers(void);

extern void readButtons(void);
extern void readEncoder(void);




extern uint8_t getButtonPushed(void);
extern int8_t getEncoderDirection(void);
extern void disablePowerOff(void);
extern void enablePowerOff(void);
extern uint8_t readPowerButton(void);
extern void powerOff(void);
extern void writeDisplay(uint8_t *line_1, uint8_t *line_2, uint8_t *line_3, uint8_t *line_4);
extern void writeDisplayLine(uint8_t lineNumber, uint8_t *lineStr);

extern void getValueString(uint8_t valueIndex, uint8_t valueType, char *valueStr);
extern void updateFootswitchLeds(void);

extern uint8_t updateParamValues(void);
extern void updateSoftKeyLabels(void);
extern uint8_t getValueChange(void);
extern void setValueChange(uint8_t value);

#endif /* USERINTERFACEDRIVERS_H_ */

/*
 * CMFuncts.h
 *
 *  Created on: Mar 10, 2016
 *      Author: mike
 */

#ifndef COMPUTEMODULEINTERFACE_H_
#define COMPUTEMODULEINTERFACE_H_

#include <stdint.h>

extern void initComputeModuleInterface(void);

extern void initSerialRam(void);
extern void clearSerialRam(uint8_t cmIndex, uint16_t address, uint16_t length);
extern void sendStringToComputeModule(char *sharedMemoryTxBufferPtr, uint8_t txSize, char *sharedMemoryRxBufferPtr, uint16_t rxSize);
extern void resetCurrentDataUpdateTimer(void);
extern void resetComputeModule(void);
extern uint8_t isComputeModuleRunning(void);
extern uint8_t isOfxMainRunning(void);

#endif /* COMPUTEMODULEINTERFACE_H_ */

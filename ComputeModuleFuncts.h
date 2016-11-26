/*
 * CMFuncts.h
 *
 *  Created on: Mar 10, 2016
 *      Author: mike
 */

#ifndef COMPUTEMODULEFUNCTS_H_
#define COMPUTEMODULEFUNCTS_H_

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "macros.h"
#include "utilityFuncts.h"

void spiTx(uint8_t data);
uint8_t spiRx(void);

void enableMcuSharedMemoryInterface(void);
void disableMcuSharedMemoryInterface(void);
void enableMcuSharedMemorySectionAccess(void);
void disableMcuSharedMemorySectionAccess(void);
void enableUsbSharedMemoryAccess(void);
void disableUsbSharedMemoryAccess(void);

void enableComputeModuleSharedMemoryInterface(uint8_t index);
void disableComputeModuleSharedMemoryInterface(uint8_t index);
void enableComputeModuleSharedMemorySectionAccess(uint8_t index);
void disableComputeModuleSharedMemorySectionAccess(uint8_t index);


void initSerialRam(void);
uint8_t writeToComputeModuleSharedMemorySection(uint8_t cmIndex, uint16_t address, char *buffer, uint16_t bufferSize);
uint8_t readFromComputeModuleSharedMemorySection(uint8_t cmIndex, uint16_t address, char *buffer, uint16_t bufferSize);
void requestComputeModuleSharedMemorySectionAccess(uint8_t index);
void releaseComputeModuleSharedMemorySectionAccess(uint8_t index);
void clearSerialRam(uint8_t cmIndex, uint16_t address, uint16_t length);
void testSerialRam(uint8_t cmIndex, uint16_t address, uint16_t length);
uint8_t getComputeModuleDataProcessingStatus(uint8_t index);
uint8_t sendDataToComputeModuleSharedMemorySection(uint8_t index, uint16_t address);
uint8_t processDataFromComputeModuleSharedMemorySection(uint8_t index);
void processComputeModuleSharedMemorySections(void);


#endif /* COMPUTEMODULEFUNCTS_H_ */

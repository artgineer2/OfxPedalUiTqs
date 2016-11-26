/*
 * IoDrivers.h
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#ifndef PINDRIVERS_H_
#define PINDRIVERS_H_

#include <avr/io.h>
#include "IoLines.h"
#include "macros.h"

void LcdWrite(void);
void LcdDataEn(void);
void LcdInstrEn(void);
void SPI_Tx(uint8_t data);
uint8_t SPI_Rx(void);
uint8_t flash_read(unsigned long address);
uint8_t flash_write(unsigned long address, uint8_t data);
uint8_t flash_seq_write(unsigned long start_address, uint8_t data_in, uint8_t uint8_t_status);
uint8_t flash_array_read(unsigned long start_address, uint8_t *data_out, uint8_t array_size);
uint8_t flash_array_write(unsigned long start_address, uint8_t *data_in, uint8_t array_size);
uint8_t flash_4K_erase(unsigned long starting_address);
uint8_t flash_64K_erase(unsigned long starting_address);
uint8_t flash_chip_erase(void);



#endif /* PINDRIVERS_H_ */

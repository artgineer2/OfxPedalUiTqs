/*
 * UserInterface.h
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#ifndef USERINTERFACE_H_
#define USERINTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include "PinDrivers.h"
#include "IoLines.h"
#include "macros.h"
#include <avr/pgmspace.h>


#define LCD_LINE1_ADDR 0x00
#define LCD_LINE2_ADDR 0x40
#define LCD_LINE3_ADDR 0x14
#define LCD_LINE4_ADDR 0x54
#define SET_DDRAM_ADDRESS 0x80

//void clearBuffer(char *buffer, uint8_t size);
void LCD_data(uint8_t data);
void LCD_instr(uint8_t data);
uint8_t read_switch(void);
uint8_t RotaryDir(void);
void RotCount(uint8_t dir, uint8_t *count);
void LCD_init(void);
void UI_IndBuf_load(uint8_t state, uint8_t index, uint8_t index_max);
void CapInd(char *target, char *source, uint8_t select);
void Display(uint8_t *line_1, uint8_t *line_2, uint8_t *line_3, uint8_t *line_4);
void DisplayStatus(uint8_t *line3);

void getComboName(uint8_t index);
void getCombo(uint8_t index);
void saveCombo(void);
void processPedalUI();
void updateParamValues(void);
void updateStatus(void);
#endif /* USERINTERFACE_H_ */

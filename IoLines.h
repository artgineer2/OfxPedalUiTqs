/*
 * IoLines.h
 *
 *  Created on: Jan 14, 2016
 *      Author: mike
 */

#ifndef IOLINES_H_
#define IOLINES_H_

//#include "macros.h"


/*** PORTA ***/
// button lines
#define LCD_LEFT 	6
#define SOFT_KEY_1	5
#define SOFT_KEY_2	4
#define ROTARY_BUTTON 3
#define SOFT_KEY_3	2
#define SOFT_KEY_4	1
#define LCD_RIGHT 	0
#define CHANGE_COMBO		7

/*#define LCD_LEFT 	6
#define SOFT_KEY_1	0
#define SOFT_KEY_2	1
#define SOFT_KEY_3	2
#define ROTARY_BUTTON 3
#define SOFT_KEY_4	4
#define LCD_RIGHT 	5
#define CHANGE_COMBO		7*/


/*** PORTB ***/
#define BUTTON_PUSHED		7
#define LCD_RS		6    //Xmega: 2
#define LCD_RW		5    //Xmega: 1
#define LCD_E		4    //Xmega: 0
#define MCU_MISO 	3    //Xmega: 6
#define MCU_MOSI 	2    //Xmega: 5
#define MCU_SCLK 	1    //Xmega: 7
#define MCU_nCS  	0    //Xmega: 4

/*** PORTC ***/
// LCD data lines

/*** PORTD ***/
// Compute modules
#define nMCU_SM_ACCESS		6 //
#define FSW2 5 //was CM1_RDY		5
#define FSW1 4 // was CM1_RUN		4
#define FSW2_LED		3
#define FSW1_LED		2
#define ROT_A		1
#define ROT_B		0

/*** PORTE ***/
#define	DATA_RXED1	7 // status of data in CM1 shared memory
#define nMCU_DATA_RDY1	6 // tell CM1 new data from MCU is ready in shared memory
#define OFX_RDY1	5 // status of FLX software on CM1
#define CM_RUNNING1	4 // status of CM1
#define DATA_RXED0	3 // status of data from MCU in CM0 shared memory
#define nMCU_DATA_RDY0 2 // tell CM0 new data is ready in shared memory
#define OFX_RDY0 1 // status of FLX software on CM0
#define CM_RUNNING0 0 // status of CM0


/*** PORTF ***/
#define	nUSB_SM_ACCESS	7
#define	USB_GP7	6
#define USB_GP6 5
#define	USB_GP5	4
// GP4 used as /CS for Serial RAM
#define	nUSB_GP3_DATA_RDY1	3
#define	nUSB_GP2_DATA_RDY0	2
#define	USB_GP1_SPI_CONTROL_GRANTED	1
#define	USB_GP0_SPI_CONTROL_RQST	0

/*** PORTG ***/
#define nPOWER_OFF 0 // change back to 0 for other boards.  Pin 0 was fried on first board.
#define MCU_POWER_CONTROL 1
#define CMX_RUN 2

#endif /* IOLINES_H_ */

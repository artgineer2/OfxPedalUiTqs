/*
 * Tasks.h
 *
 *  Created on: Jul 19, 2020
 *      Author: mike
 */

#ifndef TASKS_H_
#define TASKS_H_

#include <stdint.h>

extern void getComboList(void);
extern void loadCombo(void);
extern void saveCombo(void);
extern void changeComboEffectParameter(void);
extern void requestStatusUpdateFromCM(void);

extern void powerOffCM(void);
#endif /* TASKS_H_ */

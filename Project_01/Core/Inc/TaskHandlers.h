/*
 * TaskHandlers.h
 *
 *  Created on: Apr 20, 2024
 *      Author: Mousa Mahmoud
 */

#ifndef INC_TASKHANDLERS_H_
#define INC_TASKHANDLERS_H_

#include <stdint.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "DateTime.h"
#include "LedEffect.h"

typedef struct _Command
{
	uint8_t Payload[10U];
	uint8_t Len;
} Command_t;

typedef enum _MenusState
{
	MAIN_MENU_STATE = 0U,
	LED_EFFECT_STATE,
	RTC_MENU_STATE,
	RTC_TIMECFG_STATE,
	RTC_DATECFG_STATE,
	RTC_REPORT_STATE
} MenusState_e;

void MenuTask(void *Copy_pvParameters);
void LEDTask(void *Copy_pvParameters);
void RTCTask(void *Copy_pvParameters);
void CmdHandlerTask(void *Copy_pvParameters);
void PrintTask(void *Copy_pvParameters);

#endif /* INC_TASKHANDLERS_H_ */

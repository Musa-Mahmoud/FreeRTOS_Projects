/*
 * Date_Time.h
 *
 *  Created on: May 5, 2024
 *      Author: Mousa Mahmoud
 */

#ifndef INC_DATE_TIME_H_
#define INC_DATE_TIME_H_

#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

#include "FreeRTOS.h"
#include "queue.h"

void DateTime_voidShowDateTime(void);

void DateTime_voidConfigDate(uint8_t Copy_u8WeekDay, uint8_t Copy_u8Day, uint8_t Copy_u8Mon, uint8_t Copy_u8Year);

void DateTime_voidConfigTime(uint8_t Copy_u8Hour12Fmt, uint8_t Copy_u8Hour, uint8_t Copy_u8Min, uint8_t Copy_u8Sec);

uint8_t DateTime_u8IsDateCfgValid(uint8_t Copy_u8WeekDay, uint8_t Copy_u8Day, uint8_t Copy_u8Mon, uint8_t Copy_u8Year);

uint8_t DateTime_u8IsTimeCfgValid(uint8_t Copy_u8Hour12Fmt, uint8_t Copy_u8Hour, uint8_t Copy_u8Min, uint8_t Copy_u8Sec);

void DateTime_voidReportDateTime_ITM(void);

#endif /* INC_DATE_TIME_H_ */

/*
 * Date_Time.c
 *
 *  Created on: May 5, 2024
 *      Author: Mousa Mahmoud
 */

#include "DateTime.h"

/* Private Macros */

#define NUM_DAYS_IN_WEEK		(7U)

/* Private Functions Macro */

#define IS_LEAP_YEAR(YEAR)		( ((YEAR) % 400UL == 0UL) || ( ((YEAR) % 100UL != 0UL) && ((YEAR) % 4UL == 0UL) ) )

/* External Variables */

extern RTC_HandleTypeDef hrtc;

extern QueueHandle_t TxQueueHandle;

/* Private Variables */

static const char *WeekDays[NUM_DAYS_IN_WEEK] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

/* Public Functions */

void DateTime_voidShowDateTime(void)
{
	static char Local_TimeStr[50U];
	static char Local_DateStr[50U];

	const char *Local_pcTimeFormat;
	char *Local_pTime = Local_TimeStr;
	char *Local_pDate = Local_DateStr;

	RTC_DateTypeDef Local_Date;
	RTC_TimeTypeDef Local_Time;

	(void) HAL_RTC_GetTime(&hrtc, &Local_Time, RTC_FORMAT_BIN);
	(void) HAL_RTC_GetDate(&hrtc, &Local_Date, RTC_FORMAT_BIN);

	/* Prepare the format */
	Local_pcTimeFormat = (RTC_HOURFORMAT12_AM == Local_Time.TimeFormat) ? "AM" : "PM";

	/* Conver the time format into formatted sting: HH:MM:SS [AM/PM] */
	(void) sprintf((char *) Local_TimeStr,
			"%s: %02d:%02d:%02d [%s]",
			"\r\nCurrent Date & Time", Local_Time.Hours, Local_Time.Minutes,
			Local_Time.Seconds, Local_pcTimeFormat);

	/* Print the current time */
	(void) xQueueSend(TxQueueHandle, (const void *) &Local_pTime, portMAX_DELAY);

	/* Conver the date format into formatted sting: Day - Month - year */
	(void) sprintf((char *) Local_DateStr,
			"\t%s %02d/%02d/20%02d\r\n", WeekDays[Local_Date.WeekDay - 1U],
			Local_Date.Date, Local_Date.Month, Local_Date.Year);

	/* Print the current date */
	(void) xQueueSend(TxQueueHandle, (const void *) &Local_pDate, portMAX_DELAY);
}

void DateTime_voidConfigDate(uint8_t Copy_u8WeekDay, uint8_t Copy_u8Day, uint8_t Copy_u8Mon, uint8_t Copy_u8Year)
{
	RTC_DateTypeDef Local_Date;

	Local_Date.WeekDay = Copy_u8WeekDay;
	Local_Date.Date = Copy_u8Day;
	Local_Date.Month = Copy_u8Mon;
	Local_Date.Year = Copy_u8Year;

	(void) HAL_RTC_SetDate(&hrtc, &Local_Date, RTC_FORMAT_BIN);
}

void DateTime_voidConfigTime(uint8_t Copy_u8Hour12Fmt, uint8_t Copy_u8Hour, uint8_t Copy_u8Min, uint8_t Copy_u8Sec)
{
	RTC_TimeTypeDef Local_Time = {0U};

	Local_Time.TimeFormat = Copy_u8Hour12Fmt;
	Local_Time.Hours = Copy_u8Hour;
	Local_Time.Minutes = Copy_u8Min;
	Local_Time.Seconds = Copy_u8Sec;

	(void) HAL_RTC_SetTime(&hrtc, &Local_Time, RTC_FORMAT_BIN);
}

uint8_t DateTime_u8IsDateCfgValid(uint8_t Copy_u8WeekDay, uint8_t Copy_u8Day, uint8_t Copy_u8Mon, uint8_t Copy_u8Year)
{
	uint8_t Local_u8ValidityState = 1U;
	static uint8_t Local_u8DaysInMonths[12U] = { 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U };

	Local_u8DaysInMonths[1U] = (IS_LEAP_YEAR(2000UL + Copy_u8Year)) ? 29U : 28U;

	if ((Copy_u8Year > 99U) || (Copy_u8Mon > 12U) || (Copy_u8Mon == 0U)
			|| (Copy_u8Day == 0U) || (Copy_u8Day > Local_u8DaysInMonths[Copy_u8Mon - 1U])
			|| (Copy_u8WeekDay < RTC_WEEKDAY_MONDAY) || (Copy_u8WeekDay > RTC_WEEKDAY_SUNDAY))
	{
		Local_u8ValidityState = 0U;
	}

	return Local_u8ValidityState;
}

uint8_t DateTime_u8IsTimeCfgValid(uint8_t Copy_u8Hour12Fmt, uint8_t Copy_u8Hour, uint8_t Copy_u8Min, uint8_t Copy_u8Sec)
{
	uint8_t Local_u8ValidityState = 1U;

	if (((Copy_u8Hour12Fmt != RTC_HOURFORMAT12_PM) && (Copy_u8Hour12Fmt != RTC_HOURFORMAT12_AM))
			|| (Copy_u8Hour > 12U) || (0U == Copy_u8Hour)
			|| (Copy_u8Min > 59U) || (Copy_u8Sec > 59U))
	{
		Local_u8ValidityState = 0U;
	}

	return Local_u8ValidityState;
}

void DateTime_voidReportDateTime_ITM(void)
{
	const char *Local_pcTimeFormat;

	RTC_DateTypeDef Local_Date;
	RTC_TimeTypeDef Local_Time;

	(void) HAL_RTC_GetTime(&hrtc, &Local_Time, RTC_FORMAT_BIN);
	(void) HAL_RTC_GetDate(&hrtc, &Local_Date, RTC_FORMAT_BIN);

	/* Prepare the format */
	Local_pcTimeFormat = (RTC_HOURFORMAT12_AM == Local_Time.TimeFormat) ? "AM" : "PM";

	/* Print the current time */
	(void) printf("\r\nCurrent Date & Time: %02d:%02d:%02d [%s]",
			Local_Time.Hours, Local_Time.Minutes,Local_Time.Seconds, Local_pcTimeFormat);

	/* Print the current date */
	(void) printf("\t%s %02d/%02d/20%02d\r\n", WeekDays[Local_Date.WeekDay - 1U],
			Local_Date.Date, Local_Date.Month, Local_Date.Year);
}

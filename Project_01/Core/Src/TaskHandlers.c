/*
 * TaskHandlers.c
 *
 *  Created on: Apr 8, 2024
 *      Author: Mousa Mahmoud
 */

#include "TaskHandlers.h"

/* External Variables */

extern UART_HandleTypeDef huart2;

extern TaskHandle_t MenuTaskHandle;
extern TaskHandle_t CommandTaskHandle;
extern TaskHandle_t LEDTaskHandle;
extern TaskHandle_t RTCTaskHandle;
extern TaskHandle_t PrintTaskHandle;

extern QueueHandle_t TxQueueHandle;
extern QueueHandle_t RxQueueHandle;

extern TimerHandle_t ReportTimerHandle;

/* Private Macros */

#define HH12_FORMAT_STATE	(0U)
#define HH_STATE			(1U)
#define MM_STATE			(2U)
#define SS_STATE			(3U)

#define WEEK_DAY_STATE		(0U)
#define DAY_STATE			(1U)
#define MON_STATE			(2U)
#define YEAR_STATE			(3U)

/* Private Functions Prototypes */

static void ProcessCommand(Command_t *Copy_pCmd);
static void ExtractCommand(Command_t *Copy_pCmd);
static uint8_t u8StrToNum(const char *Copy_pcNumAsStr, uint8_t Copy_u8Len);
static void RtcMenuHandler(Command_t *Copy_pCmd, uint8_t *Copy_pu8RtcCfgState);
static void RtcTimeCfgHandler(Command_t *Copy_pCmd, uint8_t *Copy_pu8RtcCfgState);
static void RtcDateCfgHandler(Command_t *Copy_pCmd, uint8_t *Copy_pu8RtcCfgState);
static void RtcReportingHandler(Command_t *Copy_pCmd);

/* Private Variables */

static MenusState_e CurrentState = MAIN_MENU_STATE;
static const char *InvalidMsg = "\r\n!!! Invalid Option. !!!\r\n";
static const char *ConfigSuccessMsg = "\r\nConfiguration setted successfully.\r\n";

/* Public Functions Implementation */

void MenuTask(void *Copy_pvParameters)
{
	const char *MainMenu =
			"\r\n"
			"===============================\r\n"
			"||         Main Menu         ||\r\n"
			"===============================\r\n"
			"[0] For LEDs' Effect Menu\r\n"
			"[1] For date and time Menu\r\n"
			"[2] For exit\r\n"
			"Enter your choice here: ";
	Command_t *Local_pCmd;
	uint32_t Local_u32CmdAddr;

	while (1)
	{
		static uint8_t Local_u8IsCmdValid = 1U;

		xQueueSend(TxQueueHandle, (const void *) &MainMenu, portMAX_DELAY);

		/* Wait until receiving a notification containg the address of the command */
		xTaskNotifyWait(0U, 0U, (uint32_t *) &Local_u32CmdAddr, portMAX_DELAY);

		Local_pCmd = (Command_t *) Local_u32CmdAddr;

		/* Command length should be 1, otherwise it's invalid */
		if (1U == Local_pCmd->Len)
		{
			switch (Local_pCmd->Payload[0U])
			{
			case '0': CurrentState = LED_EFFECT_STATE; xTaskNotify(LEDTaskHandle,0U, eNoAction); break;
			case '1': CurrentState = RTC_MENU_STATE; xTaskNotify(RTCTaskHandle,0U, eNoAction); break;
			case '2': break;
			default:
				xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
				Local_u8IsCmdValid = 0U;
				break;
			}
		}
		else
		{
			xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
			Local_u8IsCmdValid = 0U;;
		}

		/* Wait until getting notified */
		if (Local_u8IsCmdValid)
		{
			xTaskNotifyWait(0U, 0U, NULL, portMAX_DELAY);
		}
	}
}

void LEDTask(void *Copy_pvParameters)
{
	Command_t *Local_pCmd;
	uint32_t Local_u32CmdAddr;
	const char *LEDsEffectMenu =
			"\r\n"
			"===============================\r\n"
			"||     LEDs' Effect Menu     ||\r\n"
			"===============================\r\n"
			"[none] For stopping the effect\r\n"
			"[e1]   For the togglling effect\r\n"
			"[e2]   For the even-odd effect\r\n"
			"[e3]   For the shift right effect\r\n"
			"[e4]   For the shift left effect\r\n"
			"Enter your choice here: ";
	while (1)
	{
		/* Wait for notification */
		xTaskNotifyWait(0U, 0U, NULL, portMAX_DELAY);

		/* Print the menu */
		xQueueSend(TxQueueHandle, (const void *) &LEDsEffectMenu, portMAX_DELAY);

		/* Wait for the command */
		xTaskNotifyWait(0U, 0U, (uint32_t *) &Local_u32CmdAddr, portMAX_DELAY);

		Local_pCmd = (Command_t *) Local_u32CmdAddr;

		if (Local_pCmd->Len <= 4U)
		{
			if (0U == strcmp("none", (const char *) Local_pCmd->Payload))
			{
				LedEffectStop();
			}
			else if (0U == strcmp("e1", (const char *) Local_pCmd->Payload))
			{
				LedEffectStart(1U);
			}
			else if (0U == strcmp("e2", (const char *) Local_pCmd->Payload))
			{
				LedEffectStart(2U);
			}
			else if (0U == strcmp("e3", (const char *) Local_pCmd->Payload))
			{
				LedEffectStart(3U);
			}
			else if (0U == strcmp("e4", (const char *) Local_pCmd->Payload))
			{
				LedEffectStart(4U);
			}
			else
			{
				/* Invalid option */
				xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
			}
		}
		else
		{
			/* Invalid comman */
			xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
		}

		CurrentState = MAIN_MENU_STATE;

		xTaskNotify(MenuTaskHandle, 0U, eNoAction);
	}
}

void RTCTask(void *Copy_pvParameters)
{
	const char *RTCMenu1 =
			"\r\n"
			"================================\r\n"
			"||      Date & Time Menu      ||\r\n"
			"================================\r\n";
	const char *RTCMenu2 =
			"\r\n"
			"[0] For setting time\r\n"
			"[1] For setting date\r\n"
			"[2] For enabling always reporting\r\n"
			"[3] For exiting the menu\r\n"
			"Enter your choice here: ";
	Command_t *Local_pCmd;
	uint32_t Local_CmdAddr;
	static uint8_t Local_u8RtcCfgState = HH_STATE;

	while (1)
	{
		/* Wait until being notified */
		xTaskNotifyWait(0U, 0U, NULL, portMAX_DELAY);

		/* Print the menu, show the current date and time information */
		xQueueSend(TxQueueHandle, (const void *) &RTCMenu1, portMAX_DELAY);
		DateTime_voidShowDateTime();
		xQueueSend(TxQueueHandle, (const void *) &RTCMenu2, portMAX_DELAY);

		/* Loop until MAIN_MENU_STATE is invoked */
		while (MAIN_MENU_STATE != CurrentState)
		{
			/* Wait for command notification */
			xTaskNotifyWait(0U, 0U, (uint32_t *) &Local_CmdAddr, portMAX_DELAY);
			Local_pCmd = (Command_t *) Local_CmdAddr;

			/* Switch on the current state */
			switch (CurrentState)
			{
			case RTC_MENU_STATE: RtcMenuHandler(Local_pCmd, &Local_u8RtcCfgState); break;
			case RTC_TIMECFG_STATE: RtcTimeCfgHandler(Local_pCmd, &Local_u8RtcCfgState); break;
			case RTC_DATECFG_STATE: RtcDateCfgHandler(Local_pCmd, &Local_u8RtcCfgState); break;
			case RTC_REPORT_STATE: RtcReportingHandler(Local_pCmd); break;
			default:
				CurrentState = MAIN_MENU_STATE;
				xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
				break;
			}
		}

		xTaskNotify(MenuTaskHandle, 0U, eNoAction);
	}
}

void CmdHandlerTask(void *Copy_pvParameters)
{
	BaseType_t Local_RetVal;
	Command_t Cmd;

	while (1)
	{
		Local_RetVal = xTaskNotifyWait(0U, 0U, NULL, portMAX_DELAY);

		if (pdTRUE == Local_RetVal)
		{
			ProcessCommand(&Cmd);
		}
	}
}

void PrintTask(void *Copy_pvParameters)
{
	uint32_t *Local_pu32Msg = NULL;

	while (1)
	{
		xQueueReceive(TxQueueHandle, &Local_pu32Msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t *) Local_pu32Msg, strlen((char *) Local_pu32Msg), HAL_MAX_DELAY);
	}
}

/* Private Functions Implementation */

static void ProcessCommand(Command_t *Copy_pCmd)
{
	ExtractCommand(Copy_pCmd);

	switch (CurrentState)
	{
	case MAIN_MENU_STATE:
		/* Notify main menu task of the command */
		xTaskNotify(MenuTaskHandle, (uint32_t) Copy_pCmd, eSetValueWithOverwrite); break;
	case LED_EFFECT_STATE:
		/* Notify LED task of the command */
		xTaskNotify(LEDTaskHandle, (uint32_t) Copy_pCmd, eSetValueWithOverwrite); break;
	case RTC_MENU_STATE:
	case RTC_TIMECFG_STATE:
	case RTC_DATECFG_STATE:
	case RTC_REPORT_STATE:
		/* Notify LED task of the command */
		xTaskNotify(RTCTaskHandle, (uint32_t) Copy_pCmd, eSetValueWithOverwrite); break;
	}
}

static void ExtractCommand(Command_t *Copy_pCmd)
{
	UBaseType_t Local_NumOfMsgs;
	uint8_t Local_u8LoopCounter;

	Local_NumOfMsgs = uxQueueMessagesWaiting(RxQueueHandle);

	for (Local_u8LoopCounter = 0U; Local_u8LoopCounter < Local_NumOfMsgs; Local_u8LoopCounter++)
	{
		xQueueReceive(RxQueueHandle, (void *const) &Copy_pCmd->Payload[Local_u8LoopCounter], 0U);

		if ('\r' == (char) Copy_pCmd->Payload[Local_u8LoopCounter])
		{
			break;
		}
	}

	/* Insert null terminating character at the end of the command */
	Copy_pCmd->Payload[Local_u8LoopCounter] = '\0';
	/* Save the command length excluding the null character */
	Copy_pCmd->Len = Local_u8LoopCounter;
}

static uint8_t u8StrToNum(const char *Copy_pcNumAsStr, uint8_t Copy_u8Len)
{
	uint8_t Local_u8DecimalNum = 0U;
    for (uint8_t Local_u8LoopCounter = 0U; Local_u8LoopCounter < Copy_u8Len; Local_u8LoopCounter++)
    {
        if (Copy_pcNumAsStr[Local_u8LoopCounter] >= '0' && Copy_pcNumAsStr[Local_u8LoopCounter] <= '9')
        {
            Local_u8DecimalNum *= 10U;
            Local_u8DecimalNum += Copy_pcNumAsStr[Local_u8LoopCounter] - '0';
        }
    }

    return Local_u8DecimalNum;
}

static void RtcMenuHandler(Command_t *Copy_pCmd, uint8_t *Copy_pu8RtcCfgState)
{
	const char *Msg12HourFmtConfing =
			"\r\n[ 0 -> AM, 1 -> PM ]"
			"\r\nEnter the 12-hour format  (0 - 1):  ";
	const char *MsgWeekDayConfing =
			"\r\n[ 1 -> Mon, 2 -> Tue, 3 -> Wed, 4 -> Thu, 5 -> Fri, 6 -> Sat, 7 -> Sun ]"
			"\r\nEnter the week day (1 - 7):  ";
	const char *MsgEnableReporting = "\r\nEnable date & time reporting? (y/n): ";

	if (1U == Copy_pCmd->Len)
	{
		switch (Copy_pCmd->Payload[0U])
		{
		case '0':
			CurrentState = RTC_TIMECFG_STATE;
			xQueueSend(TxQueueHandle, (const void *) &Msg12HourFmtConfing, portMAX_DELAY);
			*Copy_pu8RtcCfgState = HH12_FORMAT_STATE;
			break;
		case '1':
			CurrentState = RTC_DATECFG_STATE;
			xQueueSend(TxQueueHandle, (const void *) &MsgWeekDayConfing, portMAX_DELAY);
			*Copy_pu8RtcCfgState = WEEK_DAY_STATE;
			break;
		case '2':
			CurrentState = RTC_REPORT_STATE;
			xQueueSend(TxQueueHandle, (const void *) &MsgEnableReporting, portMAX_DELAY);
			break;
		case '3':
			CurrentState = MAIN_MENU_STATE;
			break;
		default:
			CurrentState = MAIN_MENU_STATE;
			xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
			break;
		}
	}
	else /* Invalid command */
	{
		CurrentState = MAIN_MENU_STATE;
		xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
	}
}

static void RtcTimeCfgHandler(Command_t *Copy_pCmd, uint8_t *Copy_pu8RtcCfgState)
{
	/* Get hour, minute, seconds info and configure RTC */
	/* Take case of invalid entries */
	const char *MsgHourConfing	= "\r\nEnter the hour            (1 - 12): ";
	const char *MsgMinConfing 	= "\r\nEnter the Minutes         (0 - 59): ";
	const char *MsgSecConfing 	= "\r\nEnter the seconds         (0 - 59): ";
	static uint8_t Local_u812HourFmt, Local_u8Hour, Local_u8Min, Local_u8Sec;

	if (Copy_pCmd->Len <= 2U)
	{
		switch (*Copy_pu8RtcCfgState)
		{
		case HH12_FORMAT_STATE:
			Local_u812HourFmt = u8StrToNum((const char *) Copy_pCmd->Payload, Copy_pCmd->Len);
			xQueueSend(TxQueueHandle, (const char *) &MsgHourConfing, portMAX_DELAY);
			*Copy_pu8RtcCfgState = HH_STATE;
			break;

		case HH_STATE:
			Local_u8Hour = u8StrToNum((const char *) Copy_pCmd->Payload, Copy_pCmd->Len);
			xQueueSend(TxQueueHandle, (const char *) &MsgMinConfing, portMAX_DELAY);
			*Copy_pu8RtcCfgState = MM_STATE;
			break;

		case MM_STATE:
			Local_u8Min = u8StrToNum((const char *) Copy_pCmd->Payload, Copy_pCmd->Len);
			xQueueSend(TxQueueHandle, (const char *) &MsgSecConfing, portMAX_DELAY);
			*Copy_pu8RtcCfgState = SS_STATE;
			break;

		case SS_STATE:
			Local_u8Sec = u8StrToNum((const char *) Copy_pCmd->Payload, Copy_pCmd->Len);

			if (DateTime_u8IsTimeCfgValid(Local_u812HourFmt, Local_u8Hour, Local_u8Min, Local_u8Sec))
			{
				/* Valid time config */
				DateTime_voidConfigTime(Local_u812HourFmt, Local_u8Hour, Local_u8Min, Local_u8Sec);
				xQueueSend(TxQueueHandle, (const char *) &ConfigSuccessMsg, portMAX_DELAY);
				DateTime_voidShowDateTime();
			}
			else /* Invalid command */
			{
				xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
			}

			*Copy_pu8RtcCfgState = HH12_FORMAT_STATE;
			CurrentState = MAIN_MENU_STATE;
			break;

		default:
			CurrentState = MAIN_MENU_STATE;
			xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
			break;
		}
	}
	else /* Invalid command */
	{
		CurrentState = MAIN_MENU_STATE;
		xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
	}
}

static void RtcDateCfgHandler(Command_t *Copy_pCmd, uint8_t *Copy_pu8RtcCfgState)
{
	/* Get day, month, year info and configure RTC */
	/* Take case of invalid entries */
	const char *MsgDayConfing = 	"\r\nEnter the day      (1 - 31): ";
	const char *MsgMonConfing =		"\r\nEnter the Month    (1 - 12): ";
	const char *MsgYearConfing =	"\r\nEnter the year     (0 - 99): ";
	static uint8_t Local_u8WeekDay, Local_u8Day, Local_u8Mon, Local_u8Year;

	if (Copy_pCmd->Len <= 2U)
	{
		switch (*Copy_pu8RtcCfgState)
		{
		case WEEK_DAY_STATE:
			Local_u8WeekDay = u8StrToNum((const char *) Copy_pCmd->Payload, Copy_pCmd->Len);
			xQueueSend(TxQueueHandle, (const char *) &MsgDayConfing, portMAX_DELAY);
			*Copy_pu8RtcCfgState = DAY_STATE;
			break;

		case DAY_STATE:
			Local_u8Day = u8StrToNum((const char *) Copy_pCmd->Payload, Copy_pCmd->Len);
			xQueueSend(TxQueueHandle, (const char *) &MsgMonConfing, portMAX_DELAY);
			*Copy_pu8RtcCfgState = MON_STATE;
			break;

		case MON_STATE:
			Local_u8Mon = u8StrToNum((const char *) Copy_pCmd->Payload, Copy_pCmd->Len);
			xQueueSend(TxQueueHandle, (const char *) &MsgYearConfing, portMAX_DELAY);
			*Copy_pu8RtcCfgState = YEAR_STATE;
			break;

		case YEAR_STATE:
			Local_u8Year = u8StrToNum((const char *) Copy_pCmd->Payload, Copy_pCmd->Len);

			if (DateTime_u8IsDateCfgValid(Local_u8WeekDay, Local_u8Day, Local_u8Mon, Local_u8Year))
			{
				/* Valid time config */
				DateTime_voidConfigDate(Local_u8WeekDay, Local_u8Day, Local_u8Mon, Local_u8Year);
				xQueueSend(TxQueueHandle, (const char *) &ConfigSuccessMsg, portMAX_DELAY);
				DateTime_voidShowDateTime();
			}
			else /* Invalid command */
			{
				CurrentState = MAIN_MENU_STATE;
				xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
			}

			*Copy_pu8RtcCfgState = WEEK_DAY_STATE;
			CurrentState = MAIN_MENU_STATE;
			break;

		default:
			CurrentState = MAIN_MENU_STATE;
			xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
			break;
		}
	}
	else /* Invalid command */
	{
		CurrentState = MAIN_MENU_STATE;
		xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
	}
}

static void RtcReportingHandler(Command_t *Copy_pCmd)
{
	const char *Local_pcStartMsg = "\r\n\r\n--- Start Reporting ---\r\n";
	const char *Local_pcStoptMsg = "\r\n\r\n--- Stop Reporting ---\r\n";

	if (1U == Copy_pCmd->Len)
	{
		if ('y' == Copy_pCmd->Payload[0U])
		{
			xTimerStart(ReportTimerHandle, portMAX_DELAY);
			xQueueSend(TxQueueHandle, (const void *) &Local_pcStartMsg, portMAX_DELAY);
		}
		else if ('n' == Copy_pCmd->Payload[0U])
		{
			xTimerStop(ReportTimerHandle, portMAX_DELAY);
			xQueueSend(TxQueueHandle, (const void *) &Local_pcStoptMsg, portMAX_DELAY);
		}
		else /* Invalid command */
		{
			xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
		}
	}
	else /* Invalid command */
	{
		xQueueSend(TxQueueHandle, (const void *) &InvalidMsg, portMAX_DELAY);
	}

	CurrentState = MAIN_MENU_STATE;
}

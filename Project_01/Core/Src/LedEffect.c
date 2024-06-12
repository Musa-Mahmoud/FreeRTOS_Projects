/*
 * LedEffect.c
 *
 *  Created on: Apr 27, 2024
 *      Author: Mousa Mahmoud
 */

#include "LedEffect.h"

/* Private Macros */

#define NUM_OF_LEDS			(4U)

#define NUM_OF_TIMERS		(4U)

#define LED1_Pin 			GPIO_PIN_8
#define LED1_GPIO_Port 		GPIOC
#define LED2_Pin 			GPIO_PIN_6
#define LED2_GPIO_Port 		GPIOC
#define LED3_Pin 			GPIO_PIN_5
#define LED3_GPIO_Port 		GPIOC
#define LED4_Pin 			GPIO_PIN_12
#define LED4_GPIO_Port 		GPIOA

/* Private Function Macros */

#define IS_ODD(VAR)			((VAR) & (1U))
#define INVERT_VAL(VAL)		((VAL) = !(VAL))
#define GET_BIT(VAR, BIT)	(((VAR) >> (BIT)) & 1U)

/* External Variables */

extern TimerHandle_t LedTimerHandles[NUM_OF_TIMERS];
extern TimerHandle_t ReportTimerHandle;
/* Private Variables */
static GPIO_TypeDef *LedPorts[NUM_OF_LEDS] = {LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port, LED4_GPIO_Port};
static uint16_t LedPins[NUM_OF_LEDS] = {LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin};

/* Private Fuctions' Prototype */

static void TurnOnEvenLeds(void);
static void TurnOnOddLeds(void);
static void LedControl(uint8_t Copy_u8Num);
static void LedTurnOff(uint8_t Copy_u8LedIdx);

/* Public Functions */

void LedEffectStop(void)
{
	for (uint8_t Local_u8LoopCounter = 0U; Local_u8LoopCounter < NUM_OF_TIMERS; Local_u8LoopCounter++)
	{
		xTimerStop(LedTimerHandles[Local_u8LoopCounter], portMAX_DELAY);
		LedTurnOff(Local_u8LoopCounter);
	}
}

void LedEffectStart(uint8_t Copy_u8EffectNum)
{
	/* Stop all SW timers animation at beginning */
	LedEffectStop();

	xTimerStart(LedTimerHandles[Copy_u8EffectNum - 1U], portMAX_DELAY);
}

void LedEffectAction1(void)
{
	/* Toogle all LEDs */
	for (uint8_t Local_u8LoopCounter = 0U; Local_u8LoopCounter < NUM_OF_LEDS; Local_u8LoopCounter++)
	{
		HAL_GPIO_TogglePin(LedPorts[Local_u8LoopCounter], LedPins[Local_u8LoopCounter]);
	}
}

void LedEffectAction2(void)
{
	/* Even-Odd permutation */
	static uint8_t Local_u8CurState = 0U;

	if (IS_ODD(Local_u8CurState))
	{
		TurnOnOddLeds();
	}
	else
	{
		TurnOnEvenLeds();
	}

	INVERT_VAL(Local_u8CurState);
}

void LedEffectAction3(void)
{
	/* Shift left */
	static uint8_t Local_u8PinNum = 0U;

	LedControl(Local_u8PinNum);

	Local_u8PinNum++;
	/* Limit the Pin number to 3*/
	Local_u8PinNum &= 3U;
}

void LedEffectAction4(void)
{
	/* Shift left */
	static uint8_t Local_u8PinNum = 3U;

	LedControl(Local_u8PinNum);

	Local_u8PinNum--;
	/* Limit the Pin number to 3*/
	Local_u8PinNum &= 3U;
}

/* Private Functions' Impelementation */

static void TurnOnEvenLeds(void)
{
	for (uint8_t Local_u8LoopCounter = 0U; Local_u8LoopCounter < NUM_OF_LEDS; Local_u8LoopCounter++)
	{
		if (IS_ODD(Local_u8LoopCounter))
		{
			HAL_GPIO_WritePin(LedPorts[Local_u8LoopCounter], LedPins[Local_u8LoopCounter], GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(LedPorts[Local_u8LoopCounter], LedPins[Local_u8LoopCounter], GPIO_PIN_SET);
		}
	}
}

static void TurnOnOddLeds(void)
{
	for (uint8_t Local_u8LoopCounter = 0U; Local_u8LoopCounter < NUM_OF_LEDS; Local_u8LoopCounter++)
	{
		if (IS_ODD(Local_u8LoopCounter))
		{
			HAL_GPIO_WritePin(LedPorts[Local_u8LoopCounter], LedPins[Local_u8LoopCounter], GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(LedPorts[Local_u8LoopCounter], LedPins[Local_u8LoopCounter], GPIO_PIN_RESET);
		}
	}
}

static void LedControl(uint8_t Copy_u8Num)
{
	uint8_t Local_u8LedMsk = 1U << Copy_u8Num;
	for (uint8_t Local_u8LoopCounter = 0U; Local_u8LoopCounter < NUM_OF_LEDS; Local_u8LoopCounter++)
	{
		HAL_GPIO_WritePin(LedPorts[Local_u8LoopCounter], LedPins[Local_u8LoopCounter], GET_BIT(Local_u8LedMsk, Local_u8LoopCounter));
	}
}

static void LedTurnOff(uint8_t Copy_u8LedIdx)
{
	HAL_GPIO_WritePin(LedPorts[Copy_u8LedIdx], LedPins[Copy_u8LedIdx], GPIO_PIN_RESET);
}

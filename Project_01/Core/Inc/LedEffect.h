/*
 * LedEffect.h
 *
 *  Created on: Apr 27, 2024
 *      Author: Mousa Mahmoud
 */

#ifndef INC_LEDEFFECT_H_
#define INC_LEDEFFECT_H_

#include <stdint.h>

#include "stm32f4xx_hal.h"

#include "FreeRTOS.h"
#include "timers.h"

void LedEffectStop(void);

void LedEffectStart(uint8_t Copy_u8EffectNum);

void LedEffectAction1(void);

void LedEffectAction2(void);

void LedEffectAction3(void);

void LedEffectAction4(void);

#endif /* INC_LEDEFFECT_H_ */

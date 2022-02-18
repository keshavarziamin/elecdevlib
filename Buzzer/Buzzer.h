/*
 * Buzzer.h
 *
 *  Created on: Sep 14, 2021
 *      Author: Amin-PC
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f1xx.h"
#include "tim.h"


class Buzzer {
public:
	Buzzer(){};
	virtual ~Buzzer(){};

	HAL_StatusTypeDef initialize(TIM_HandleTypeDef *htim,uint32_t htimCannel, uint16_t toggleDelay);
	void setTimeOfToggle(const uint16_t &toggleDelay);

	HAL_StatusTypeDef activateWarning(void);
	HAL_StatusTypeDef activateWarning(const uint32_t toggleDelay);
	HAL_StatusTypeDef deactivateWarning(void);

private:
	TIM_HandleTypeDef *htim;
	uint32_t timChannel;

};

#ifdef __cplusplus
}
#endif

#endif /* BUZZER_H_ */

/*
 * Buzzer.cpp
 *
 *  Created on: Sep 14, 2021
 *      Author: Amin-PC
 */

#include "Buzzer.h"

extern void Error_Handler(void);


HAL_StatusTypeDef Buzzer::initialize(TIM_HandleTypeDef *htim,uint32_t htimCannel, uint16_t toggleDelay){

	if (!htim)
		return HAL_ERROR;
	this->htim=htim;
	this->timChannel=htimCannel;

	setTimeOfToggle(toggleDelay);
	if(deactivateWarning())
		return HAL_ERROR;
	return HAL_OK;
}

void Buzzer::setTimeOfToggle(const uint16_t &toggleDelay){

	auto err=HAL_TIM_PWM_Stop(htim, timChannel);
	if(err)
		Error_Handler();

	auto TimerClock=HAL_RCC_GetPCLK2Freq()*2;
	//IM auto-reload register

	decltype(htim->Instance->PSC) PSC=65000;//TIM prescaler
	htim->Instance->PSC=PSC-1;
	htim->Instance->CNT=0;//TIM counter register,

	TimerClock /= PSC+1;
	TimerClock *= ((float)toggleDelay/1000);
	htim->Instance->ARR=TimerClock;



	//set pulse
	switch (timChannel){
	case TIM_CHANNEL_1:
		htim->Instance->CCR1=TimerClock/2; // %50
		break;
	case TIM_CHANNEL_2:
		htim->Instance->CCR2=TimerClock/2;
		break;
	case TIM_CHANNEL_3:
		htim->Instance->CCR3=TimerClock/2;
		break;
	case TIM_CHANNEL_4:
		htim->Instance->CCR4=TimerClock/2;
		break;
	}


}

HAL_StatusTypeDef Buzzer::activateWarning(void){
	return HAL_TIM_PWM_Start(htim,timChannel);
}

HAL_StatusTypeDef Buzzer::activateWarning(const uint32_t toggleDelay){
	setTimeOfToggle(toggleDelay);
	return HAL_TIM_PWM_Start(htim,timChannel);

}


HAL_StatusTypeDef Buzzer::deactivateWarning(void){
	return HAL_TIM_PWM_Stop(htim, timChannel);
}




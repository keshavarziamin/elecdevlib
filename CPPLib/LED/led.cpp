
#include "led.h"

LED::LED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	port=GPIOx;
	pin=GPIO_Pin;
};

void LED::on(){
	HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET);
}


void LED::off(){
	HAL_GPIO_WritePin(port,pin,GPIO_PIN_RESET);
}

void LED::toggel(){
	HAL_GPIO_TogglePin(port,pin);
}

void LED::toggel(uint32_t const& delay){
	
	if (HAL_GetTick()-Tick_time>delay) return;
		
		
	Tick_time=HAL_GetTick(); //reset timer
	HAL_GPIO_TogglePin(port,pin);
	
}
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif	
	#include "stm32f7xx.h"
	#include "stm32f7xx_hal.h"
	
	class LED{
		private:
			GPIO_TypeDef* port;
			uint16_t pin;
			uint32_t Tick_time;
		public:			
		//function
			LED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
			~LED();		

			void on();
			void off();
			void toggel();
			void toggel(uint32_t const& time);
		
			//
	};
	
	
	
#ifdef __cplusplus
}	
#endif

#endif //__LED_H
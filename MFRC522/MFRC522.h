#ifndef MFRC522_H
#define MFRC522_H

//includes 
#include "stm32f4xx_hal.h"
#include "spi.h"


//select MFRC522 to transmit data using SPI
#define MFRC522_ChipSelect_ON()    HAL_GPIO_WritePin(MF_typedef.CS_port,MF_typedef.CS_Pin,GPIO_PIN_RESET)   
#define MFRC522_ChipSelect_OFF()    HAL_GPIO_WritePin(MF_typedef.CS_port,MF_typedef.CS_Pin,GPIO_PIN_SET)



// MFRC522 Registers
// Page 0: Command and Status
#define MFRC522_REG_RESERVED00				0x00
#define MFRC522_REG_COMMAND						0x01
#define MFRC522_REG_COMM_IE_N					0x02
#define MFRC522_REG_DIV1_EN						0x03
#define MFRC522_REG_COMM_IRQ					0x04
#define MFRC522_REG_DIV_IRQ						0x05
#define MFRC522_REG_ERROR							0x06
#define MFRC522_REG_STATUS1						0x07
#define MFRC522_REG_STATUS2						0x08
#define MFRC522_REG_FIFO_DATA					0x09
#define MFRC522_REG_FIFO_LEVEL				0x0A
#define MFRC522_REG_WATER_LEVEL				0x0B
#define MFRC522_REG_CONTROL						0x0C
#define MFRC522_REG_BIT_FRAMING				0x0D
#define MFRC522_REG_COLL							0x0E
#define MFRC522_REG_RESERVED01				0x0F
// Page 1: Command
#define MFRC522_REG_RESERVED10				0x10
#define MFRC522_REG_MODE							0x11
#define MFRC522_REG_TX_MODE						0x12
#define MFRC522_REG_RX_MODE						0x13
#define MFRC522_REG_TX_CONTROL				0x14
#define MFRC522_REG_TX_AUTO						0x15
#define MFRC522_REG_TX_SELL						0x16
#define MFRC522_REG_RX_SELL						0x17
#define MFRC522_REG_RX_THRESHOLD			0x18
#define MFRC522_REG_DEMOD							0x19
#define MFRC522_REG_RESERVED11				0x1A
#define MFRC522_REG_RESERVED12				0x1B
#define MFRC522_REG_MIFARE						0x1C
#define MFRC522_REG_RESERVED13				0x1D
#define MFRC522_REG_RESERVED14				0x1E
#define MFRC522_REG_SERIALSPEED				0x1F
// Page 2: CFG
#define MFRC522_REG_RESERVED20				0x20
#define MFRC522_REG_CRC_RESULT_M			0x21
#define MFRC522_REG_CRC_RESULT_L			0x22
#define MFRC522_REG_RESERVED21				0x23
#define MFRC522_REG_MOD_WIDTH					0x24
#define MFRC522_REG_RESERVED22				0x25
#define MFRC522_REG_RF_CFG						0x26
#define MFRC522_REG_GS_N							0x27
#define MFRC522_REG_CWGS_PREG					0x28
#define MFRC522_REG__MODGS_PREG				0x29
#define MFRC522_REG_T_MODE						0x2A
#define MFRC522_REG_T_PRESCALER				0x2B
#define MFRC522_REG_T_RELOAD_H				0x2C
#define MFRC522_REG_T_RELOAD_L				0x2D
#define MFRC522_REG_T_COUNTER_VALUE_H	0x2E
#define MFRC522_REG_T_COUNTER_VALUE_L	0x2F
// Page 3:TestRegister
#define MFRC522_REG_RESERVED30				0x30
#define MFRC522_REG_TEST_SEL1					0x31
#define MFRC522_REG_TEST_SEL2					0x32
#define MFRC522_REG_TEST_PIN_EN				0x33
#define MFRC522_REG_TEST_PIN_VALUE		0x34
#define MFRC522_REG_TEST_BUS					0x35
#define MFRC522_REG_AUTO_TEST					0x36
#define MFRC522_REG_VERSION						0x37
#define MFRC522_REG_ANALOG_TEST				0x38
#define MFRC522_REG_TEST_ADC1					0x39
#define MFRC522_REG_TEST_ADC2					0x3A
#define MFRC522_REG_TEST_ADC0					0x3B
#define MFRC522_REG_RESERVED31				0x3C
#define MFRC522_REG_RESERVED32				0x3D
#define MFRC522_REG_RESERVED33				0x3E
#define MFRC522_REG_RESERVED34				0x3F


////////Register descriptions///////

//CommandReg register (address 01h)

#define COMMAND_RcvOff 0x20 	 //analog part of the receiver is switched off
#define COMMAND_RESET 0x20			// reset value: 20h bit allocation

//ComIEnReg register (address 02h);
#define COMIEN_TimerIEN 		0x01 // allows the timer interrupt request (TimerIRq bit) to be propagated to pin IRQ
#define COMIEN_ErrIEN				0x02 //allows the error interrupt request (ErrIRq bit) to be propagated to pin IRQ
#define COMIEN_LoAlertIEn		0x04 //allows the low alert interrupt request (LoAlertIRq bit) to be propagated to pin IRQ
#define COMIEN_HiAlertIEn		0x08 //allows the high alert interrupt request (HiAlertIRq bit) to be propagated to pin IRQ
#define COMIEN_IdleIEn			0x10 //allows the idle interrupt request (IdleIRq bit) to be propagated to pin IRQ
#define COMIEN_RxIEn				0x20 //allows the receiver interrupt request (RxIRq bit) to be propagated to pin IRQ
#define COMIEN_TxIEn				0x40 //allows the transmitter interrupt request (TxIRq bit) to be propagated to pin IRQ
/*
if IRqInv be 1 then signal on pin IRQ is inverted with respect to the Status1Reg register’s
IRq bit
else if IRqInv	be 0 then signal on pin IRQ is equal to the IRq bit; 
in combination with the DivIEnReg register’s IRqPushPull bit, 
the default value of logic 1 ensures that the output level on pin IRQ is 3-state
*/
#define COMIEN_IRqInv				0x80
#define COMIEN_RESET				0x80 // reset value: 80h bit allocation







#define MFRC522_DUMMY									0x00		// Dummy byte
#define MFRC522_MAX_LEN								16			// Buf len byte


typedef struct 
{
	SPI_HandleTypeDef SPI;
	GPIO_TypeDef* CS_port; 
	uint16_t CS_Pin;
}MFRC522_HandleTypeDef;
/*

*/
void MFRC522_init(MFRC522_HandleTypeDef MF_config);
/*
*/
void MFRC522_reset(void);
/*
*/
void MFRC522_setDefault(void);

#endif
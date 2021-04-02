#include "MAX44009.h"



int MAX_init(MAX_HANDLETYPEDEF *MAX){

	uint8_t config[2];
	
	if (MAX->Config.ContinuousMode==EN)
		config[1]|=MAX_REG_CONFIGURATION_CONT;
	else
		config[1]&=~MAX_REG_CONFIGURATION_CONT;
	
	if (MAX->Config.CurrentDivision==EN)
		config[1]|=MAX_REG_CONFIGURATION_CDR;
	else
		config[1]&=~MAX_REG_CONFIGURATION_CDR;
	
	if (MAX->Config.ManualConfiguration==EN)
		config[1]|=MAX_REG_CONFIGURATION_MANUAL;
	else
		config[1]&=~MAX_REG_CONFIGURATION_MANUAL;
	
	config[1]|=MAX->Config.IntegrationTime;
	
	if( MAX_writeRegister(MAX,MAX_REG_ADDR_CONFIGURATION,config[1])!=0)
		return -1;
	
	
	if (MAX->Config.Interrupt==EN)
		config[0]|=MAX_REG_INT_ENABLE;
	else
		config[0]&=~MAX_REG_INT_ENABLE;
	
	
	
	if(MAX_writeRegister(MAX,MAX_REG_ADDR_INTERRUPT_ENABLE,config[0])!=0)
		return -2;
	
	return 0;
}


int MAX_getLUX(MAX_HANDLETYPEDEF *MAX,float *LUX){
	
	uint8_t RxData[2],exp,mantissa;
	if (MAX_readRegister(MAX,MAX_REG_ADDR_LUX_HIGH_BYTE,&RxData[0])!=0)
		return -1;
	if (MAX_readRegister(MAX,MAX_REG_ADDR_LUX_LOW_BYTE,&RxData[1])!=0)
		return -2;
	
	
	exp=(RxData[0]&0xF0)>>4;
	mantissa=((RxData[0]&0x0F)<<4)|(RxData[1]&0x0F);
	
	*LUX=MAX_calculateLUX(exp,mantissa);
	
	return 0;
}


int MAX_enableContinuous(MAX_HANDLETYPEDEF *MAX){
	return MAX_setBit(MAX,MAX_REG_ADDR_CONFIGURATION,MAX_REG_CONFIGURATION_CONT);
}
int MAX_disableContinuous(MAX_HANDLETYPEDEF *MAX){
	return MAX_clearBit(MAX,MAX_REG_ADDR_CONFIGURATION,MAX_REG_CONFIGURATION_CONT);
}
int MAX_enableManual(MAX_HANDLETYPEDEF *MAX){
	return MAX_setBit(MAX,MAX_REG_ADDR_CONFIGURATION,MAX_REG_CONFIGURATION_CONT);
}
int MAX_disableManual(MAX_HANDLETYPEDEF *MAX){
	return MAX_clearBit(MAX,MAX_REG_ADDR_CONFIGURATION,MAX_REG_CONFIGURATION_CONT);
}
int MAX_enableCDR(MAX_HANDLETYPEDEF *MAX){
	return MAX_setBit(MAX,MAX_REG_ADDR_CONFIGURATION,MAX_REG_CONFIGURATION_CDR);
}
int MAX_disableCDR(MAX_HANDLETYPEDEF *MAX){
	return MAX_clearBit(MAX,MAX_REG_ADDR_CONFIGURATION,MAX_REG_CONFIGURATION_CDR);
}

int MAX_setIntegrationTime(MAX_HANDLETYPEDEF *MAX,uint8_t time){
	
	uint8_t RxData=0;
	time&=0x07;
	 if (MAX_readRegister(MAX,MAX_REG_ADDR_CONFIGURATION,&RxData)!=0)
		 return -1;
	 RxData&=0xF8;
	 RxData|=time;
	 return MAX_writeRegister(MAX,MAX_REG_ADDR_CONFIGURATION,RxData);
}

int MAX_enableINT(MAX_HANDLETYPEDEF *MAX){
uint8_t Tx_Data=MAX_REG_INT_ENABLE;
	return MAX_writeRegister(MAX,MAX_REG_ADDR_INTERRUPT_ENABLE,Tx_Data);
}

int MAX_disableINT(MAX_HANDLETYPEDEF *MAX){
uint8_t Tx_Data=0x00;
	return MAX_writeRegister(MAX,MAX_REG_ADDR_INTERRUPT_ENABLE,Tx_Data);
}

int MAX_setUpperThresshod(MAX_HANDLETYPEDEF *MAX,float upperThreshLUX){
	
	uint8_t TxData=MAX_calculateMantissa(upperThreshLUX);
	return MAX_writeRegister(MAX,MAX_REG_ADDR_UPPER_THRESHOLD,TxData);
	
}

int MAX_setLowerThresshod(MAX_HANDLETYPEDEF *MAX,float lowerThreshLUX){
	
	uint8_t TxData=MAX_calculateMantissa(lowerThreshLUX);
	MAX_writeRegister(MAX,MAX_REG_ADDR_LOWER_THRESHOLD,TxData);
	
	return 0;
}

int MAX_setBit(MAX_HANDLETYPEDEF *MAX,uint8_t regAddr,uint8_t bits){
	
	uint8_t recvData;
	if (MAX_readRegister(MAX,regAddr,&recvData)!=0)
		return -1;
	recvData|=bits; //set bit
	return MAX_writeRegister(MAX,regAddr,recvData);
		
}

int MAX_clearBit(MAX_HANDLETYPEDEF *MAX,uint8_t regAddr,uint8_t bits){
	
	uint8_t recvData;
	if (MAX_readRegister(MAX,regAddr,&recvData)!=0)
		return -1;
	recvData&=~bits; //clear bit
	return MAX_writeRegister(MAX,regAddr,recvData);
		
}


int MAX_readRegister(MAX_HANDLETYPEDEF *MAX,uint8_t regAddr,uint8_t *RxData){
	

	uint8_t devAddr=MAX_DEVICE_ADDRESS;
	if (MAX->HW.deviceADDR_Pin==PULL_UP)
		devAddr|=(MAX_DEVICE_ADDR_PULLUP);
	
if (HAL_I2C_Master_Transmit(MAX->HW.hi2c,(uint16_t) devAddr,&regAddr,1,100)!=0)
	return -1;

	return HAL_I2C_Master_Receive(MAX->HW.hi2c,(uint16_t) devAddr,RxData,1,100);
	
}

int MAX_writeRegister(MAX_HANDLETYPEDEF *MAX,uint8_t regAddr,uint8_t TxData){
	
	uint8_t TxBuff[2];
	uint8_t devAddr=MAX_DEVICE_ADDRESS;
	if (MAX->HW.deviceADDR_Pin==PULL_UP)
		devAddr|=(MAX_DEVICE_ADDR_PULLUP);

	//cat reg address and data to send device
	TxBuff[0]=regAddr;
	TxBuff[1]=TxData;
	
	return HAL_I2C_Master_Transmit(MAX->HW.hi2c,(uint16_t) devAddr,TxBuff,2,20);
	
}

uint8_t MAX_calculateMantissa(float value){
	uint32_t exp=0,mantissa=0;
	// lux threshold = 2(exponent) x mantissa x 0.045
	mantissa=(uint32_t)round(value/0.045); //or you can mulitlpy 22.22  
	
	//Data=(2^exp)*mantissa ==> if n=log2(2) then Data=n(2*mantissa)
	while (mantissa>0x0F){
		exp++;
		mantissa=mantissa>>1;
		
	}
	if (mantissa<0x0F) mantissa++;
	exp=exp<<4;
	return (uint8_t)(exp|mantissa);
}

float MAX_calculateLUX(uint8_t exp,uint8_t mantissa){
	return pow(2.0,exp)*mantissa*0.045;
}
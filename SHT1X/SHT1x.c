

#include "SHT1x.h"

#if (DEBUG==1)
	char str[20];
#endif

//static function
static float calculateRelativeHumidity(uint16_t valueOfHumidity,float valueOfTempture);
static float calculateRealTempture(uint16_t value);
static void send_transmitStart(void);
static void Delay(uint32_t microSec);
static void send_data(uint8_t data);
static void read_data(uint8_t *pData,uint8_t length);
static void get_data(uint8_t *receivedData,uint8_t size);
static SHT1x_StatusTypedef	read_ACK(void);
static SHT1x_StatusTypedef	send_ACK (GPIO_PinState PinState);
static void	send_stopBit(void);
static void set_pin_output(uint16_t Pin);
static void set_pin_input(uint16_t Pin);

uint8_t print[2]={0x01,0x1};

uint8_t pri;
uint32_t measurementTime;
uint16_t measurementResolution;


SHT1x_HandelTypedef *SHT_X;
SHT1x_StatusTypedef SHT1x_init(SHT1x_HandelTypedef *PSHT1x)
{
	SHT_X=PSHT1x;
	uint8_t statusReg;
	
	//init gpio as output 
	set_pin_output(SHT_X->GPIO_SCK.Pin);
	set_pin_output(SHT_X->GPIO_SDA.Pin);
	
	//set defualt
	SCKPIN_RESET;
	SDAPIN_SET;
	
	//check config of status register
	if (SHT_X->config.heater==ENABLE)
		SET_BIT(statusReg,STATUS_REGISTER_HEATER);
	else
		CLEAR_BIT(statusReg,STATUS_REGISTER_HEATER);
	
	
	SHT_X->config.Coffi.TCC1=D1_C_35V; //select it beacuse the VDD is 3.3V
	
	if (SHT_X->config.highResolution==ENABLE)
	{
		
		CLEAR_BIT(statusReg,STATUS_REGISTER_RESOLUTION); //active high resolution bit rate
		
		//humidity conversion coefficients
		SHT_X->config.Coffi.C2=C2_HIGH_V3;
		SHT_X->config.Coffi.C2=C2_HIGH_V3;
		SHT_X->config.Coffi.C2=C2_HIGH_V3;
		
		//Temperature conversion coefficients
		SHT_X->config.Coffi.T1=T1_HIGH_RESOLUTION;
		SHT_X->config.Coffi.T2=T2_HIGH_RESOLUTION;
		SHT_X->config.Coffi.TCC2=D2_C_HIGH_RESOLUTION;
	}
	else
	{
		SET_BIT(statusReg,STATUS_REGISTER_RESOLUTION); //deactive high resolution bit rate
		
		//humidity conversion coefficients
		SHT_X->config.Coffi.C2=C2_LOW_V3;
		SHT_X->config.Coffi.C2=C2_LOW_V3;
		SHT_X->config.Coffi.C2=C2_LOW_V3;
		
		//Temperature conversion coefficients
		SHT_X->config.Coffi.T1=T1_LOW_RESOLUTION;
		SHT_X->config.Coffi.T2=T2_LOW_RESOLUTION;
		SHT_X->config.Coffi.TCC2=D2_C_LOW_RESOLUTION;
	}
	
	
	
	if(SHT_X->config.reloadOTP==ENABLE)
		CLEAR_BIT(statusReg,STATUS_REGISTER_OTP);
	else
		SET_BIT(statusReg,STATUS_REGISTER_OTP);
//	#if(DEBUG==1)
//	 HAL_UART_Transmit(&huart3,print,2,100);
//		HAL_UART_Transmit(&huart3,&statusReg,1,100);
//	HAL_UART_Transmit(&huart3,print,2,100);
//		
//	#endif
	//write statrus register
	SHT1x_writeStatusReg(statusReg);

	return SHT1x_OK;
	
}

SHT1x_StatusTypedef SHT1x_writeStatusReg(uint8_t write_Data)
{
	uint8_t read_data;
	
	if (SHT1x_readStatusReg(&read_data)!=SHT1x_OK)
		printf("can't read data from SHT\r\n");
	//	return SHT1x_ERROR;
//	
//	//change the bits of register
//	read_data&=~write_Data;
//	read_data|=write_Data;
	
	if (SHT1x_WriteRegister(SHT_REGISTER_STATUS,&write_Data)!=SHT1x_OK)
		return SHT1x_ERROR;
	
	SHT_X->reg.Status=read_data;
	return SHT1x_OK;
}

SHT1x_StatusTypedef SHT1x_readStatusReg(uint8_t *pData)
{
	SHT1x_StatusTypedef ST;
	ST=SHT1x_readRegister((uint8_t)SHT_REGISTER_STATUS,pData,1);
	SHT_X->reg.Status=*pData;
	return ST;
}


SHT1x_StatusTypedef SHT1x_GetTempture(float *valueOfTempture)
{
	//read value of tempture
	uint8_t valueOfTempture_int[2]={0};
	uint16_t valueOfTempture_RAW;
	if (SHT1x_readRegister(SHT_REGISTER_TEMPETURE,valueOfTempture_int,2)!=SHT1x_OK)
		return SHT1x_ERROR;
	
	
	
	//calculate value of temputure
	valueOfTempture_RAW=((valueOfTempture_int[0]&0x3F)<<8)|valueOfTempture_int[1];
	//printf("valueOfTempture_RAW is %d\r\n",valueOfTempture_RAW);
	*valueOfTempture=calculateRealTempture(valueOfTempture_RAW);
	SHT_X->reg.Temp=*valueOfTempture;
	return SHT1x_OK;
	
}

SHT1x_StatusTypedef SHT1x_GetTemptureHumidity(float *valueOfHumidity,float *valueOfTempture)
{
	
	//read value of tempture
	SHT1x_GetTempture(valueOfTempture);
	
	
	HAL_Delay(100);//
	
	//read value of Humidity
	uint8_t valueOfHumidity_int[2]={0};
	uint16_t ValueOfHumidity_RAW;
	if (SHT1x_readRegister(SHT_REGISTER_HUMIDITY,valueOfHumidity_int,2)!=SHT1x_OK)
		return SHT1x_ERROR;
	
	
	
	//calculate value of humidity
	ValueOfHumidity_RAW=((valueOfHumidity_int[0]&0x0F)<<8)|valueOfHumidity_int[1];
	*valueOfHumidity=calculateRelativeHumidity(ValueOfHumidity_RAW,*valueOfTempture);
	SHT_X->reg.humidity=*valueOfHumidity;
	
//	#if(DEBUG==1)
//		char str[50];
//		uint8_t T=s//printf(str,"value_Humidity=%d,f=%f\r\n",ValueOfHumidity_RAW,*valueOfHumidity);
//		HAL_UART_Transmit(&huart3,(uint8_t*)str,T,100);
//	#endif
	
	return SHT1x_OK;
	
}

SHT1x_StatusTypedef SHT1x_WriteRegister(uint8_t regAddr,uint8_t *pData)
{
	//set addres of device 
	SET_DEVICE_ADDRESS(regAddr);
	//enable read mode
	SET_WRITE_MODE(regAddr);
	
	set_pin_output(SHT_X->GPIO_SDA.Pin);
	set_pin_output(SHT_X->GPIO_SCK.Pin);
	//write data
	
	send_transmitStart(); 
	send_data(regAddr);
	read_ACK();	
	send_data(*pData);
	read_ACK();

	
	return SHT1x_OK;
}

SHT1x_StatusTypedef SHT1x_readRegister(uint8_t regAddr,uint8_t *pData, uint8_t length)
{
	
	//set addres of device 
	SET_DEVICE_ADDRESS(regAddr);
	//enable read mode
	SET_READ_MODE(regAddr);

	set_pin_output(SHT_X->GPIO_SDA.Pin);
	set_pin_output(SHT_X->GPIO_SCK.Pin);
	SCKPIN_RESET_DELAY;
	
	
	//send  command 
	send_transmitStart();
	send_data(regAddr);
	read_ACK();
	
	//Delay(TIME_MEASUREMENT);  //wait for measurement
	HAL_Delay(300);
	read_data(pData,length);
	
	return SHT1x_OK;
	
}
static float calculateRelativeHumidity(uint16_t valueOfHumidity,float valueOfTempture)
{
	float fVH_linear,RH;
	
	/*
	Conversion from SO(RH) to relative humidity
				%RH=C1+(C2.value)+(C3.value^2)
	*/
	fVH_linear=SHT_X->config.Coffi.C3 *((float)valueOfHumidity*(float)valueOfHumidity);
	fVH_linear=(SHT_X->config.Coffi.C2 *(float) valueOfHumidity)+fVH_linear; 
	fVH_linear=SHT_X->config.Coffi.C1+fVH_linear;
	
	/*
	Temperature compensation of Humidity Signal
		relative humidity true(RHt)=(T(c)-25(c)).(T1+T2*valueOfHumidity)+FVH_linear
	*/
	
	valueOfTempture-=25.0;
	RH=valueOfTempture*(SHT_X->config.Coffi.T1+(SHT_X->config.Coffi.T2*valueOfHumidity));
	RH+=fVH_linear;
	
	return RH;
}
static float calculateRealTempture(uint16_t valueofTempture)
{
	
	return SHT_X->config.Coffi.TCC1+(SHT_X->config.Coffi.TCC2*(float)valueofTempture);
}
static void read_data(uint8_t *pData,uint8_t length)
{
	
	uint8_t lengthOfBuffer=length+1; //length of data and checksum
	
	uint8_t *buffer=malloc(lengthOfBuffer*sizeof(uint8_t));
	
	set_pin_input(SHT_X->GPIO_SDA.Pin); //change config to input mode
	
	get_data(buffer,lengthOfBuffer);  //get value of register and checksum
	
	
	//check CRC
	
	//PRINT(buffer,lengthOfBuffer);
	memcpy(pData,buffer,length);  //copy data from buffer to return
	
	/* clear and free buffer*/
	memset(buffer,0,lengthOfBuffer); //clear buffer;
	free(buffer); 
	
	set_pin_output(SHT_X->GPIO_SDA.Pin); //change config to output mode
}
static void get_data(uint8_t *receivedData,uint8_t size)
{
	uint8_t reData=0;
	/*
	get data from sensor
	received data must be contained value of register and checksum
	*/
	for(uint8_t k=0;k<size;k++)
	{
		
		for(uint8_t m=0;m<8;m++)
		{
			SCKPIN_SET_DELAY;     //start clock
			reData|=SDAPIN_READ&0x01; //read bit
			//shift data to left
			if (m<7)
				reData=reData<<1;
			SCKPIN_RESET_DELAY;  //stop clock
		}
		receivedData[k]=reData;
		
		
		if (k<size-1)
			send_ACK(GPIO_PIN_RESET); // send ACK 0 for Data  
		else
			send_ACK(GPIO_PIN_SET); // send ACK 1 for checksum  
	}
}
static void send_data(uint8_t data)
{
	
	uint8_t reversedData=0;
	GPIO_PinState stateOfPin;
	
	SCKPIN_RESET; //set to defualt
	
	//printf("send data to SHT10 ====>%02x\r\n",data);
	 //reverse data to send MSB first
	for(uint8_t k=0;k<8;k++)
	{
		reversedData|=(data<<k)&0x80;
		if(k<7) 
			reversedData=reversedData>>1;
	}
	
	pri=reversedData;
	//send data with clk
	for(uint8_t k=0;k<8;k++)
	{
		stateOfPin=reversedData&0x01; //check set or reset
		HAL_GPIO_WritePin(SHT_X->GPIO_SDA.Port,SHT_X->GPIO_SDA.Pin,stateOfPin);
		reversedData=reversedData>>1;
		SCKPIN_SET_DELAY;
		SCKPIN_RESET_DELAY;
	}
	
	SDAPIN_SET; //set to defualt

}







static void send_transmitStart(void)
{
	
	
	/*
	To initiate a transmission, a Transmission Start sequence
	has to be issued. It consists of a lowering of the DATA line
	while SCK is high, followed by a low pulse on SCK and
	raising DATA again while SCK is still high
		
	*/
	
	SCKPIN_RESET;//set pin to 0 to sure
	SDAPIN_SET;	 //set pin to 1 to sure	 
	
	//SCK is high and low DATA line	while SCK is high
	SCKPIN_SET_DELAY; 
	SDAPIN_RESET;				
	SCKPIN_RESET_DELAY;
	
	//raising DATA again while SCK is still high
	SCKPIN_SET_DELAY;
	SDAPIN_SET;
	SCKPIN_RESET_DELAY;
}
static SHT1x_StatusTypedef	send_ACK (GPIO_PinState PinState)
{
	
	if (PinState!=GPIO_PIN_RESET && PinState!=GPIO_PIN_SET)
		return SHT1x_ERROR;
	
	set_pin_output(SHT_X->GPIO_SDA.Pin); //change config to output mode
	

	//set pin to 0 or 1 with clock
	HAL_GPIO_WritePin(SHT_X->GPIO_SDA.Port,SHT_X->GPIO_SDA.Pin,PinState);
	SCKPIN_SET_DELAY;
	SCKPIN_RESET_DELAY;
		
	set_pin_input(SHT_X->GPIO_SDA.Pin); //change config to input mode
	
	return SHT1x_OK;
}
static SHT1x_StatusTypedef read_ACK(void)
{
	SHT1x_StatusTypedef statusOfPin;
	
	set_pin_input(SHT_X->GPIO_SDA.Pin); //change config to input mode
	

	SCKPIN_SET_DELAY;
	
	
	if (SDAPIN_READ==0)
		 statusOfPin= SHT1x_OK;
	else
		 statusOfPin=SHT1x_ERROR;
	
	SCKPIN_RESET_DELAY;
	
	set_pin_output(SHT_X->GPIO_SDA.Pin); //change config to output mode
	return statusOfPin;
		
}
static void set_pin_input(uint16_t Pin)
{
	 GPIO_InitTypeDef GPIO_InitStruct = {0};


	/*Configure GPIO pins : PHPin PHPin */
	GPIO_InitStruct.Pin = Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SHT_X->GPIO_SDA.Port, &GPIO_InitStruct);
	
}	

static void set_pin_output(uint16_t Pin)
{
	 GPIO_InitTypeDef GPIO_InitStruct = {0};


	/*Configure GPIO pins : PHPin PHPin */
	GPIO_InitStruct.Pin = Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SHT_X->GPIO_SDA.Port, &GPIO_InitStruct);
	
}	
static void Delay(uint32_t microSec)
{
	uint32_t time=1*microSec;
	for(uint32_t k=0;k<=time;k++);
}
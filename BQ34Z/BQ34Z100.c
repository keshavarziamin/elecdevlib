#include "BQ34Z100.h"


int BQ_init(BQ_HandleTypedef *pBQ)
{
	uint8_t CMD,data[2];
	uint16_t RXD;
	HAL_Delay(500);
	uint8_t ret;
	printf("!!!!!!!!!!!!!!!!!!START!!!!!!!!!!!!!!!!!\r\n");


	//reset device
	//BQ_writeCTRL(pBQ,BQ_CTRL_SUB_RESET_DATA);

	HAL_Delay(1000);


	/*step1.Review and Modify the Data Flash Configuration Data.*/ 
	//check address 
//	__HAL_RCC_I2C3_FORCE_RESET();
//	HAL_Delay(4);
//	__HAL_RCC_I2C3_RELEASE_RESET();
//	HAL_Delay(100);
//	HAL_I2C_Init(&hi2c3);
//	for(uint8_t i=0;i<=255;i++){
//		HAL_Delay(100);
//		printf("the address send %x\r\n",i);
//		ret=HAL_I2C_IsDeviceReady(&hi2c3,i,4,100);
//		printf("return is %x\r\n",ret);
//		if(ret==HAL_OK){
//			printf("the address is %x\r\n",i);
//		}

//	}
	
		
	BQ_readCTRL(pBQ,BQ_CTRL_SUB_CONTROL_STATUS,&RXD);
	printf ("CTRL_STATUS:%x\r\n",RXD);
	if(BQ_updateDesignCapacity(pBQ)!=0)
		return -1;
	printf("!!!!!!!!!!!!!!!!!!update design capacity!!!!!!!!!!!!!!!!!!\r\n");
	//while(1){};
	HAL_Delay(200);
	printf ("CTRL_STATUS:%x\r\n",RXD);
	if (BQ_updateDesignEnergy(pBQ)!=0)
		return -2;
	printf("!!!!!!!!!!!!!!!!!!update design energy!!!!!!!!!!!!!!!!!!\r\n");
	HAL_Delay(200);
	if (BQ_setScale(pBQ)!=0)
		return -3;
	printf("!!!!!!!!!!!!!!!!!!update scale !!!!!!!!!!!!!!!!!!\r\n");
	
	HAL_Delay(200);
	if(BQ_setVoltageDivider(pBQ)!=0)
		return -4;
	printf("!!!!!!!!!!!!!!!!!!set voltage divider  !!!!!!!!!!!!!!!!!!\r\n");
	/*step2.Review and Modify the Data Flash Configuration Registers*/
	
	HAL_Delay(200);
	uint8_t bits=(BQ_BITS_VOLTSEL|BQ_BITS_SCALE);
	if(BQ_setBitFlashBlock(pBQ,BQ_SUBCLASS_ID_CONFIGURATION_REGISTER,BQ_OFFSET_PACKCONFIGURATION,BQ_BITS_VOLTSEL)!=0)
		return -5;
	printf("!!!!!!!!!!!!!!!!!!set scale & voltsel!!!!!!!!!!!!!!!!!!\r\n");
	HAL_Delay(200);
	
//	uint8_t bits=(BQ_BITS_CAL_EN);
//	if(BQ_setBitFlashBlock(pBQ,BQ_SUBCLASS_ID_CONFIGURATION_REGISTER,BQ_OFFSET_PACKCONFIGURATION,bits)!=0)
//		return -5;
//	printf("!!!!!!!!!!!!!!!!!!set scale & voltsel!!!!!!!!!!!!!!!!!!\r\n");
//	HAL_Delay(200);
	
	
	
//	HAL_Delay(200);
//	/*step3.Design and Configure the Voltage Divider.*/
//	if (BQ_setVoltageDivider(pBQ)!=0)
//		return -1;
//	printf("!!!!!!!!!!!!!!!!!!setVoltageDivider!!!!!!!!!!!!!!!!!!\r\n");
	

}

int BQ_setScale(BQ_HandleTypedef *pBQ)
{
	uint8_t subClass=BQ_SUBCLASS_ID_CONFIGURATION_DATA;

	if ( BQ_writeFlashBlock(pBQ,subClass,BQ_OFFSET_DESIGNENERGYSCALE,&pBQ->Battery.scale,1)!=0)
		return -1;
	return 0;
}
int BQ_setNumberOfCell(BQ_HandleTypedef *pBQ,uint8_t number)
{
	uint8_t subClass=BQ_SUBCLASS_ID_CONFIGURATION_REGISTER;

	if ( BQ_writeFlashBlock(pBQ,subClass,BQ_OFFSET_NUMBEROFCELL,&number,1)!=0)
		return -1;
	return 0;
}
int BQ_setVoltageDivider(BQ_HandleTypedef *pBQ)
{
	uint8_t subClass=BQ_SUBCLASS_ID_CALIBRARTION_DATA;
	uint8_t data[2];
	data[1]=(uint8_t)(pBQ->Battery.voltageDevider&0x00FF);
	data[0]=(uint8_t)((pBQ->Battery.voltageDevider&0xFF00)>>8);
	if ( BQ_writeFlashBlock(pBQ,subClass,BQ_OFFSET_VOLTAGEDIVIDER,data,2)!=0)
		return -1;
	return 0;
}

/*
Enter the desired cell charge voltage for each JEITA temperature range.
*/



int BQ_updateCellChargeVolatge(BQ_HandleTypedef *pBQ)
{
	uint8_t subClass,data[6];
	data[1]=(uint8_t)(pBQ->Battery.cellChargeVoltage&0x00FF);
	data[0]=(uint8_t)((pBQ->Battery.cellChargeVoltage&0xFF00)>>8);
	data[2]=data[0];
	data[3]=data[1];
	data[4]=data[0];
	data[5]=data[1];
	subClass=BQ_SUBCLASS_ID_CONFIGURATION_DATA;
	if (BQ_writeFlashBlock(pBQ,subClass,1,data,6)!=0) 
		return -1;
		
	return 0;
}
/*
	weite design energy
	Enter the value in mWh.
	*/
int BQ_updateDesignCapacity(BQ_HandleTypedef *pBQ)
{
	uint8_t subClass=BQ_SUBCLASS_ID_CONFIGURATION_DATA;
	uint8_t data[2];
	data[1]=(uint8_t)(pBQ->Battery.designCapacity&0x00FF);
	data[0]=(uint8_t)((pBQ->Battery.designCapacity&0xFF00)>>8);
	int ret=BQ_writeFlashBlock(pBQ,subClass,BQ_OFFSET_DESIGNCAPACITY,data,2);
	printf("writeFlashBlock's ret is %d \r\n",ret);
	if ( ret!=0)
		return -1;
	return 0;
}
/*
	write design capacity
	Enter the value in mAh for the battery, even from the “design energy” point of view
	*/
int BQ_updateDesignEnergy(BQ_HandleTypedef *pBQ)
{
	uint8_t subClass=BQ_SUBCLASS_ID_CONFIGURATION_DATA;
	uint8_t data[2];
	data[1]=(uint8_t)(pBQ->Battery.designEnergy&0x00FF);
	data[0]=(uint8_t)((pBQ->Battery.designEnergy&0xFF00)>>8);
	if ( BQ_writeFlashBlock(pBQ,subClass,1,data,2)!=0)
		return -1;
	return 0;

}
int BQ_clearBitFlashBlock(BQ_HandleTypedef *pBQ,uint8_t subClass,uint8_t offset,uint8_t Data)
{
	/***********************************
	***********NOTE********************
	(please read datasheet page 21)
	*********************************
	**********************************/
	uint8_t CMD,selOffset,modOffset,addr,checkSum_old,checkSum_new;
	
	//select unseal mode
	if(BQ_setUNSEAL(pBQ)!=0)
		return -1;
	
	//Write sub class using the DataFlashClass() command (0x3E) to access the Registers subclass.
	if (BQ_accessSubClass(pBQ,subClass)!=0)
		return -2;
	
	
	//this command directs which data flash block will be accessed by the BlockData() command.
	CMD=BQ_CMD_EXT_BLOCKDATACONTROL;
	uint8_t value=0x00;
	if(BQ_writeRegister(pBQ,&CMD,1,&value,1)!=0)
		return -3;
	
	//select offset
	selOffset=offset/32;
	if (BQ_selectOffset(pBQ,selOffset)!=0) //select offset 0 or 1
		return -4;
	
	//read block data to calculate new checksum
	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA;
	if (BQ_readRegister(pBQ,&addr,1,pBQ->reg.blockData,32)!=0)
		return -6;
	
	modOffset=offset%32;	
	pBQ->reg.blockData[modOffset]&=~Data;//rewrite data
	
	//calculate new check sum & write to memory
	checkSum_new=BQ_calculateChecksum(pBQ);
	
	// write data&checksum to BQ34z100
	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA+modOffset;
	if (BQ_writeRegister(pBQ,&addr,1,&pBQ->reg.blockData[modOffset],1)!=0)
		return -7;
	if (BQ_writeCheckSum(pBQ,checkSum_new)!=0)
		return -8;
	//HAL_Delay(200);
	//Reset the gauge to ensure the new data flash parameter goes into effect
	if (BQ_reset(pBQ)!=0)
		return -9;
	HAL_Delay(200);
	
	//select seal mode
//	if(BQ_setSEAL(pBQ)!=0)
//		return -10;
	
	uint8_t RXD[32];
	BQ_readFlashBlock(pBQ,subClass,0,RXD,32);
	
	return 0;
}

int BQ_setBitFlashBlock(BQ_HandleTypedef *pBQ,uint8_t subClass,uint8_t offset,uint8_t Data)
{
	/***********************************
	***********NOTE********************
	(please read datasheet page 21)
	*********************************
	**********************************/
	uint8_t CMD,selOffset,modOffset,addr,checkSum_old,checkSum_new;
	
	//select unseal mode
	if(BQ_setUNSEAL(pBQ)!=0)
		return -1;
	
	//Write sub class using the DataFlashClass() command (0x3E) to access the Registers subclass.
	if (BQ_accessSubClass(pBQ,subClass)!=0)
		return -2;
	
	
	//this command directs which data flash block will be accessed by the BlockData() command.
	CMD=BQ_CMD_EXT_BLOCKDATACONTROL;
	uint8_t value=0x00;
	if(BQ_writeRegister(pBQ,&CMD,1,&value,1)!=0)
		return -3;
	
	//select offset
	selOffset=offset/32;
	if (BQ_selectOffset(pBQ,selOffset)!=0) //select offset 0 or 1
		return -4;
	
	//read block data to calculate new checksum
	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA;
	if (BQ_readRegister(pBQ,&addr,1,pBQ->reg.blockData,32)!=0)
		return -6;
	
	modOffset=offset%32;	
	pBQ->reg.blockData[modOffset]|=Data;//rewrite data
	
	//calculate new check sum & write to memory
	checkSum_new=BQ_calculateChecksum(pBQ);
	HAL_Delay(200);
	// write data&checksum to BQ34z100
//	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA+modOffset;
//	if (BQ_writeRegister(pBQ,&addr,1,&pBQ->reg.blockData[modOffset],1)!=0)
//		return -7;
	
	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA;
	if (BQ_writeRegister(pBQ,&addr,1,pBQ->reg.blockData,32)!=0)
		return -6;
	
	if (BQ_writeCheckSum(pBQ,checkSum_new)!=0)
		return -8;
	HAL_Delay(200);
	//Reset the gauge to ensure the new data flash parameter goes into effect
	if (BQ_reset(pBQ)!=0)
		return -9;
	HAL_Delay(200);
	
	//select seal mode
//	if(BQ_setSEAL(pBQ)!=0)
//		return -10;
	
	//HAL_Delay(1000);
	//read block data to calculate new checksum
	uint8_t RXD[32];	
	BQ_readFlashBlock(pBQ,subClass,0,RXD,32);
	
	return 0;
}

/*
See datasheet (Table 20 and Table 21) to aid in selection of an LED mode. Note that the
pin used for the optional Alert signal is dependent upon the LED mode selected.
*/

int BQ_writeFlashBlock(BQ_HandleTypedef *pBQ,uint8_t subClass,uint8_t offset,uint8_t *pData,uint8_t lengthOfData)
{
	
	/***********************************
	***********NOTE********************
	(please read datasheet page 21)
	*********************************
	**********************************/
	uint8_t CMD,selOffset,modOffset,addr,checkSum_old,checkSum_new;
	
	//select unseal mode
	if(BQ_setUNSEAL(pBQ)!=0)
		return -1;
	HAL_Delay(500);
	printf("select unseal mode\r\n");
	//Write sub class using the DataFlashClass() command (0x3E) to access the Registers subclass.
	if (BQ_accessSubClass(pBQ,subClass)!=0)
		return -2;
	
	
	//this command directs which data flash block will be accessed by the BlockData() command.
	CMD=BQ_CMD_EXT_BLOCKDATACONTROL;
	uint8_t value=0x00;
	if(BQ_writeRegister(pBQ,&CMD,1,&value,1)!=0)
		return -3;

	//select offset
	selOffset=offset/32;
	if (BQ_selectOffset(pBQ,selOffset)!=0) //select offset 0 or 1
		return -4;

	//memset(pBQ->reg.blockData,0,32);
	//read block data to calculate new checksum
	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA;
	if (BQ_readRegister(pBQ,&addr,1,pBQ->reg.blockData,32)!=0)
		return -6;
	
	modOffset=offset%32;
printf("modOffset is %d ans offset is %d\r\n",modOffset,offset);	
	memcpy(&pBQ->reg.blockData[modOffset],pData,lengthOfData);//rewrite data
	
	//calculate new check sum & write to memory
	checkSum_new=BQ_calculateChecksum(pBQ);
		HAL_Delay(200);
	// write data&checksum to BQ34z100
//	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA+modOffset;
//	if (BQ_writeRegister(pBQ,&addr,1,pData,lengthOfData)!=0)
//		return -7;
	
	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA;
	if (BQ_writeRegister(pBQ,&addr,1,pBQ->reg.blockData,32)!=0)
		return -6;
	
	printf("********AAAA\r\n");
	HAL_Delay(200);
	if (BQ_writeCheckSum(pBQ,checkSum_new)!=0)
		return -8;
	HAL_Delay(1000);
	printf("********AAAA\r\n");
	//Reset the gauge to ensure the new data flash parameter goes into effect
	if (BQ_reset(pBQ)!=0)
		return -9;
	HAL_Delay(1000);
	printf("********AAAA\r\n");
	//select seal mode
//	if(BQ_setSEAL(pBQ)!=0)
//		return -10;
	//HAL_Delay(1);
	
	uint8_t RXD[32];
 int ret=BQ_readFlashBlock(pBQ,subClass,0,RXD,32);
	printf("ret is %d\r\n",ret);
	
	return 0;
	
}
int BQ_readFlashBlock(BQ_HandleTypedef *pBQ,uint8_t subClass,uint8_t offset,uint8_t *pData,uint8_t lengthOfData)
{
	/***********************************
	***********NOTE********************
	(please read datasheet page 21)
	*********************************
	**********************************/
	uint8_t CMD,selOffset,modOffset,addr,checkSum_old,checkSum_new;
	//select unseal mode
	if(BQ_setUNSEAL(pBQ)!=HAL_OK)
		return -1;

	
	//Write sub class using the DataFlashClass() command (0x3E) to access the Registers subclass.
	if (BQ_accessSubClass(pBQ,subClass)!=0)
		return -2;
	
	
	//this command directs which data flash block will be accessed by the BlockData() command.
	CMD=BQ_CMD_EXT_BLOCKDATACONTROL;
	uint8_t value=0x00;
	if(BQ_writeRegister(pBQ,&CMD,1,&value,1)!=0)
		return -3;
	
	
	
	//select offset
	selOffset=offset/32;
	if (BQ_selectOffset(pBQ,selOffset)!=0) //select offset 0 or 1
		return -4;
	
	
	// read data from data block
	modOffset=offset%32;
	addr=BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA+modOffset;
	if (BQ_readRegister(pBQ,&addr,1,pData,lengthOfData)!=0)
		return -5;
	
	//select seal mode
	//if(BQ_setSEAL(pBQ)!=0)
	//	return -6;
	
	return 0;
}

int BQ_setSEAL(BQ_HandleTypedef *pBQ)
{
	//Write the first 2 bytes of the SEAL key using the Control(0x0020) command.
	return BQ_writeCTRL(pBQ,(uint16_t)(BQ_CTRL_SUB_SEALED));
		
}
int BQ_setUNSEAL(BQ_HandleTypedef *pBQ)
{
	//Write the first 2 bytes of the UNSEAL key using the Control(0x0414) command.
	if (BQ_writeCTRL(pBQ,(uint16_t)(BQ_CTRL_SUB_UNSEALED))!=0) 
		return -1;
	//Write the second 2 bytes of the UNSEAL key using the Control(0x3672) command
	if (BQ_writeCTRL(pBQ,(uint16_t)(BQ_CTRL_SUB_UNSEALED>>16))!=0) 
		return -2;
	
	return 0;
}
int BQ_selectOffset(BQ_HandleTypedef *pBQ,uint8_t offset)
{
	uint8_t CMD;
	CMD=BQ_CMD_EXT_DATAFLASHBLOCK;
	offset&=0x01; //0x00 offset 0 & 0x01 offset 1
	
	if (BQ_writeRegister(pBQ,&CMD,1,&offset,1)!=0)
		return -1;
	return 0;
	
}

int BQ_reset(BQ_HandleTypedef *pBQ)
{
	//Reset the gauge to ensure the new data flash parameter goes into effect
	return BQ_writeCTRL(pBQ,(uint16_t)BQ_CTRL_SUB_RESET);
}




/*
The data is actually transferred to the data flash when the correct checksum for the whole block (0x40 to
0x5F) is written to BlockDataChecksum() (0x60).
*/
int BQ_writeCheckSum(BQ_HandleTypedef *pBQ,uint8_t checkSum)
{
	uint8_t CMD=BQ_CMD_EXT_BLOCKDATACHECKSUM;
	if (BQ_writeRegister(pBQ,&CMD,1,&checkSum,1)!=0)
		return -1;
	return 0; 

}


uint8_t BQ_calculateChecksum(BQ_HandleTypedef *pBQ)
{
	uint32_t CS=0;
	for(uint8_t k=0;k<32;k++)
		CS+=(uint32_t)pBQ->reg.blockData[k];
	printf("check sum: %x\r\n",(uint8_t)(255-CS));
	return (uint8_t)(255-CS);

}

//To read the 1-byte checksum, use the BlockDataChecksum() command (0x60).
int BQ_readCheckSum(BQ_HandleTypedef *pBQ,uint8_t *checkSum)
{
	uint8_t CMD=BQ_CMD_EXT_BLOCKDATACHECKSUM;
	if (BQ_readRegister(pBQ,&CMD,1,checkSum,1)!=0)
		return -1;
	return 0;
}

int BQ_accessSubClass(BQ_HandleTypedef *pBQ,uint8_t subClass)
{
	uint8_t CMD;
	CMD=BQ_CMD_EXT_DATAFLASHCLASS;
	if (BQ_writeRegister(pBQ,&CMD,1,&subClass,1)!=0)
		return -1;
	return 0;
	
}
int BQ_writeCTRL(BQ_HandleTypedef *pBQ,uint16_t subCMD)
{
	uint8_t CMD=(uint8_t)(BQ_CMD_CONTROL>>8); //0x00
	uint8_t sCMD[2];//sub command
	sCMD[0]=(uint8_t)(subCMD&0x00FF);
	sCMD[1]=(uint8_t)((subCMD&0xFF00)>>8);
	uint8_t recvData[2];
	
	if (BQ_writeRegister(pBQ,&CMD,1,sCMD,2)!=0)//write sub command on control command	
		return -1;
	return 0;
}

int BQ_readCTRL(BQ_HandleTypedef *pBQ,uint16_t subCMD,uint16_t *RxData)
{
	uint8_t CMD=(uint8_t)(BQ_CMD_CONTROL>>8); //0x00
	uint8_t sCMD[2];//sub command
	sCMD[0]=(uint8_t)(subCMD&0x00FF);
	sCMD[1]=(uint8_t)((subCMD&0xFF00)>>8);
	uint8_t recvData[2];
	
	if (BQ_writeRegister(pBQ,&CMD,1,sCMD,2)!=0) return -1;//write sub command on control command	
	if (BQ_readRegister(pBQ,&CMD,1,recvData,2)!=0) return -2;
	
	*RxData=((recvData[1]<<8)&0xFF00)|recvData[0];
	return 0;
}
int BQ_getDesignCapacity(BQ_HandleTypedef *pBQ,uint16_t *designCapacity)
{
	uint8_t CMD[2];
	CMD[0]=(uint8_t )((BQ_CMD_EXT_DESIGNCAPACITY&0xFF00)>>8);
	CMD[1]=(uint8_t)(BQ_CMD_EXT_DESIGNCAPACITY&0x00FF);
	
	uint8_t dc[2];
	if (BQ_readRegister(pBQ,&CMD[0],1,dc,2)!=0)
		return -1;
	
	*designCapacity=(uint16_t)((dc[1]<<8)|dc[0]);
	return 0;
}
int BQ_getVlotage(BQ_HandleTypedef *pBQ)
{
	uint8_t CMD[2];int ret;
	CMD[0]=(uint8_t )((BQ_CMD_VOLTAGE&0xFF00)>>8);
	CMD[1]=(uint8_t)(BQ_CMD_VOLTAGE&0x00FF);
	
	uint8_t volatge[2]={0};
	ret=BQ_readRegister(pBQ,&CMD[0],1,volatge,2);
	printf("read voltage state is %d\r\n",ret);
	if (ret!=0)
		return -1;
	
	pBQ->Battery.voltage=(uint16_t)((volatge[1]<<8)|volatge[0]);
	return 0;
}
int BQ_getStateOfCharge(BQ_HandleTypedef *pBQ)
{
	uint8_t CMD[2];
	CMD[0]=(uint8_t )BQ_CMD_STATEOFCHARGE;	
	uint8_t volatge[2];
	if (BQ_readRegister(pBQ,&CMD[0],1,&pBQ->Battery.stateOfCharge,1)!=0)
		return -1;
	
	return 0;
}

int BQ_calibrateVoltageDivider(BQ_HandleTypedef *pBQ,uint16_t appliedVoltage)
{
	printf("*****************start calibration ******************\r\n");
	uint16_t vol[50];
	float vol_mean=0,vol_std;
	for (uint8_t k=0;k<50;k++)
	{
		BQ_getVlotage(pBQ);
		vol[k]=pBQ->Battery.voltage;
		printf("value of battery is %d\r\n",pBQ->Battery.voltage);
		HAL_Delay(100);
		vol_mean+=vol[k];
	}
	printf("sum of voltage is %f\r\n",vol_mean);
	vol_mean/=50.0;
	
	for(uint8_t k=0;k<50;k++)
		vol_std+=pow(((double)vol[k]-vol_mean),2);
	
	vol_std /= 50.0;
	vol_std=sqrt(vol_std);
	printf("mean of voltage is %f\r\n",vol_mean);
	printf("standard of voltage is %f\r\n",vol_std);
//	if (vol_std>100)
//		return -1;
	
	uint16_t new_voltage_divider = ((double)appliedVoltage / vol_mean) * (double)pBQ->Battery.voltageDevider;
	printf("new voltage devider is %d\r\n",new_voltage_divider);
	pBQ->Battery.voltageDevider=new_voltage_divider;
	if(BQ_setVoltageDivider(pBQ)!=0)
		return -2;
	printf("*****************finish calibration ******************\r\n");
	return 0;
	
}
int BQ_writeRegister(BQ_HandleTypedef *pBQ,uint8_t *CMD,uint8_t lengthOfCMD,uint8_t *TxData,uint8_t lengthTxData)
{
	uint8_t lengthOfTxBuffer=lengthTxData+lengthOfCMD;
	uint8_t *Txbuffer=(uint8_t*)malloc(lengthOfTxBuffer * sizeof(uint8_t));
	int8_t timeout=10;
	int TxST=0;
	
	memcpy(Txbuffer,CMD,lengthOfCMD);
	memcpy(&Txbuffer[lengthOfCMD],TxData,lengthTxData);
	//show_hex("TXData=>",TxData,lengthTxData);
	show_hex("BQ:Write Data=>",Txbuffer,lengthOfTxBuffer);
	
	while(timeout>0)		
	{
		if (HAL_I2C_Master_Transmit(pBQ->Config.i2c,(uint16_t)BQ_I2C_ADDRESS,Txbuffer,lengthOfTxBuffer ,100)!=HAL_OK)
		{
			timeout--;
			printf("timeout =%d\r\n",timeout);	
			HAL_Delay(200);
			TxST=-1;
		}
		else
		{
			TxST=0;
			break;		
		}
				
	}
	
	if (TxST==-1)		
		printf("transmition failed\r\n");
	else
		printf("transmition is OK\r\n");
	
	memset(Txbuffer,0,lengthOfTxBuffer);
	free(Txbuffer);
	return TxST;
		
}
int BQ_readRegister(BQ_HandleTypedef *pBQ,uint8_t *CMD,uint8_t lengthOfCMD,uint8_t *RxData,uint8_t lengthRxData)
{
	//show_hex("BQ_CMD=>",CMD,lengthOfCMD);
	if (HAL_I2C_Master_Transmit(pBQ->Config.i2c,(uint16_t)BQ_I2C_ADDRESS,&CMD[0],1,100)!=HAL_OK)
		return -1;
	
	if(HAL_I2C_Master_Receive(pBQ->Config.i2c,(uint16_t)BQ_I2C_ADDRESS,RxData,lengthRxData,100)!=HAL_OK)
		return -2;
	
	show_hex("BQ_RxData=>",RxData,lengthRxData);
	
	return 0;
}

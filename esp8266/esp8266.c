/*
 * esp8266.c
 *
 *  Created on: Aug 14, 2021
 *      Author: Amin-PC
 */


#include "esp8266.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"


uint8_t  *RxPtr;

uint32_t RXCounterLast;

#define MIN_LENGTH 20

static Wifi_Status compareReceivedData(Wifi_HandleTypeDef *wifi, const char *acceptedData, uint16_t delay);
static Wifi_Status waitToGetData(Wifi_HandleTypeDef *wifi,uint16_t minimumLength, uint16_t delay);
static Wifi_Status extractInformation(char *infBuffer, const char *recvData,const char *searchStr,\
		uint8_t startOffset, uint8_t maxlength, const char *endChar);
//static Wifi_Status getDataFromRxBuffer(Wifi_HandleTypeDef *wifi,uint8_t minimumLength, uint16_t delay, char *receivedData);
static uint32_t getNumberReceivedData(Wifi_HandleTypeDef *wifi);
static uint32_t getRxCounter(Wifi_HandleTypeDef *wifi);
static uint8_t *getRxBufferPtr(Wifi_HandleTypeDef *wifi);
static void recordRxCounter(Wifi_HandleTypeDef *wifi);
static uint8_t *getRecordedRxBufferPtr(Wifi_HandleTypeDef *wifi);
static void recordRxBufferPtr(Wifi_HandleTypeDef *wifi);
static Wifi_Status clearRxBuffer(Wifi_HandleTypeDef *wifi);
static Wifi_Status clearTxBuffer(Wifi_HandleTypeDef *wifi);
static Wifi_State checkWifiState(Wifi_HandleTypeDef *wifi);
static void setWifiState(Wifi_HandleTypeDef *wifi, Wifi_State state, uint32_t delay);
static Wifi_Status checkValidation(const char *data, const char *instruction, char *startPtr, char *endPtr);
static Wifi_Status extractAPs(Wifi_HandleTypeDef *wifi, const char *data);



/***************************************************************************************
 ******************************** High level functions *********************************
 ***************************************************************************************/

Wifi_Status wifi_init(Wifi_HandleTypeDef *wifi){

	Wifi_Status stts;
	//check pointer of wifi_handel
	if (wifi==NULL){
		printf("error::wifi_init:: pointer error\r\n");
		return WIFI_STATUS_ERR_PTR;
	}


	if (HAL_UART_Receive_DMA(wifi->uart,wifi->Buffer.RX, MAXIMUM_SIZE_BUFFER)!=HAL_OK)
		return WIFI_STATUS_ERROR;

	setWifiState(wifi, WIFI_STATE_READY, 0);

	//test wifi module
	printf("test device \r\n");
	stts=wifi_testDevice(wifi);
	printf("status of (init-> testDevice) => %d\r\n",stts);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERROR;
	printf("device has just been tested (OK)\r\n");


	/*
	 * to configure wifi device
	 * 1. set mode
	 *   */
	printf("set configuration of device \r\n");
	if (wifi_setModeDefault(wifi)!=WIFI_STATUS_OK) //request to change mode of device
		return WIFI_STATUS_ERROR;
	printf("the mode of device has just been changed to 'mode=%c' \r\n", wifi->cnfg.mode);


	return WIFI_STATUS_OK;
}

Wifi_Status wifi_testDevice(Wifi_HandleTypeDef *wifi){

	const char *rqstData="AT\r\n"; // request data
	const char *accptData="AT\r\r\n\r\nOK\r\n";	//accepted response
	char receivedData[20]={0};
	Wifi_Status stts;


	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData), 5*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;

	//check validation of received data
	if (memcmp(receivedData,accptData,strlen(accptData))!=0)
		return WIFI_STATUS_ERR_RSPN2;

	//	printf("received data [%d]\r\n=>%s",strlen((char *)receivedData),(uint8_t*)receivedData);
	return WIFI_STATUS_OK;


}

Wifi_Status wifi_createServer(Wifi_HandleTypeDef *wifi){


	//enable multiple connection
	if (wifi_setMultipleConnections(wifi,WIFI_MULTIPLE_CONNECTION_ENABLE)!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERROR;


}


/***************************************************************************************
 ******************************** Middle level functions *******************************
 ***************************************************************************************/

Wifi_Status wifi_reset(Wifi_HandleTypeDef *wifi){

	const char *rqstData="AT+RST\r\n"; // request data
	const char *accptData="AT+RST\r\r\n\r\nOK\r\n";	//accepted response
	char receivedData[20]={0};
	uint8_t length;
	Wifi_Status stts;

	//	if (checkWifiState(wifi)!=WIFI_STATE_READY)
	//		return WIFI_STATUS_ERROR1;

	/*send request data*/
	if (wifi_sendData(wifi,rqstData,strlen(rqstData))!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERROR2;

	setWifiState(wifi, WIFI_STATE_BUSY, 30*MAXIMUM_DELAY);

	length=strlen(accptData);
	stts=getDataFromRxBuffer(wifi,length,5*MAXIMUM_DELAY,receivedData);

	//	if (stts!=WIFI_STATUS_OK)
	//		return WIFI_STATUS_ERR_RSPN1;


	if (memcmp(receivedData,accptData,length)!=0)
		return WIFI_STATUS_ERR_RSPN2;


	printf("received data [%d]\r\n=>%s",strlen((char *)receivedData),(uint8_t*)receivedData);

	return WIFI_STATUS_OK;


}

/*
 * @brief	: Sets the Default Wi-Fi mode; Configuration Saved in the Flash
 * @mode: 1	: station/ 2 : softAP / 3: AP+station
 * @return 	: Wifi_Status
 *
 * @note: The configuration changes will be saved in the system parameter area in the flash
 * */
Wifi_Status wifi_setModeDefault(Wifi_HandleTypeDef *wifi){

	// instruction
	char *instruction="AT+CWMODE_DEF";
	char rqstData[20],accptData[40];
	char receivedData[40]={0};
	Wifi_Status stts;
	sprintf(rqstData,"%s=%c\r\n",instruction,wifi->cnfg.mode);
	sprintf(accptData,"%s=%c\r\r\n\r\nOK\r\n",instruction,wifi->cnfg.mode);


	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData), 5*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;

	if (strcmp(accptData,receivedData)!=0)
		return WIFI_STATUS_ERR_RSPN2;

	return WIFI_STATUS_OK;
}

/*
 * @brief	: Sets the current Wi-Fi mode; Configuration Not Saved in the Flash
 * @mode: 1	: station/ 2 : softAP / 3: AP+station
 * @return 	: Wifi_Status
 *
 * */
Wifi_Status wifi_setModeCurrent(Wifi_HandleTypeDef *wifi){

	const char *rqstData="AT+CWMODE_DEF=3\r\n"; // request data
	const char *accptData="AT+CWMODE_DEF\r\r\n\r\nOK\r\n";	//accepted response
	char receivedData[40]={0};
	Wifi_Status stts;


	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData), 5*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;

	if (strcmp(accptData,receivedData)!=0)
		return WIFI_STATUS_ERR_RSPN2;

	return WIFI_STATUS_OK;
}
/*
 * @brief	: Gets the Local IP Address
 * @mode: 1	:
 * @return 	: Wifi_Status
 *
 * */
Wifi_Status wifi_getLocalIpAddress(Wifi_HandleTypeDef *wifi){

	const char *instruction="AT+CIFSR";
	char rqstData[20]; // request data
	uint8_t offset=2;
	uint16_t minimumlength=2*MIN_LENGTH;
	char receivedData[20*MIN_LENGTH];
	Wifi_Status stts;

	sprintf(rqstData,"%s\r\n",instruction);
	//key words
	const char *APIP="CIFSR:APIP";
	const char *APMAC="CIFSR:APMAC";
	const char *STAIP="CIFSR:STAIP";
	const char *STAMAC="CIFSR:STAMAC";


	stts=wifi_sendATComand(wifi, rqstData, receivedData, minimumlength, 20*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;

	stts=checkValidation(receivedData, instruction, NULL, NULL );
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN2;


	/*extract information*/
	if (wifi->cnfg.mode==WIFI_MODE_SoftAP || wifi->cnfg.mode==WIFI_MODE_AP_Station){
		//extract APIP
		if (extractInformation(wifi->IP.AP, receivedData, APIP,offset, 20, (char*)"\"")!= WIFI_STATUS_OK)
			return WIFI_STATUS_ERROR1;
		printf("wifi->IP.AP => %s\r\n",wifi->IP.AP);

		//extract APMAC
		if (extractInformation(wifi->MAC.AP, receivedData, APMAC,offset, 20, (char*)"\"")!= WIFI_STATUS_OK)
			return WIFI_STATUS_ERROR2;
		printf("wifi->MAC.AP => %s\r\n",wifi->MAC.AP);
	}

	if(wifi->cnfg.mode==WIFI_MODE_Station || wifi->cnfg.mode==WIFI_MODE_AP_Station){
		//extract STAIP
		if (extractInformation(wifi->IP.STA, receivedData, STAIP,offset, 20, (char*)"\"")!= WIFI_STATUS_OK)
			return WIFI_STATUS_ERROR3;
		printf("wifi->IP.STA => %s\r\n",wifi->IP.STA);

		//extract STAMAC
		if (extractInformation(wifi->MAC.STA, receivedData, STAMAC,offset, 20, (char*)"\"")!= WIFI_STATUS_OK)
			return WIFI_STATUS_ERROR4;
		printf("wifi->MAC.STA => %s\r\n",wifi->MAC.STA);

	}
	return WIFI_STATUS_OK;
}

Wifi_Status wifi_configureListsAvailableAP(Wifi_HandleTypeDef *wifi){

	const char *instruction="AT+CWLAPOPT";
	char accptedData[40]={0};
	char rqstData[30]={0};
	char receivedData[80]={0};
	char maskstr[10]={0};
	const char *OKstr="\r\nOK\r\n";
	char RSSI;
	Wifi_Status stts;
	uint16_t mask=0;

	if (wifi->cnfg.ListAP.AllParameter==Enable){
		RSSI='1';
		/*The second parameter is 2047, namely 0x7FF,
		meaning that the corresponding bits of <mask> are set to 1.*/
		mask=0x07FF; //or 2047
		itoa(mask, maskstr, 10);
		sprintf(rqstData,"%s=%c,%s\r\n",instruction,RSSI,maskstr);
		sprintf(accptedData,"%s=%c,%s\r\r\n%s",instruction,RSSI,maskstr,OKstr);
	}

	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData), 20*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;

	if(strcmp(receivedData,accptedData)!=0)
		return WIFI_STATUS_ERR_RSPN2;

	return WIFI_STATUS_OK;
}

Wifi_Status wifi_getListAP(Wifi_HandleTypeDef *wifi){
	const char *instruction="AT+CWLAP";
	char rqstData[30]={0};
	Wifi_Status stts;
	char receivedData[MAXIMUM_SIZE_BUFFER]={0};
	sprintf(rqstData,"%s\r\n",instruction);


	wifi->cnfg.ListAP.AllParameter=Enable;
	wifi_configureListsAvailableAP(wifi);


	stts=wifi_sendATComand(wifi, rqstData, receivedData, 100, 50*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;

	//printf("strlnd receivedData => %d \r\n",strlen(receivedData));
	stts=checkValidation(receivedData, instruction, NULL, NULL);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN2;

	printf("received data (wifi_configureListsAvailableAP) => \r\n %s",receivedData);

	extractAPs(wifi,receivedData);
	return WIFI_STATUS_OK;

}

Wifi_Status wifi_connectToAP(Wifi_HandleTypeDef *wifi, const char *ssidName, const char *password){

	const char *instruction="AT+CWJAP";
	char rqstData[60]={0};
	Wifi_Status stts;
	char receivedData[100]={0};
	sprintf(rqstData,"%s=\"%s\",\"%s\"\r\n",instruction,ssidName,password); //AT+CWJAP="ssid","password"

	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData)+30, 100*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;


	if (checkValidation(receivedData, rqstData, NULL, NULL)!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN2;

	return WIFI_STATUS_OK;

}

Wifi_Status wifi_disconnectFromAP(Wifi_HandleTypeDef *wifi){

	const char *instruction="AT+CWQAP";
	char rqstData[60]={0};
	Wifi_Status stts;
	char receivedData[100]={0};
	sprintf(rqstData,"%s\r\n",instruction);

	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData), 20*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;


	if (checkValidation(receivedData, rqstData, NULL, NULL)!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN2;


	return WIFI_STATUS_OK;

}

Wifi_Status wifi_setMultipleConnections(Wifi_HandleTypeDef *wifi, char mltplConn_mode){

	const char *instruction="AT+CIPMUX";
	char rqstData[60]={0};
	Wifi_Status stts;
	char receivedData[100]={0};
	sprintf(rqstData,"%s=%c\r\n",instruction,mltplConn_mode);

	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData), 20*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;


	if (checkValidation(receivedData, instruction, NULL, NULL)!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN2;


	return WIFI_STATUS_OK;
}

Wifi_Status wifi_configAsServer(Wifi_HandleTypeDef *wifi, char CnfgSrvr_mode, uint16_t port){

	const char *instruction="AT+CIPSERVER";
	char rqstData[60]={0};
	Wifi_Status stts;
	char portStr[10]={0};
	char receivedData[100]={0};


	sprintf(rqstData,"%s=%c",instruction,CnfgSrvr_mode);
	sprintf(portStr,",%d",port);
	if (port!=WIFI_CONFIG_SERVER_PORT_DFAULT && CnfgSrvr_mode==WIFI_CINFIG_SERVER_CEREATE)
		strcat(rqstData,portStr);
	strcat(rqstData,"\r\n");

	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData), 20*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;


	if (checkValidation(receivedData, instruction, NULL, NULL)!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN2;

	if (strstr(receivedData,(char*)"no change")!=NULL)
		WIFI_STATUS_OK;


	return WIFI_STATUS_OK;
}

Wifi_Status wifi_configAsHotspot(Wifi_HandleTypeDef *wifi){

	//set wifi as hotspot (config softAP)
	const char *instruction="AT+CWSAP=\"NETTCO\",\"123456789\",5,3";
	char rqstData[60]={0};
	Wifi_Status stts;
	char portStr[10]={0};
	char receivedData[100]={0};

	sprintf(rqstData,"%s\r\n",instruction);

	stts=wifi_sendATComand(wifi, rqstData, receivedData, strlen(rqstData), 20*MAXIMUM_DELAY);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;


	if (checkValidation(receivedData, instruction, NULL, NULL)!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN2;


	return WIFI_STATUS_OK;

}

/***************************************************************************************
 ******************************** low level functions **********************************
 ***************************************************************************************/


Wifi_Status wifi_sendATComand(Wifi_HandleTypeDef *wifi, const char *rqstData ,char *receivedData,uint16_t minimumLength,uint16_t maximumDelay){



	if (checkWifiState(wifi)!=WIFI_STATE_READY)
		return WIFI_STATUS_ERROR1;

	/*send request data*/
	if (wifi_sendData(wifi,rqstData,strlen(rqstData))!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERROR1;

	if (getDataFromRxBuffer(wifi,minimumLength,maximumDelay,receivedData)!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;

	printf("received data=> \r\n%s\r\n",receivedData);
	return WIFI_STATUS_OK;

}

Wifi_Status wifi_sendData(Wifi_HandleTypeDef *wifi, const char *data, uint16_t size){

	if (data==NULL||size==0)
		return WIFI_STATUS_ERR_PTR;



	/*record the last pointer of RX buffer*/
	recordRxCounter(wifi); //record the last rx counter
	recordRxBufferPtr(wifi);

	clearRxBuffer(wifi);



	//send request to esp
	if (HAL_UART_Transmit(wifi->uart,(uint8_t *)data,size,100)!=HAL_OK) //send data
		return WIFI_STATUS_ERROR;

	printf("send data => %s\r\n",data);
	return WIFI_STATUS_OK;
}

 Wifi_Status getDataFromRxBuffer(Wifi_HandleTypeDef *wifi,uint8_t minimumLength, uint16_t delay, char *receivedData) {

	uint16_t numRcvData=0;
	uint8_t *RXBuffPtr;
	uint8_t *RXBuffPtrBase=wifi->uart->pRxBuffPtr;
	uint16_t delayNull=10*MAXIMUM_DELAY;
	Wifi_Status stts;


	//wait to get some data form wifi
	stts=waitToGetData(wifi,(int)minimumLength,delay);
	if (stts!=WIFI_STATUS_OK)
		return WIFI_STATUS_ERR_RSPN1;

	RXBuffPtr=getRecordedRxBufferPtr(wifi); //get the latest pointer which data have received

	while(delayNull--){
		numRcvData=getNumberReceivedData(wifi);
		recordRxCounter(wifi); //record the last NDTR
		if (numRcvData){
			delayNull=10*MAXIMUM_DELAY;// reset delay
			// copy data
			while(numRcvData--){
				*receivedData=*RXBuffPtr++;
				receivedData++;
				if (RXBuffPtr>=(RXBuffPtrBase+wifi->uart->RxXferSize))
					RXBuffPtr=RXBuffPtrBase; //reset pointer of RX
			}
			if (memcmp((receivedData-4),(uint8_t*)"OK\r\n",4)==0)
				break;
		}
		HAL_Delay(1);
	}
	recordRxCounter(wifi);
	recordRxBufferPtr(wifi);
	return WIFI_STATUS_OK;
}

static Wifi_Status waitToGetData(Wifi_HandleTypeDef *wifi,uint16_t minimumLength, uint16_t delay){
	uint16_t numRcvData=0;

	while(delay--){
		numRcvData=getNumberReceivedData(wifi);
		if(numRcvData>=minimumLength)
			return WIFI_STATUS_OK;
		HAL_Delay(1);
	}

	return WIFI_STATUS_ERR_RSPN;

}

static Wifi_Status checkValidation(const char *data, const char *instruction, char *startPtr, char *endPtr){

	startPtr=NULL;
	char *OKstrPtr;

	char *OKstr="\r\nOK\r\n";
	/*
	 * check instruction of received data
	 * if instruction is NULL startPtr will be NULL
	 * */
	if(instruction!=NULL){
		startPtr=strstr(data,instruction);
		if (!startPtr)
			return WIFI_STATUS_ERR_RSPN1;
	}

	//check OK string
	OKstrPtr=strstr(data,OKstr); //searching string
	if (!OKstrPtr)
		return WIFI_STATUS_ERR_RSPN2;

	endPtr=OKstr+strlen(OKstr);

	return WIFI_STATUS_OK;
}

static Wifi_Status extractAPs(Wifi_HandleTypeDef *wifi,const char *data){


	char CWLAP[100]={0};
	char *ptr;
	char *ptr_start,*ptr_end;


	const char *searchstr_start="(";
	const char *searchstr_end=")";
	const char *searchstr_comma=",";
	const char *CWLAPstr="CWLAP:";
	uint8_t numAP=0;
	uint8_t length;




	while( 1 ) {

		//ex
		memset(CWLAP,0,100); //clear buffer
		ptr = strstr(data, CWLAPstr);
		ptr_start=strstr(data, searchstr_start);
		ptr_end=strstr(data, searchstr_end);

		if (!ptr || !ptr_start || !ptr_end)
			break;

		length=ptr_end-ptr_start;
		memcpy(CWLAP,ptr_start+1,length-1);
		data+=(ptr_end-data+1);
		//printf("CWLAP => %s\r\n",CWLAP);



		//extract ECN
		ptr=strtok(CWLAP, searchstr_comma);
		wifi->AP[numAP].ECN=atoi(ptr);
		//printf("wifi->AP[%d].ECN => %d \r\n",numAP,wifi->AP[numAP].ECN);

		//extract SSID
		ptr=strtok(NULL, searchstr_comma);
		memset(wifi->AP[numAP].ssid,0,20);
		length=strlen(ptr);
		*(ptr+length-1)=0;//remove the last '"'
		strcpy(wifi->AP[numAP].ssid,ptr+1);
		//printf("wifi->AP[%d].ssid => %s \r\n",numAP,wifi->AP[numAP].ssid);


		//extract rssi
		ptr=strtok(NULL, searchstr_comma);
		wifi->AP[numAP].rssi=atoi(ptr);
		//printf("wifi->AP[%d].rssi => %d \r\n",numAP,wifi->AP[numAP].rssi);

		//extract MAC
		ptr=strtok(NULL, searchstr_comma);
		memset(wifi->AP[numAP].MAC,0,20);
		length=strlen(ptr);
		*(ptr+length-1)=0;//remove the last '"'
		strcpy(wifi->AP[numAP].MAC,ptr+1);
		//printf("wifi->AP[%d].MAC => %s \r\n",numAP,wifi->AP[numAP].MAC);

		numAP++;
	}

	return WIFI_STATUS_OK;
}

static Wifi_Status extractInformation(char *infBuffer, const char *recvData,const char *searchStr,uint8_t startOffset, uint8_t maxlength, const char *endChar){

	char *searchPtr;
	memset(infBuffer,0,maxlength);
	//search keywords
	searchPtr=strstr(recvData,searchStr);
	if (searchPtr==NULL)
		return WIFI_STATUS_ERR_PTR;

	//extract information after keywords
	searchPtr+=(strlen(searchStr)+startOffset);
	while(*searchPtr!=*endChar){
		*infBuffer=*searchPtr++;
		infBuffer++;
	}

	return WIFI_STATUS_OK;
}

static uint32_t getNumberReceivedData(Wifi_HandleTypeDef *wifi){
	uint16_t num=0;
	uint16_t ndtr=wifi->uart->hdmarx->Instance->NDTR;
	uint16_t ndtrLast=wifi->Buffer.RXNDTRRecorded;

	if (ndtr>ndtrLast)
		num=ndtrLast+wifi->uart->RxXferSize-ndtr;
	else
		num=ndtrLast-ndtr; //calculate the number of received data and

	return num;
}

static uint32_t getRxCounter(Wifi_HandleTypeDef *wifi){
	return wifi->uart->hdmarx->Instance->NDTR;
}

static void recordRxCounter(Wifi_HandleTypeDef *wifi){
	wifi->Buffer.RXNDTRRecorded = wifi->uart->hdmarx->Instance->NDTR;
	return;
}

static void recordRxBufferPtr(Wifi_HandleTypeDef *wifi){
	wifi->Buffer.RXPtrRecorded=getRxBufferPtr(wifi);
	return;
}
static uint8_t *getRecordedRxBufferPtr(Wifi_HandleTypeDef *wifi){
	return wifi->Buffer.RXPtrRecorded;
}

static uint8_t *getRxBufferPtr(Wifi_HandleTypeDef *wifi){

	uint16_t RXndtr=(uint16_t)wifi->uart->hdmarx->Instance->CNDTR;
	uint16_t RXBufferSize=wifi->uart->RxXferSize;
	uint8_t *RxBuffPtr=wifi->uart->pRxBuffPtr;
	return RxBuffPtr+(RXBufferSize-RXndtr);
}

static Wifi_Status clearRxBuffer(Wifi_HandleTypeDef *wifi){

	if (memset(wifi->uart->pRxBuffPtr,0,wifi->uart->RxXferSize)!=0)//clear rx buffer
		return WIFI_STATUS_ERROR;
	return WIFI_STATUS_OK;
}

static Wifi_Status clearTxBuffer(Wifi_HandleTypeDef *wifi){
	if (memset(wifi->uart->pTxBuffPtr,0,wifi->uart->TxXferSize)!=0)//clear rx buffer
		return WIFI_STATUS_ERROR;
	return WIFI_STATUS_OK;
}

static Wifi_State checkWifiState(Wifi_HandleTypeDef *wifi){
	uint32_t time = uwTick-wifi->Instance.busyTimeTick;


	if (time>=wifi->Instance.busyDelay || wifi->Instance.state==WIFI_STATE_READY ){
		wifi->Instance.state=WIFI_STATE_READY;
		return WIFI_STATE_READY;
	}

	return WIFI_STATE_BUSY;
}

static void setWifiState(Wifi_HandleTypeDef *wifi, Wifi_State state, uint32_t delay){

	wifi->Instance.busyDelay=delay;
	wifi->Instance.busyTimeTick=uwTick;//record the sysTick time
	wifi->Instance.state=state;

	return;

}

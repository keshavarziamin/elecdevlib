/*
 * esp8266.h
 *
 *  Created on: Aug 14, 2021
 *      Author: Amin-PC
 */

#ifndef ESP8266_ESP8266_H_
#define ESP8266_ESP8266_H_

#include "main.h"
#include "stdio.h"
#include "string.h"

#define RXBUFFER_SIZE	wifi->uart->RxXferSize
#define RX_COUNTER		wifi->uart->hdmarx->Instance->NDTR
#define RXBUFFER_POINTER_BASE	wifi->uart->pRxBuffPtr

#define RXBUFFER_POINTER_CURRENT 	(uint8_t*)RXBUFFER_POINTER_BASE+(RXBUFFER_SIZE-RX_COUNTER)





#define MAXIMUX_TRY	5
#define MAXIMUM_DELAY 100 //ms
#define MINIMUM_SIZ_BUFFER 100
#define MAXIMUM_SIZE_BUFFER 1024
#define MAXIMUM_NUMBER_AP 20

//mode of system --> AT+CWMODE_CUR & AT+CWMODE_DEF
#define WIFI_MODE_Station 		'1' // 1:  Station_mode
#define WIFI_MODE_SoftAP 		'2' // 2: SoftAP mode
#define WIFI_MODE_AP_Station 	'3' // 3: SoftAP+Station mode



#define WIFI_CONECTION_TYPE_TCP	"TCP"
#define WIFI_CONECTION_TYPE_UDP	"UDP"
#define WIFI_CONECTION_TYPE_SSL	"SSL"

#define WIFI_CONNECTION_STATUS_CONECTED_AP 			2
#define WIFI_CONNECTION_STATUS_CREATED_TCPUDP		3
#define WIFI_CONNECTION_STATUS_TCPUDP_DEICONNECT	4
#define WIFI_CONNECTION_STATUS_NOT_CONNECTION		5


#define WIFI_CONNECTION_TETYPE_CLIENT	'0'
#define WIFI_CONNECTION_TETYPE_SERVER	'1'

#define WIFI_MULTIPLE_CONNECTION_DISABLE '0'
#define WIFI_MULTIPLE_CONNECTION_ENABLE '1'

#define WIFI_CONFIG_SERVER_DELETE	'0'
#define WIFI_CINFIG_SERVER_CEREATE	'1'

#define WIFI_CONFIG_SERVER_PORT_DFAULT 333

typedef enum __Wifi_Status{
	WIFI_STATUS_OK=0,

	WIFI_STATUS_ERROR=-1,
	WIFI_STATUS_ERR_PTR=-2, //pointer error
	WIFI_STATUS_ERR_RSPN=-3,	//respond error

	WIFI_STATUS_ERROR1=-11,
	WIFI_STATUS_ERROR2=-12,
	WIFI_STATUS_ERROR3=-13,
	WIFI_STATUS_ERROR4=-14,
	WIFI_STATUS_ERROR5=-15,

	WIFI_STATUS_ERR_PTR1=-21, //pointer error
	WIFI_STATUS_ERR_PTR2=-22, //pointer error
	WIFI_STATUS_ERR_PTR3=-23, //pointer error
	WIFI_STATUS_ERR_PTR4=-24, //pointer error


	WIFI_STATUS_ERR_RSPN1=-31,
	WIFI_STATUS_ERR_RSPN2=-32,
	WIFI_STATUS_ERR_RSPN3=-33,
	WIFI_STATUS_ERR_RSPN4=-34,
	WIFI_STATUS_ERR_RSPN5=-35


}Wifi_Status;

typedef enum __wifi_Function{
	Enable=1,
	Disable=0
}Wifi_Function;

typedef struct __wifi_GPIO{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
}WifiGPIO_typeDef;


typedef struct __wifi_CWLAP{
	uint8_t ECN;
	char ssid[20];
	int16_t rssi;
	char MAC[20];
}WifiCWLAP_typedef;

typedef struct __wifi_ConfigurationIlstAP{
	Wifi_Function AllParameter;

}WifiConfigurationIlstAP_typeDef;

typedef struct __wifi_configuration{
	char mode;
	WifiConfigurationIlstAP_typeDef ListAP;
}WifiConfiguration_typeDef;

typedef struct __wifi_IPAddress{
	char AP	[20];
	char STA	[20];
}WifiIPAddress_typeDef;

typedef enum __Wifi_state{
	WIFI_STATE_READY=0,
	WIFI_STATE_BUSY=-1
}Wifi_State;

typedef struct __wifi_buffer{
	uint8_t  RX[MAXIMUM_SIZE_BUFFER];
	uint16_t RXNDTRRecorded;
	uint8_t *RXPtrBase;
	uint8_t *RXPtrRecorded;
	////////////////////////////////
	uint8_t  TX[MAXIMUM_SIZE_BUFFER];
	uint16_t TXNDTRRecorded;
	uint8_t *TXPtrBase;
	uint8_t *TXPtrRecorded;
}WifiBuffer_typedef;


typedef struct __wifi_Instance{

	Wifi_State state;
	uint32_t busyTimeTick;
	uint32_t busyDelay;
}WifiInstance_typeDef;


typedef struct __wifi_Conection{
	uint8_t Status;
	char type[3];
	uint8_t linkID;
	char	addr[20];
	uint16_t port;
	char tetype;
}WifiConnection_typedef;



typedef struct __wifi_handletypedef{
	UART_HandleTypeDef *uart;
	WifiGPIO_typeDef enable;
	WifiGPIO_typeDef reset;
	WifiInstance_typeDef Instance;
	WifiBuffer_typedef Buffer;

	WifiConfiguration_typeDef cnfg;
	WifiCWLAP_typedef AP[MAXIMUM_NUMBER_AP];
	WifiIPAddress_typeDef IP;
	WifiIPAddress_typeDef MAC;
	WifiConnection_typedef Connection;

}Wifi_HandleTypeDef;


Wifi_Status wifi_init(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_testDevice(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_reset(Wifi_HandleTypeDef *wifi);


/*
 * WIFI AT Commands
 */
Wifi_Status wifi_sendATComand(Wifi_HandleTypeDef *wifi, const char *rqstData ,char *receivedData,uint16_t minimumLength,uint16_t maximumDelay);
Wifi_Status wifi_setModeDefault(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_setModeCurrent(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_getLocalIpAddress(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_configureListsAvailableAP(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_getListAP(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_connectToAP(Wifi_HandleTypeDef *wifi, const char * ssidName, const char *password);
Wifi_Status wifi_disconnectFromAP(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_setMultipleConnections(Wifi_HandleTypeDef *wifi, char mltplConn_mode);
Wifi_Status wifi_configAsHotspot(Wifi_HandleTypeDef *wifi);
Wifi_Status wifi_configAsServer(Wifi_HandleTypeDef *wifi, char CnfgSrvr_mode, uint16_t port);
/*******************************************************************************/
Wifi_Status wifi_sendData(Wifi_HandleTypeDef *wifi, const char *data, uint16_t size);

Wifi_Status getDataFromRxBuffer(Wifi_HandleTypeDef *wifi,uint8_t minimumLength, uint16_t delay, char *receivedData);


#endif /* ESP8266_ESP8266_H_ */

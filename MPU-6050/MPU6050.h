#ifndef MPU6050_H
#define MPU6050_H


#include "stm32f4xx_hal.h"
#include "i2c.h"

//address of device 
#define MPU6050_ADDRESS 										0xD0
#define MPU6050_MEM_SIZE										0x0F
//registers
#define MPU6050_REG_SELF_TEST_X							0X0D
#define MPU6050_REG_SELF_TEST_Y             0X0E
#define MPU6050_REG_SELF_TEST_Z             0X0F
#define MPU6050_REG_SELF_TEST_A             0X10
#define MPU6050_REG_SMPLRT_DIV              0X19
#define MPU6050_REG_CONFIG                  0X1A
#define MPU6050_REG_GYRO_CONFIG             0X1B
#define MPU6050_REG_ACCEL_CONFIG            0X1C
#define MPU6050_REG_FIFO_EN                 0X23
#define MPU6050_REG_I2C_MST_CTRL            0X24
#define MPU6050_REG_I2C_SLV0_ADDR           0X25
#define MPU6050_REG_I2C_SLV0_REG            0X26
#define MPU6050_REG_I2C_SLV0_CTRL           0X27
#define MPU6050_REG_I2C_SLV1_ADDR           0X28
#define MPU6050_REG_I2C_SLV1_REG            0X29
#define MPU6050_REG_I2C_SLV1_CTRL           0X2A
#define MPU6050_REG_I2C_SLV2_ADDR           0X2B
#define MPU6050_REG_I2C_SLV2_REG            0X2C
#define MPU6050_REG_I2C_SLV2_CTRL           0X2D
#define MPU6050_REG_I2C_SLV3_ADDR           0X2E
#define MPU6050_REG_I2C_SLV3_REG            0X2F
#define MPU6050_REG_I2C_SLV3_CTRL           0X30
#define MPU6050_REG_I2C_SLV4_ADDR           0X31
#define MPU6050_REG_I2C_SLV4_REG            0X32
#define MPU6050_REG_I2C_SLV4_DO             0X33
#define MPU6050_REG_I2C_SLV4_CTRL           0X34
#define MPU6050_REG_I2C_SLV4_DI             0X35
#define MPU6050_REG_I2C_MST_STATUS          0X36
#define MPU6050_REG_INT_PIN_CFG             0X37
#define MPU6050_REG_INT_ENABLE              0X38
#define MPU6050_REG_INT_STATUS              0X3A
#define MPU6050_REG_ACCEL_XOUT_H            0X3B
#define MPU6050_REG_ACCEL_XOUT_L            0X3C
#define MPU6050_REG_ACCEL_YOUT_H            0X3D
#define MPU6050_REG_ACCEL_YOUT_L            0X3E
#define MPU6050_REG_ACCEL_ZOUT_H            0X3F
#define MPU6050_REG_ACCEL_ZOUT_L            0X40
#define MPU6050_REG_TEMP_OUT_H              0X41
#define MPU6050_REG_TEMP_OUT_L              0X42
#define MPU6050_REG_GYRO_XOUT_H             0X43
#define MPU6050_REG_GYRO_XOUT_L             0X44
#define MPU6050_REG_GYRO_YOUT_H             0X45
#define MPU6050_REG_GYRO_YOUT_L             0X46
#define MPU6050_REG_GYRO_ZOUT_H             0X47
#define MPU6050_REG_GYRO_ZOUT_L             0X48
#define MPU6050_REG_EXT_SENS_DATA_00        0X49
#define MPU6050_REG_EXT_SENS_DATA_01        0X4A
#define MPU6050_REG_EXT_SENS_DATA_02        0X4B
#define MPU6050_REG_EXT_SENS_DATA_03        0X4C
#define MPU6050_REG_EXT_SENS_DATA_04        0X4D
#define MPU6050_REG_EXT_SENS_DATA_05        0X4E
#define MPU6050_REG_EXT_SENS_DATA_06        0X4F
#define MPU6050_REG_EXT_SENS_DATA_07				0X50
#define MPU6050_REG_EXT_SENS_DATA_08        0X51
#define MPU6050_REG_EXT_SENS_DATA_09        0X52
#define MPU6050_REG_EXT_SENS_DATA_10        0X53
#define MPU6050_REG_EXT_SENS_DATA_11        0X54
#define MPU6050_REG_EXT_SENS_DATA_12        0X55
#define MPU6050_REG_EXT_SENS_DATA_13        0X56
#define MPU6050_REG_EXT_SENS_DATA_14        0X57
#define MPU6050_REG_EXT_SENS_DATA_15        0X58
#define MPU6050_REG_EXT_SENS_DATA_16        0X59
#define MPU6050_REG_EXT_SENS_DATA_17        0X5A
#define MPU6050_REG_EXT_SENS_DATA_18        0X5B
#define MPU6050_REG_EXT_SENS_DATA_19        0X5C
#define MPU6050_REG_EXT_SENS_DATA_20        0X5D
#define MPU6050_REG_EXT_SENS_DATA_21        0X5E
#define MPU6050_REG_EXT_SENS_DATA_22        0X5F
#define MPU6050_REG_EXT_SENS_DATA_23        0X60
#define MPU6050_REG_I2C_SLV0_DO             0X63
#define MPU6050_REG_I2C_SLV1_DO             0X64
#define MPU6050_REG_I2C_SLV2_DO             0X65
#define MPU6050_REG_I2C_SLV3_DO             0X66
#define MPU6050_REG_I2C_MST_DELAY_CTRL      0X67
#define MPU6050_REG_SIGNAL_PATH_RESET       0X68
#define MPU6050_REG_USER_CTRL               0X6A
#define MPU6050_REG_PWR_MGMT_1              0X6B
#define MPU6050_REG_PWR_MGMT_2              0X6C
#define MPU6050_REG_FIFO_COUNTH             0X72
#define MPU6050_REG_FIFO_COUNTL             0X73
#define MPU6050_REG_FIFO_R_W                0X74
#define MPU6050_REG_WHO_AM_I                0X75





//
typedef struct{
	I2C_HandleTypeDef *i2c;
} MPU6050_HandleTypedef;


//FUNCTIONS
void MPU6050_init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef MPU6050_readData(uint8_t RegAddr,uint8_t *Data,uint16_t Size);
HAL_StatusTypeDef MPU6050_writeData(uint8_t RegAddr,uint8_t *Data,uint16_t Size);






#endif





















































































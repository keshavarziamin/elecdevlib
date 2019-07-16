#include "MPU6050.h"

MPU6050_HandleTypedef MPU_conf;
void MPU6050_init(I2C_HandleTypeDef *hi2c)
{
	MPU_conf.i2c=hi2c;
}

void MPU6050_readData(uint8_t *Addr,uint8_t *Data,uint8_t Size)
{
	HAL_I2C_Mem_Read(MPU_conf.i2c,)
}
void MPU6050_writeData(uint8_t *Addr,uint8_t *Data,uint8_t Size )
{
	
}
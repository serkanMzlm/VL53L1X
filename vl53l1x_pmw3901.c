#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "spi.h"
#include "vl53l1_api.h"
#include "vl53l1_platform.h"

static uint8_t mode = 0;
static uint32_t speed = 400000;


int main()
{	
	VL53L1X_Result_t result;
	int file_spi,file_i2c, status;
	uint8_t sensorState;
	int16_t x,y,dev;
	
	if((file_i2c =VL53L1X_UltraLite_Linux_I2C_Init(dev))<0)
	{
		printf("can not open i2c file");
		return -1;
	}

	while(sensorState ==0)
	{
		status = VL53L1X_BootState(dev,&sensorState);
		VL53L1_WaitMs(dev,2);
	}

	status=VL53L1X_SensorInit(dev);
	status=VL53L1X_SetDistanceMode(dev,2);
	status=VL53L1X_SetTimingBudgetInMs(dev,50);
	status=VL53L1X_SetInterMeasurementInMs(dev,50);
	status=VL53L1X_StartRanging(dev);




	file_spi = open("/dev/spidev0.0",O_RDWR);
	if(file_spi<1 )
	{
		printf("can not open file\n");
		return -1;
	}
	if((status=ioctl(file_spi,SPI_IOC_RD_MODE,&mode))<0)
	{
		printf("can not read SPI mode\n");
		return -1;
	}
	if((status=ioctl(file_spi,SPI_IOC_WR_MODE,&mode))<0)
	{
		printf("can not set SPI mode\n");
		return -1;
	}
	

	if((status=ioctl(file_spi,SPI_IOC_WR_MAX_SPEED_HZ,&speed))<0)
	{
		printf("can not set SPI speed\n");
		return -1;
	}
	if((status=ioctl(file_spi,SPI_IOC_RD_MAX_SPEED_HZ,&speed))<0)
	{
		printf("can not read SPI speed\n");
		return -1;
	}

	pmw3901_init(file_spi);
	while(1)
	{
		uint8_t dataReady=0;
		while(dataReady==0)
		{
			status=VL53L1X_CheckForDataReady(dev,&dataReady);
			usleep(5);
		}
		pmw3901_readMotion(file_spi,&x,&y);
		status=VL53L1X_GetResult(dev,&result);
		printf("x:%4d  |  y:%4d   | z:%4d\n",x,y,result.Distance);
		status = VL53L1X_ClearInterrupt(dev);

	}


	return 0;
}

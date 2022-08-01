
#include "vl53l1_api.h"
#include "vl53l1_platform.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
    VL53L1X_Result_t result;
    int file;
  
    uint16_t dev;
    int status,sensorState;
    
    if((file = VL53L1X_UltraLite_Linux_I2C_Init(dev))<0)
    {
        printf("i2c dosyasi acilmadi");
        return -1;
    }

    while (sensorState==0)
    {
        status=VL53L1X_BootState(dev,&sensorState);
        VL53L1_WaitMs(dev,2); 
    }
    printf("Chip Booted\n");
    status = VL53L1X_SensorInit(dev);
    status = VL53L1X_SetDistanceMode(dev,2);
    status = VL53L1X_SetTimingBudgetInMs(dev,100);
    status = VL53L1X_SetInterMeasurementInMs(dev,100);
    status = VL53L1X_StartRanging(dev);

    while(1)
    {
        uint8_t dataReady=0;
        while(dataReady==0)
        {
            status = VL53L1X_CheckForDataReady(dev,&dataReady);
            usleep(1);
        }
        status = VL53L1X_GetResult(dev,&result);
        printf("%d\n",result.Distance);
        status = VL53L1X_ClearInterrupt(dev);

    }




    return 0;
}

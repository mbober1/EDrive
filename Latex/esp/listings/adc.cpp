#include "adc.hpp"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "task_mngm.hpp"


/**
 * Contructs ADC object and set it up.
 */
myADC::myADC() 
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_6);

    for (int i = 0; i < ADC_DATA_SIZE; i++) nextMeasurement();
}

/**
 * Adds another measurement to the buffer
 */
void myADC::nextMeasurement() 
{
    static int idx;
    this->data[idx++] = adc1_get_raw(ADC1_CHANNEL_0);
    if(idx >= ADC_DATA_SIZE) idx = 0;
}

/**
 * Calculates the average of the measurements in the buffer 
 * and convert the result into SI units.
 * @return Voltage in V
 */
float myADC::getVoltage() 
{
    uint32_t adc_reading = 0;
    
    for (size_t i = 0; i < ADC_DATA_SIZE; i++) 
    {
        adc_reading += this->data[i];
    }

    return ((float)adc_reading / ADC_DATA_SIZE) / CONVERSION_FACTOR;
}


void batteryTask(void*)
{
    myADC battery;
    int idx = 0;
    float voltage;

    while (1)
    {
        battery.nextMeasurement();

        if (idx++ > 10)
        {
            idx = 0;
            voltage = battery.getVoltage();
            xQueueSendToBack(voltageQueue, &voltage, 0);
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
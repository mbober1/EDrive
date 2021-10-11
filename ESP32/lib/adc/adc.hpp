#pragma once
#include "freertos/FreeRTOS.h"

#define CONVERSION_FACTOR 415
#define ADC_DATA_SIZE 10

class myADC {
    uint16_t data[ADC_DATA_SIZE] = {};

public:
    myADC();
    void nextMeasurement();
    float getVoltage();
};

void batteryTask(void*);

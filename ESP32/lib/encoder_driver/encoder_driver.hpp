#pragma once

#include "driver/pcnt.h"
#include "esp_log.h"

#define PCNT_COUNT_LIMIT 1000
#define ENCODER_TAG "ENCODER"






class Encoder
{
private:
    
public:
    Encoder(gpio_num_t encoderA, gpio_num_t encoderB, pcnt_unit_t pcnt_unit);
    pcnt_unit_t pcnt_unit;
};
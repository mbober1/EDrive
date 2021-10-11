#pragma once

#include "driver/pcnt.h"
#include "esp_log.h"

#define PCNT_COUNT_LIMIT 1000
#define ENCODER_TAG "ENCODER"


#define GET_ENCODER_VALUE(pcnt_unit, input)        pcnt_get_counter_value(pcnt_unit, input); \
    pcnt_counter_clear(pcnt_unit);


class Encoder
{
private:
    
public:
    Encoder(gpio_num_t encoderA, gpio_num_t encoderB, pcnt_unit_t pcnt_unit);
    pcnt_unit_t pcnt_unit;
};
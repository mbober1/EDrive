#pragma once
#include <stdint.h>
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "math.h"

extern QueueHandle_t powerQueue;

enum class Direction {
    FORWARD,
    BACKWARD
};


#define MAX_POWER 1023
#define MAX_INTEGRAL 100


/**
    @brief Engine management class
*/
class motor
{
private:
    gpio_num_t in1, in2, pwmPin;
    int kp, ki, kd;
    int16_t epsilonOld, epsilonSuma, integralError, derivativeError;
    ledc_channel_config_t ledc_channel = {};
    ledc_timer_config_t ledc_timer = {};
    uint16_t countedPulses;

    void power(const uint16_t &pow);
    void fastStop();
    void direction(const Direction &dir);
    void softStop();
    
public:
    pcnt_unit_t encoder;
    motor(gpio_num_t in1, gpio_num_t in2, gpio_num_t encoderA, gpio_num_t encoderB, gpio_num_t pwmPin, ledc_channel_t pwmChannel, pcnt_unit_t pcntUnit);
    void compute(const int &setpoint);
    void setKP(int kp);
    void setKI(int ki);
    void setKD(int kd);
    uint16_t getPulses();
};


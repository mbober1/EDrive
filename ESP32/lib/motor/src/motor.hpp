#pragma once
#include <stdint.h>
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "math.h"

extern QueueHandle_t powerQueue;

#define MAX_PID 100
#define MIN_PID -100
#define MAX_INTEGRAL 50


/**
    @brief Engine management class
*/
class motor
{
private:
    gpio_num_t in1, in2, enPin;
    int16_t epsilonOld, epsilonSuma, integralError, derivativeError;
    uint16_t countedPulses;
    int kp, ki, kd;

public:
    pcnt_unit_t encoder;
    motor(gpio_num_t in1, gpio_num_t in2, gpio_num_t encoderA, gpio_num_t encoderB, gpio_num_t pwmPin, ledc_channel_t pwmChannel, pcnt_unit_t pcntUnit);
    void compute(const int &setpoint);
    void setKP(int kp);
    void setKI(int ki);
    void setKD(int kd);
    int16_t getPulses();
};


#pragma once

#include "driver/pcnt.h"
#include "driver/mcpwm.h"
#include "freertos/queue.h"

extern QueueHandle_t powerQueue;
extern QueueHandle_t pulsesQueue;

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
    pcnt_unit_t encoder;
    int16_t epsilonOld, epsilonSuma, integralError, derivativeError;
    int kp, ki, kd;

public:
    motor(gpio_num_t in1, gpio_num_t in2, gpio_num_t encoderA, gpio_num_t encoderB, gpio_num_t enPin, pcnt_unit_t pcntUnit);
    void compute(const int &setpoint);
    void setKP(int kp);
    void setKI(int ki);
    void setKD(int kd);
};


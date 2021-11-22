#pragma once

#include "driver/mcpwm.h"
#include "esp_log.h"

#define MOTOR_TAG "MOTOR"
#define MOTOR_PWM_FREQUENCY (40000)     // 40kHz/2 (For symmetric MCPWM, frequency is half of MCPWM frequency set)


class Motor
{
private:
    mcpwm_unit_t mcpwm_unit;
    mcpwm_timer_t timer_num;

public:
    Motor(mcpwm_unit_t mcpwm_unit, gpio_num_t in1, gpio_num_t in2, mcpwm_timer_t timer_num);
    void set_duty(float duty_cycle);
};
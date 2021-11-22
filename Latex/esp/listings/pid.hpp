#pragma once

#include "encoder_driver.hpp"
#include "motor_driver.hpp"


#define PID_LIMIT 100
#define INTEGRAL_LIMIT 50

#define NORMALIZE_I(integral_error) ({\
    if(integral_error > INTEGRAL_LIMIT) integral_error = INTEGRAL_LIMIT;\
    else if(integral_error < -INTEGRAL_LIMIT) integral_error = -INTEGRAL_LIMIT; })

#define NORMALIZE_PID(pid) ({\
    if(pid > PID_LIMIT) pid = PID_LIMIT;\
    else if(pid < -PID_LIMIT) pid = -PID_LIMIT; })


typedef struct 
{
    uint16_t kp : 6;
    uint16_t ki : 5;
    uint16_t kd : 5;

} Pid_config_t;


class Pid
{
private:
    int16_t last_epsilon;
    int16_t integral_error;
    int16_t derivative_error;
    pcnt_unit_t pcnt_unit;
    Pid_config_t config;

public:
    Pid(const Encoder &encoder, const Pid_config_t *config);
    int16_t compute(const int &setpoint);
    void set_parameters(const Pid_config_t *config);
};

void motorTask(void*);
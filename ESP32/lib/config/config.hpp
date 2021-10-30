#pragma once

#include "driver/pcnt.h"
#include "driver/mcpwm.h"

#define RPMtoTick(RPM)  ((int) (RPM * 11 * PID_LOOP_PERIOD / 100))


/*---------- PID CONFIG ----------*/
#define DEFAULT_PID_KP 3
#define DEFAULT_PID_KI 2
#define DEFAULT_PID_KD 1
#define PID_LOOP_PERIOD (1)   // 1 * 10ms
/*--------------------------------*/


/*-------- ENCODER CONFIG --------*/
const gpio_num_t ENC_A = GPIO_NUM_39;
const gpio_num_t ENC_B = GPIO_NUM_34;
const pcnt_unit_t ENC_PCNT_UNIT = PCNT_UNIT_0;
/*--------------------------------*/


/*--------- MOTOR CONFIG ---------*/
const gpio_num_t MOTOR_IN1 = GPIO_NUM_27;
const gpio_num_t MOTOR_IN2 = GPIO_NUM_26;
const mcpwm_unit_t MOTOR_UNIT = MCPWM_UNIT_0;
const mcpwm_timer_t MOTOR_TIMER = MCPWM_TIMER_0;
/*--------------------------------*/


/*--------- TOPICS CONFIG --------*/
#define SETPOINT_TOPIC "edrive/setpoint"
#define KP_TOPIC "edrive/kp"
#define KI_TOPIC "edrive/ki"
#define KD_TOPIC "edrive/kd"

/*--------------------------------*/

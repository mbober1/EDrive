#pragma once
#define INCLUDE_vTaskSuspend 1

#include <string>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_pm.h"
#include "driver/pcnt.h"

// module includes
#include "motor.hpp"
#include "adc.hpp"

// watchdog includes
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

// motors config
const gpio_num_t ENC_A = GPIO_NUM_32;
const gpio_num_t ENC_B = GPIO_NUM_33;

const gpio_num_t MOTOR_PWM_PIN = GPIO_NUM_12;
const gpio_num_t MOTOR_IN1 = GPIO_NUM_14;
const gpio_num_t MOTOR_IN2 = GPIO_NUM_13;
const pcnt_unit_t MOTOR_PCNT = PCNT_UNIT_0;
const ledc_channel_t MOTOR_PWM_CHANNEL = LEDC_CHANNEL_0;

// queues init
QueueHandle_t powerQueue, voltageQueue, setpointQueue;
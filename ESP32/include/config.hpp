#pragma once
#define INCLUDE_vTaskSuspend 1

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// module includes
#include "motor.hpp"
#include "adc.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"

// motors config
const gpio_num_t ENC_A = GPIO_NUM_39;
const gpio_num_t ENC_B = GPIO_NUM_34;

const gpio_num_t MOTOR_PWM_PIN = GPIO_NUM_2;
const gpio_num_t MOTOR_IN1 = GPIO_NUM_27;
const gpio_num_t MOTOR_IN2 = GPIO_NUM_26;
const pcnt_unit_t MOTOR_PCNT = PCNT_UNIT_0;

// queues init
QueueHandle_t powerQueue, voltageQueue, setpointQueue, pulsesQueue;
QueueHandle_t kpQueue, kiQueue, kdQueue;
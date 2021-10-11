#pragma once

#include "mqtt_client.h"
#include "esp_log.h"

#define MQTT_TAG "MQTT"

enum Pid_parameter_id
{
    KP_PARAMETER,
    KI_PARAMETER,
    KD_PARAMETER,
    SETPOINT_PARAMETER,
};

typedef struct 
{
    uint16_t parameter : 2;
    int16_t value : 14;

} Pid_message_t;

void mqttTask(void*);
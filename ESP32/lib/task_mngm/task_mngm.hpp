#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define TASK_MNGM_TAG "TASK MNGM"



/*-------- TASK STRUCTS --------*/

struct Task_config_t
{
    TaskFunction_t pvTaskCode;
    const char *pcName;
    uint32_t usStackDepth;
    void *pvParameters;
    UBaseType_t uxPriority;
    TaskHandle_t pvCreatedTask;
    BaseType_t xCoreID;
};

enum Task_id 
{
    MOTOR_TASK_ID,
    WIFI_TASK_ID,
    MQTT_TASK_ID,
    BATTERY_TASK_ID,
    TASKS_MAX_ID,
};

/*------------------------------*/





/*------- QUEUES STRUCTS -------*/

struct Queue_config_t
{
    QueueHandle_t handle;
    uint32_t uxItemSize;
    uint32_t uxQueueLength;
    const char *name;
};

enum Queue_id 
{
    PID_QUEUE_ID,
    POWER_QUEUE_ID,
    SETPOINT_QUEUE_ID,
    VOLTAGE_QUEUE_ID,
    PULSES_QUEUE_ID,
    QUEUES_MAX_ID,
};

/*------------------------------*/



/*----------- QUEUES -----------*/
extern Queue_config_t queue[QUEUES_MAX_ID];

#define pidQueue        queue[PID_QUEUE_ID].handle
#define powerQueue      queue[POWER_QUEUE_ID].handle
#define setpointQueue   queue[SETPOINT_QUEUE_ID].handle
#define voltageQueue    queue[VOLTAGE_QUEUE_ID].handle
#define pulsesQueue     queue[PULSES_QUEUE_ID].handle
/*------------------------------*/






enum class Mode 
{
    OPERATIONAL,
    SLEEP
};



class Task_manager
{

private:
    Mode mode;

public:
    Task_manager();
    ~Task_manager();
    void start_schedule();
    void end_schedule();
    void start_task(uint8_t id);
    void stop_task(uint8_t id);
    void suspend_task(uint8_t id);
    void resume_task(uint8_t id);
    void set_mode(Mode new_mode);
    void init_queues();
};


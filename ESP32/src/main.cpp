#include "task_mngm.hpp"
#include "esp_log.h"

extern "C" void app_main()
{
    Task_manager task_manager;
    task_manager.init_queues();
    task_manager.set_mode(Mode::OPERATIONAL);
    task_manager.start_task(MOTOR_TASK_ID);
    vTaskDelay(100);
    task_manager.start_task(WIFI_TASK_ID);
    vTaskDelay(100);
    task_manager.start_task(MQTT_TASK_ID);
    vTaskDelay(100);
    task_manager.start_task(BATTERY_TASK_ID);
    vTaskDelay(100);


    vTaskSuspend(NULL);
    task_manager.end_schedule();
    vTaskDelete(NULL);
}
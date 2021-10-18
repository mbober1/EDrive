#include "task_mngm.hpp"
#include "esp_log.h"



extern "C" void app_main()
{
    Task_manager task_manager;
    task_manager.init_queues();
    task_manager.start_schedule();

    while (true)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);

    }

    vTaskSuspend(NULL);
    task_manager.end_schedule();
    vTaskDelete(NULL);
}
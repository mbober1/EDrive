#include "task_mngm.hpp"
#include "esp_log.h"

extern "C" void app_main()
{
    Task_manager task_manager;
    task_manager.init_queues();
    task_manager.start_schedule();
    task_manager.set_mode(Mode::OPERATIONAL);


    vTaskSuspend(NULL);
    task_manager.end_schedule();
    vTaskDelete(NULL);
}
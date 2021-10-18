#include "task_mngm.hpp"
#include "esp_pm.h"
#include "esp_log.h"

#include "pid.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"
#include "adc.hpp"


/*--------- TASKS ----------*/
extern void motorTask(void*);
extern void wifiTask(void*);
extern void mqttTask(void*);
extern void batteryTask(void*);
/*--------------------------*/


Task_config_t tasks[TASKS_MAX_ID] =
{
    {
        .pvTaskCode = motorTask,
        .pcName = "motorTask",
        .usStackDepth = 4096,
        .pvParameters = nullptr,
        .uxPriority = configMAX_PRIORITIES,
        .pvCreatedTask = NULL,
        .xCoreID = 1,
    },

    {
        .pvTaskCode = batteryTask,
        .pcName = "batteryTask",
        .usStackDepth = 4096,
        .pvParameters = nullptr,
        .uxPriority = 3,
        .pvCreatedTask = NULL,
        .xCoreID = 0,
    },
    
    {
        .pvTaskCode = wifiTask,
        .pcName = "wifiTask",
        .usStackDepth = 4096,
        .pvParameters = nullptr,
        .uxPriority = 4,
        .pvCreatedTask = NULL,
        .xCoreID = 0,
    },

    {
        .pvTaskCode = mqttTask,
        .pcName = "mqttTask",
        .usStackDepth = 4096,
        .pvParameters = nullptr,
        .uxPriority = 5,
        .pvCreatedTask = NULL,
        .xCoreID = 0,
    },
};


Queue_config_t queue[QUEUES_MAX_ID] =
{
    {
        .handle = NULL,
        .uxItemSize = sizeof(Pid_config_t),
        .uxQueueLength = 10,
        .name = "PID Queue",
    },

    {
        .handle = NULL,
        .uxItemSize = sizeof(int16_t),
        .uxQueueLength = 10,
        .name = "Power Queue",
    },

    {
        .handle = NULL,
        .uxItemSize = sizeof(int16_t),
        .uxQueueLength = 10,
        .name = "Setpoint Queue",
    },

    {
        .handle = NULL,
        .uxItemSize = sizeof(float),
        .uxQueueLength = 10,
        .name = "Voltage Queue",
    },

    {
        .handle = NULL,
        .uxItemSize = sizeof(int),
        .uxQueueLength = 10,
        .name = "Pulses Queue",
    },
};


Task_manager::Task_manager() {}

Task_manager::~Task_manager()
{
    esp_log_level_set(TASK_MNGM_TAG, ESP_LOG_VERBOSE);
    this->end_schedule();
}

void Task_manager::start_schedule() 
{
    for (size_t i = 0; i < TASKS_MAX_ID; i++)
    {
        this->start_task(i);
    }
}

void Task_manager::end_schedule() 
{
    for (size_t i = 0; i < TASKS_MAX_ID; i++)
    {
        this->stop_task(i);
    }
    
}


void Task_manager::start_task(uint8_t id) 
{
    BaseType_t result = xTaskCreatePinnedToCore
    (
        tasks[id].pvTaskCode, 
        tasks[id].pcName, 
        tasks[id].usStackDepth, 
        tasks[id].pvParameters,
        tasks[id].uxPriority,
        &(tasks[id].pvCreatedTask),
        tasks[id].xCoreID
    );

    if(pdPASS == result)
    {
        ESP_LOGI(TASK_MNGM_TAG, "Started task '%s' on core %d", tasks[id].pcName, tasks[id].xCoreID);
    }
    else
    {
        ESP_LOGI(TASK_MNGM_TAG, "Unable to start task '%s' on core %d", tasks[id].pcName, tasks[id].xCoreID);
    }

}

void Task_manager::stop_task(uint8_t id) 
{
    if(tasks[id].pvCreatedTask == NULL) 
    {
        ESP_LOGI(TASK_MNGM_TAG, "Cannot remove task '%s' becouse handle is null", tasks[id].pcName);
    }
    else 
    {
        vTaskDelete(tasks[id].pvCreatedTask);
        ESP_LOGI(TASK_MNGM_TAG, "Ended task '%s' from core %d", tasks[id].pcName, tasks[id].xCoreID);
    }
}

void Task_manager::suspend_task(uint8_t id) 
{
    if(tasks[id].pvCreatedTask == NULL) 
    {
        ESP_LOGI(TASK_MNGM_TAG, "Cannot suspend task '%s' becouse handle is null", tasks[id].pcName);
    }
    else 
    {
        vTaskSuspend(tasks[id].pvCreatedTask);
        ESP_LOGI(TASK_MNGM_TAG, "Suspended task '%s' on core %d", tasks[id].pcName, tasks[id].xCoreID);
    }
}

void Task_manager::resume_task(uint8_t id) 
{
    vTaskResume(tasks[id].pvCreatedTask);
    ESP_LOGI(TASK_MNGM_TAG, "Suspended task '%s' on core %d", tasks[id].pcName, tasks[id].xCoreID);
}



void Task_manager::set_mode(Mode new_mode)
{
    if(this->mode == new_mode) // no mode change
    {
        return;
    }
    else // change to different mode
    {
        this->mode = new_mode;
    }


    esp_pm_config_esp32_t power;

    switch (mode)
    {
        case Mode::OPERATIONAL:
        {
            ESP_LOGI(TASK_MNGM_TAG, "Going to operational mode");
            this->resume_task(MOTOR_TASK_ID);
            this->resume_task(BATTERY_TASK_ID);
            
            power = 
            {
                .max_freq_mhz = 240, 
                .min_freq_mhz = 240, 
                .light_sleep_enable = false
            };

            break;
        }

        case Mode::SLEEP:
        {
            ESP_LOGI(TASK_MNGM_TAG, "Going to sleep mode");
            this->suspend_task(MOTOR_TASK_ID);
            this->suspend_task(BATTERY_TASK_ID);

            power = 
            {
                .max_freq_mhz = 80, 
                .min_freq_mhz = 80, 
                .light_sleep_enable = true
            };

        }

    }
    
    esp_pm_configure(&power);

}


void Task_manager::init_queues() 
{
    for (size_t id = 0; id < QUEUES_MAX_ID; id++)
    {
        queue[id].handle = xQueueCreate(queue[id].uxQueueLength, queue[id].uxItemSize);
        ESP_LOGI(TASK_MNGM_TAG, "Queue %s' initialized", queue[id].name);
    }
}
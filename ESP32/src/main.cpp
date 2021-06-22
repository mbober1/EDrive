#include "config.hpp"


void batteryTask(void*) 
{
    myADC battery;

    while (1)
    {
        float voltage = battery.getVoltage();

        printf("Voltage: %.2fV\n", voltage);
        xQueueSendToBack(voltageQueue, &voltage, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


void motorDriver(void*) 
{
    int setpoint = 0;
    motor engine(MOTOR_IN1, MOTOR_IN2, ENC_A, ENC_B, MOTOR_PWM_PIN, MOTOR_PWM_CHANNEL, MOTOR_PCNT);

    while (1) 
    {
        xQueueReceive(setpointQueue, &setpoint, 0);
        engine.compute(setpoint);
        // engine.compute(15);
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


extern "C" void app_main()
{
    initialise_wifi();

    powerQueue = xQueueCreate(10, sizeof(int16_t));
    voltageQueue = xQueueCreate(10, sizeof(float));
    setpointQueue = xQueueCreate(10, sizeof(int));


    xTaskCreate(motorDriver, "motorTask", 4096, nullptr, 20, nullptr);
    // xTaskCreate(batteryTask, "batteryTask", 4096, nullptr, 3, nullptr);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    mqtt_app_start();
}
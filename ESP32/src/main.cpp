#include "config.hpp"


void batteryTask(void*) {
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


void motorDriver(void*) {
    motor Motor(MOTOR_IN1, MOTOR_IN2, ENC_A, ENC_B, MOTOR_PWM_PIN, MOTOR_PWM_CHANNEL, MOTOR_PCNT);
    int setpoint = 0;

    while (1) {
        xQueueReceive(setpointQueue, &setpoint, 0);
        Motor.compute(setpoint);
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


extern "C" void app_main()
{
    powerQueue = xQueueCreate(10, sizeof(int16_t));
    voltageQueue = xQueueCreate(10, sizeof(float));
    setpointQueue = xQueueCreate(10, sizeof(int));


    xTaskCreate(motorDriver, "motorTask", 4096, nullptr, 20, nullptr);
    xTaskCreate(batteryTask, "batteryTask", configMINIMAL_STACK_SIZE * 3, nullptr, 3, nullptr);
}
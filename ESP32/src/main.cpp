#include "config.hpp"


void batteryTask(void*) 
{
    myADC battery;

    while (1)
    {
        battery.nextMeasurement();
        float voltage = battery.getVoltage();

        xQueueSendToBack(voltageQueue, &voltage, 0);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


void motorDriver(void*) 
{
    int setpoint = 0;
    int kp = 3;
    int ki = 2;
    int kd = 0;

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1;

    motor engine(MOTOR_IN1, MOTOR_IN2, ENC_A, ENC_B, MOTOR_PWM_PIN, MOTOR_PWM_CHANNEL, MOTOR_PCNT);
    engine.setKP(kp);
    engine.setKI(ki);
    engine.setKD(kd);

    while (1) 
    {   
        xLastWakeTime = xTaskGetTickCount();

        if(xQueueReceive(kpQueue, &kp, 0)) engine.setKP(kp);
        if(xQueueReceive(kiQueue, &ki, 0)) engine.setKI(ki);
        if(xQueueReceive(kdQueue, &kd, 0)) engine.setKD(kd);

        xQueueReceive(setpointQueue, &setpoint, 0);
        engine.compute(setpoint);

        int16_t pulses = engine.getPulses();
        xQueueSend(pulsesQueue, &pulses, 0);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
    vTaskDelete(NULL);
}


extern "C" void app_main()
{
    initialise_wifi();

    kpQueue = xQueueCreate(10, sizeof(int));
    kiQueue = xQueueCreate(10, sizeof(int));
    kdQueue = xQueueCreate(10, sizeof(int));
    powerQueue = xQueueCreate(10, sizeof(int16_t));
    pulsesQueue = xQueueCreate(10, sizeof(int16_t));
    voltageQueue = xQueueCreate(10, sizeof(float));
    setpointQueue = xQueueCreate(10, sizeof(int));


    xTaskCreatePinnedToCore(motorDriver, "motorTask", 4096, nullptr, 40, nullptr, 0);
    xTaskCreatePinnedToCore(batteryTask, "batteryTask", 4096, nullptr, 3, nullptr, 1);

    mqtt_client_start();

    vTaskSuspend(NULL);
}
#include "config.hpp"


void batteryTask(void*) 
{
    myADC battery;

    while (1)
    {
        battery.nextMeasurement();
        float voltage = battery.getVoltage();

        xQueueSendToBack(voltageQueue, &voltage, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


void motorDriver(void*) 
{
    int setpoint = 0;
    int kp = 40;
    int ki = 6;
    int kd = 2;

    motor engine(MOTOR_IN1, MOTOR_IN2, ENC_A, ENC_B, MOTOR_PWM_PIN, MOTOR_PWM_CHANNEL, MOTOR_PCNT);
    engine.setKP(kp);
    engine.setKI(ki);
    engine.setKD(kd);

    while (1) 
    {   
        uint32_t lastTime = esp_timer_get_time();
        if(xQueueReceive(kpQueue, &kp, 0)) engine.setKP(kp);
        if(xQueueReceive(kiQueue, &ki, 0)) engine.setKI(ki);
        if(xQueueReceive(kdQueue, &kd, 0)) engine.setKD(kd);

        xQueueReceive(setpointQueue, &setpoint, 0);
        engine.compute(setpoint);

        int16_t pulses = engine.getPulses();
        xQueueSend(pulsesQueue, &pulses, 0);
        // printf("%d\n", (uint32_t)esp_timer_get_time() - lastTime);
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
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


    xTaskCreate(motorDriver, "motorTask", 4096, nullptr, 20, nullptr);
    xTaskCreate(batteryTask, "batteryTask", 4096, nullptr, 3, nullptr);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    mqtt_app_start();
}
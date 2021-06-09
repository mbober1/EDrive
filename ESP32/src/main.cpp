#include "config.hpp"


void batteryTask(void*) {
    myADC battery;

    while (1)
    {
        // printf("Voltage: %.2fV | Percentage %3.0d\n", battery.getVoltage(), battery.getPercentage());
        // xQueueSendToBack(voltageQueue, battery.getVoltage(), 0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


void motorDriver(void*) {
    // RobotConfig config = {};
    // config.in1 = IN1;
    // config.in2 = IN2;
    // config.in3 = IN3;
    // config.in4 = IN4;
    // config.pwm1 = PWM1;
    // config.pwm2 = PWM2;
    // config.enc1a = ENC1A;
    // config.enc1b = ENC1B;
    // config.enc2a = ENC2A;
    // config.enc2b = ENC2B;
    // config.pcntUnit1 = PCNT1;
    // config.pcntUnit2 = PCNT2;
    // config.pwmChannel = MOTOR_PWM;

    // Robot robot(config);
    // EnginePacket packet;
    
    while (1) {
        // xQueueReceive(engineQueue, &packet, 0);
        // robot.drive(packet);

        // TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        // TIMERG0.wdt_feed=1;
        // TIMERG0.wdt_wprotect=0;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


extern "C" void app_main()
{
    powerQueue = xQueueCreate(10, sizeof(int16_t));
    voltageQueue = xQueueCreate(10, sizeof(float));


    xTaskCreate(motorDriver, "motorTask", 4096, nullptr, 20, nullptr);
    xTaskCreate(batteryTask, "batteryTask", configMINIMAL_STACK_SIZE * 3, nullptr, 3, nullptr);

}
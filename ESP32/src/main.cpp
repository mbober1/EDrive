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






#include "mqtt_client.h"
#include "esp_log.h"
#include <string>
static const char *TAG = "MQTT_EXAMPLE";

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;

    std::string topic(event->topic, event->topic_len);
    std::string data(event->data, event->data_len);

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_publish(client, "edrive/voltage", "11.69", 0, 1, 0);
        esp_mqtt_client_subscribe(client, "edrive/setpoint", 0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        printf("MQTT_EVENT_DISCONNECTED\n");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        printf("MQTT_EVENT_SUBSCRIBED, Topic: %s\n", topic.c_str());
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        printf("MQTT_EVENT_UNSUBSCRIBED, Topic: %s\n", topic.c_str());
        break;
        
    case MQTT_EVENT_PUBLISHED:
        printf("MQTT_EVENT_PUBLISHED, Topic: %s, Data: %s\n", topic.c_str(), data.c_str());
        break;

    case MQTT_EVENT_DATA: {
            printf("MQTT_EVENT_DATA, Topic: %s, Data: %s\n", topic.c_str(), data.c_str());
            
            if(topic == "edrive/setpoint") {
                int setpoint = std::atoi(data.c_str());
                xQueueSendToBack(setpointQueue, &setpoint, 0);
            }
            break;
        }

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        // if (event->error_handle->error_type == MQTT_TRANSPORT_OVER_TCP) {
        //     log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
        //     log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
        //     log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->error_type);
        //     ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->error_type));

        // }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}




static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.host = mqtt_host;
    mqtt_cfg.port = mqtt_port;
    mqtt_cfg.username = mqtt_username;
    mqtt_cfg.password = mqtt_password;

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
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
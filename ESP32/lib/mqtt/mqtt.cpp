#include "mqtt.hpp"
#include "task_mngm.hpp"
#include "secrets.hpp"
#include <string>

static bool connected = false;

static inline void subscribeAllTopics(esp_mqtt_client_handle_t &client) 
{
    esp_mqtt_client_subscribe(client, SETPOINT_TOPIC, 0);
    esp_mqtt_client_subscribe(client, KP_TOPIC, 0);
    esp_mqtt_client_subscribe(client, KI_TOPIC, 0);
    esp_mqtt_client_subscribe(client, KD_TOPIC, 0);
}


static inline int8_t get_parameter(const std::string &topic)
{ 
    if (topic == SETPOINT_TOPIC) return SETPOINT_PARAMETER;
    if (topic == KP_TOPIC) return KP_PARAMETER;
    if (topic == KI_TOPIC) return KI_PARAMETER;
    if (topic == KD_TOPIC) return KD_PARAMETER;

    return -1;
}


static inline void parse_data(std::string &topic, std::string &data) 
{
    static Pid_message_t new_paramether;
    new_paramether.parameter = get_parameter(topic);
    new_paramether.value = std::atoi(data.c_str());
    xQueueSendToBack(pidQueue, &new_paramether, 0);
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;

    std::string topic(event->topic, event->topic_len);
    std::string data(event->data, event->data_len);

    switch ((esp_mqtt_event_id_t)event_id) 
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "Connected to server");
            subscribeAllTopics(client);
            connected = true;
            ESP_LOGI(MQTT_TAG, "New MQTT sender task!");
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "Disconnected from server");
            connected = false;
            
            ESP_LOGI(MQTT_TAG, "Waiting 1 second...");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP_LOGI(MQTT_TAG, "Reconnecting...");
            esp_mqtt_client_reconnect(client);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "Subscribed new topic");
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "Unsubscribed topic");
            break;
            
        case MQTT_EVENT_DATA:
            parse_data(topic, data);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(MQTT_TAG, "Mqtt client error!");
            break;

        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(MQTT_TAG, "Connecting...");
            break;

        default:
            break;
    }
}




void mqttTask(void*)
{
    ESP_LOGI(MQTT_TAG, "Starting MQTT client");
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.host = mqtt_host;
    mqtt_cfg.port = mqtt_port;
    mqtt_cfg.username = mqtt_username;
    mqtt_cfg.password = mqtt_password;

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);


    int16_t pulses;
    float voltage;
    int16_t power;

    while (true)
    {
        while (connected)
        {
            if(xQueueReceive(pulsesQueue, &pulses, 0)) {
                std::string data = std::to_string(pulses);
                esp_mqtt_client_publish(client, "edrive/value", data.c_str(), data.length(), 0, 0);
            }

            if(xQueueReceive(voltageQueue, &voltage, 0)) {
                std::string data = std::to_string(voltage);
                esp_mqtt_client_publish(client, "edrive/voltage", data.c_str(), data.length(), 0, 0);
            }

            if(xQueueReceive(powerQueue, &power, 0)) {
                std::string data = std::to_string(power);
                esp_mqtt_client_publish(client, "edrive/pwm_duty", data.c_str(), data.length(), 0, 0);
            }
        }

        vTaskDelay(500 / portTICK_PERIOD_MS); 
    }
    
    vTaskDelete(NULL);
}
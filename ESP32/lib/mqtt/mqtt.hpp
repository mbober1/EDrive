#pragma once

#include "mqtt_client.h"
#include <string>
#include "esp_log.h"
#include "secrets.hpp"

const char *MQTT_TAG = "MQTT";
extern QueueHandle_t setpointQueue;
extern QueueHandle_t kpQueue, kiQueue, kdQueue;


void subscribeAllTopics(esp_mqtt_client_handle_t &client) {
    esp_mqtt_client_subscribe(client, "edrive/setpoint", 0);
    esp_mqtt_client_subscribe(client, "edrive/kp", 0);
    esp_mqtt_client_subscribe(client, "edrive/ki", 0);
    esp_mqtt_client_subscribe(client, "edrive/kd", 0);
}

void parseData(std::string &topic, std::string &data) {
    if(topic == "edrive/setpoint") {
        int setpoint = std::atoi(data.c_str());
        xQueueSendToBack(setpointQueue, &setpoint, 0);
    }

    else if(topic == "edrive/kp") {
        int kp = std::atoi(data.c_str());
        printf("New KP: %d\n", kp);
        xQueueSendToBack(kpQueue, &kp, 0);
    }

    else if(topic == "edrive/ki") {
        int ki = std::atoi(data.c_str());
        printf("New KI: %d\n", ki);
        xQueueSendToBack(kiQueue, &ki, 0);
    }

    else if(topic == "edrive/kd") {
        int kd = std::atoi(data.c_str());
        printf("New KD: %d\n", kd);
        xQueueSendToBack(kdQueue, &kd, 0);
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
        ESP_LOGI(MQTT_TAG, "Connected to server");
        subscribeAllTopics(client);
        esp_mqtt_client_publish(client, "edrive/voltage", "11.69", 0, 1, 0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(MQTT_TAG, "Disconnected from server\n");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(MQTT_TAG, "Subscribed, Topic: %s\n", topic.c_str());
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(MQTT_TAG, "Unsubscribed, Topic: %s\n", topic.c_str());
        break;
        
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(MQTT_TAG, "Published, Topic: %s, Data: %s\n", topic.c_str(), data.c_str());
        break;

    case MQTT_EVENT_DATA: {
            ESP_LOGI(MQTT_TAG, "Data event, Topic: %s, Data: %s\n", topic.c_str(), data.c_str());
            parseData(topic, data);
            break;
        }

    case MQTT_EVENT_ERROR:
        ESP_LOGI(MQTT_TAG, "EVENT ERROR!");
        break;

    default:
        ESP_LOGI(MQTT_TAG, "Other event id:%d", event->event_id);
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
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}
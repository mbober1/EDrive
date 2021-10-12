#include "wifi.hpp"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <cstring>
#include "secrets.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

static EventGroupHandle_t wifi_event_group;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT) {
        if(event_id == WIFI_EVENT_STA_START) esp_wifi_connect();
        else if(event_id == WIFI_EVENT_STA_DISCONNECTED) {
            esp_wifi_connect();
            ESP_LOGI(WIFI_TAG, "Unable to connect to %s, retrying...", wifi_ssid);
        }
    }
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "My IP address: %d.%d.%d.%d", IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // wifi default config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // handlers config
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    // wifi config
    wifi_config_t wifi_config = {};

    strcpy((char*)wifi_config.sta.ssid, wifi_ssid);
    strcpy((char*)wifi_config.sta.password, wifi_password);
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFI_TAG, "Wifi initialization finished");

    // waiting to connect
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) ESP_LOGI(WIFI_TAG, "Connected to SSID: %s", wifi_ssid);
    else if (bits & WIFI_FAIL_BIT) ESP_LOGI(WIFI_TAG, "Failed to connect to SSID: %s", wifi_ssid);
    else ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");

    // cleanup
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(wifi_event_group);
}

void wifiTask(void*)
{
    esp_log_level_set(WIFI_TAG, ESP_LOG_DEBUG);

    if (nvs_flash_init() == ESP_ERR_NVS_NO_FREE_PAGES) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    wifi_init_sta();
    vTaskDelete(NULL);
}
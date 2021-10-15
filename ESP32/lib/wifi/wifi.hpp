#pragma once

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define WIFI_TAG "WIFI"

void wifiTask(void*);
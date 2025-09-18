#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include <string.h>

#define ESP_WIFI_SSID "mqtt_client"
#define ESP_WIFI_PASS "12345678"
#define ESP_WIFI_CHANNEL 1
#define MAX_STA_CONN 2

void init_network();
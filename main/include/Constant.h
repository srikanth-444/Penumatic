#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"


#define MISO_PIN1 13
#define MOSI_PIN1 11
#define SCLK_PIN1 12
#define MISO_PIN2 37
#define MOSI_PIN2 35
#define SCLK_PIN2 36
#define MAX_TRANSFER_SZ 4096
#define ADC_CLOCK_SPEED 1000000
#define ADC_MODE 0
#define QUEUE_SIZE 1
#define NUM_ADCS 1
#define DATA_ACCUSATION_FREQUENCY_MS 10

#define ESP_WIFI_SSID "mqtt_client"
#define ESP_WIFI_PASS "12345678"
#define ESP_WIFI_CHANNEL 1
#define MAX_STA_CONN 2
#define PC_IP "192.168.4.1"

#define BUFFER_SIZE 10

#define SPIBUS          "spi bus"
#define NETWORK         "network"
#define ADC             "adc"
#define LIMBS           "limbs"
#define PRESSURE        "Pressure"
#define CIRCULAR_BUFF   "circular buffer"
#define SERIF           "serif"
#define ACTUATOR        "pneumatic actuator"

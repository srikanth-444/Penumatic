#include <stdio.h>
#include "Network.h"
#include "LTC2498.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define MISO_PIN 13
#define MOSI_PIN 11
#define SCLK_PIN 12
#define MAX_TRANSFER_SZ 4096
#define ADC_CLOCK_SPEED 1000000
#define ADC_MODE 0
#define QUEUE_SIZE 1
#define NUM_ADCS 1

#define DATA_ACCUSATION_FREQUENCY_MS 10

LTC2498 adc[NUM_ADCS];
int cs_pins[NUM_ADCS]={10};
SemaphoreHandle_t data_mutex;
SemaphoreHandle_t spi_mutex;

// Initialize SPI bus
void init_bus(){
  spi_bus_config_t buscfg = {
        .miso_io_num = MISO_PIN,
        .mosi_io_num = MOSI_PIN,
        .sclk_io_num = SCLK_PIN,
        .max_transfer_sz = MAX_TRANSFER_SZ
    };
   esp_err_t ret= spi_bus_initialize(SPI2_HOST, &buscfg, 0) ;
    if (ret != ESP_OK) {
        ESP_LOGE("ADC", "Failed to initialize spi bus: %d", ret);
    }
}

// Task to check if data is ready from ADCs
void check_data_ready(void* args){
    gpio_set_direction(MISO_PIN, GPIO_MODE_INPUT);
    xSemaphoreTake(data_mutex, portMAX_DELAY);
    ESP_LOGI("ADC", "Checking data ready status");
    for(int i=0; i<NUM_ADCS; i++){
        gpio_set_direction(adc[i].cs_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(adc[i].cs_pin, 0);
    
        esp_rom_delay_us(10);
        if(gpio_get_level(MISO_PIN) == 0) {
            adc[i].data_ready=true;
        } else {
            adc[i].data_ready=false;
        }
        gpio_set_level(adc[i].cs_pin, 1);
        
    }
    vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);
    
    xSemaphoreGive(spi_mutex);
    
}

// Task to read ADCs when data is ready
void read_adc(void* arg){
    xSemaphoreTake(spi_mutex, portMAX_DELAY);
    for(int i=0; i<NUM_ADCS; i++){
        if(adc[i].data_ready){
            LTC2498_read(&adc[i]);
            adc[i].data_ready=false;
        }
        ESP_LOGI("ADC", "ADC %d Data: %lu", i, adc[i].rx_buffer);
    }
    vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);  
    ESP_LOGI("ADC", "Finished reading ADCs");
    xSemaphoreGive(data_mutex);
    
}

// Main application
void setup(){
    init_network();
    init_bus();
    for (int i=0; i<NUM_ADCS; i++){
        adc[i].cs_pin=cs_pins[i];
        adc[i].clock_speed_hz=ADC_CLOCK_SPEED;
        adc[i].mode=ADC_MODE;
        adc[i].queue_size=QUEUE_SIZE;
        adc[i].data_ready=true;
        adc[i].rx_buffer=0;
        init_adc(&adc[i]);
    }
    data_mutex=xSemaphoreCreateBinary();
    spi_mutex=xSemaphoreCreateBinary();
    xSemaphoreGive(spi_mutex);
}

// Main function
void app_main(void){
setup();
xTaskCreate(check_data_ready, "check_data_ready", 1024, NULL, 5, NULL);
xTaskCreate(read_adc, "read_adc", 1024, NULL, 5, NULL);
while(1){
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
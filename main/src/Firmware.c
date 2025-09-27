#include <stdio.h>
#include "Network.h"
#include "LTC2498.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "Constant.h"
#include "circularBuffer.h"

LTC2498 adc[NUM_ADCS];
int cs_pins[NUM_ADCS]={10};
SemaphoreHandle_t spi_sem;

CircularBuffer cb_spi_network;

static uint8_t adc_cycle_index=0;
uint8_t reading_channels[]={LTC2498_P0_N1,LTC2498_P2_N3,LTC2498_P4_N5,LTC2498_P6_N7,
                      LTC2498_P8_N9,LTC2498_P10_N11,LTC2498_P12_N13,LTC2498_P14_N15};

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


// Task to read ADCs when data is ready
void read_adc(void* arg){
    while(1){
    gpio_set_direction(MISO_PIN, GPIO_MODE_INPUT);
    // ESP_LOGI("ADC", "Checking data ready status");
    xSemaphoreTake(spi_sem, portMAX_DELAY);
    for(int i=0; i<NUM_ADCS; i++){
            
            LTC2498_read(&adc[i]);
            uint64_t data_to_send= ((uint64_t)adc[i].tx_buffer << 32) | ((uint64_t)i << 28) | (uint64_t)adc[i].rx_buffer;
            if(cb_push(&cb_spi_network, data_to_send)){
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
            adc[i].command[0]=adc[i].reading_channel[adc_cycle_index];
            adc_cycle_index=(adc_cycle_index+1)%8;
         
            ESP_LOGI("ADC", "ADC %d Data: %lu", i, adc[i].rx_buffer);
        }  
    vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);  
    if (spi_sem) {
    xSemaphoreGive(spi_sem);
    } 
    else {
        ESP_LOGE("ADC","data_mutex is NULL before Give!");
    }
    // ESP_LOGI("STACK","read_adc min free: %d words", uxTaskGetStackHighWaterMark(NULL));
    }
}

void send_data_task(void* arg) {
    while (1) {
        if (cb_spi_network.count > 0) {
            uint64_t data;
            // ESP_LOGI("NETWORK", "Sending data from buffer, count: %d", cb_spi_network.count);
            if (cb_pop(&cb_spi_network, &data) == 0) {
                // Send raw 64-bit data as 8 bytes
                send_data((const char*)&data, sizeof(data));
            }
        }
        vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);
    }
}


// Main application
void setup(){
    init_network();
    // xSemaphoreGive(spi_sem);
    init_bus();
    for (int i=0; i<NUM_ADCS; i++){
        adc[i].serif->cs_pin=cs_pins[i];
        adc[i].serif->clock_speed_hz=ADC_CLOCK_SPEED;
        adc[i].serif->mode=ADC_MODE;
        adc[i].serif->queue_size=QUEUE_SIZE;
        adc[i].reading_channel=reading_channels;
        init_adc(&adc[i]);
    }
    cb_init(&cb_spi_network, BUFFER_SIZE);
    
}

// Main function
void app_main(void){
spi_sem=xSemaphoreCreateBinary();

if (spi_sem == NULL) {
        ESP_LOGE("MAIN", "Failed to create semaphores");
        while(1);
    }

  // start the alternation
setup();

xTaskCreate(read_adc, "read_adc", 3072, NULL, 1, NULL);
xTaskCreate(send_data_task, "send_data_task", 3072, NULL, 1, NULL);


while(1){
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
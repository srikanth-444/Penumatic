#include <stdio.h>
#include "Network.h"
#include "LTC2498.h"
#include "MC33996.h"
#include "DRV8311.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "Constant.h"
#include "circularBuffer.h"
#include "Rtos_tasks.h"

LTC2498 right_sensor;
LTC2498 left_sensor;
LTC2498 pressure_sensor;
MC33996 actuator;
DRV8311 motor_driver;

SemaphoreHandle_t network_sem;
SemaphoreHandle_t bus1_sem;
SemaphoreHandle_t bus2_sem;
SemaphoreHandle_t socket_mutex1;
SemaphoreHandle_t socket_mutex2;
CircularBuffer network_upload_buffer;

uint8_t differential_commands[] = {LTC2498_P0_N1,LTC2498_P2_N3,LTC2498_P4_N5,LTC2498_P6_N7,LTC2498_P8_N9,LTC2498_P10_N11,LTC2498_P12_N13,LTC2498_P14_N15};

// Initialize SPI buses
void init_spi_bus(){
    spi_bus_config_t buscfg1 = {
        .miso_io_num = MISO_PIN1,
        .mosi_io_num = MOSI_PIN1,
        .sclk_io_num = SCLK_PIN1,
        .max_transfer_sz = MAX_TRANSFER_SZ
    };
    spi_bus_config_t buscfg2 = {
        .miso_io_num = MISO_PIN2,
        .mosi_io_num = MOSI_PIN2,
        .sclk_io_num = SCLK_PIN2,
        .max_transfer_sz = MAX_TRANSFER_SZ
    };
    esp_err_t ret= spi_bus_initialize(SPI2_HOST, &buscfg1, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(SPIBUS, "Failed to initialize spi bus: 1");
    }
    ret= spi_bus_initialize(SPI3_HOST, &buscfg2, 0) ;
    if (ret != ESP_OK) {
        ESP_LOGE(SPIBUS, "Failed to initialize spi bus: 2");
    }
    ESP_LOGI(SPIBUS, "SPI buses initialized");
}

// Main application
void setup(){
    init_network();
    init_spi_bus();
    init_adc(&left_sensor,SPI2_HOST,ADC_CLOCK_SPEED, ADC_MODE,ADC1_CS_PIN, QUEUE_SIZE,'l');
    init_adc(&right_sensor,SPI2_HOST,ADC_CLOCK_SPEED, ADC_MODE,ADC1_CS_PIN, QUEUE_SIZE,'r');
    init_adc(&pressure_sensor,SPI2_HOST,ADC_CLOCK_SPEED, ADC_MODE,ADC2_CS_PIN, QUEUE_SIZE,'p');
    init_MC33996(&actuator,SPI2_HOST,ADC_CLOCK_SPEED, ADC_MODE,ACTUATOR_CS_PIN, QUEUE_SIZE);
    init_drv8311(&motor_driver,SPI3_HOST,ADC_CLOCK_SPEED, ADC_MODE,MOTOR_CS_PIN, QUEUE_SIZE);
    init_circular_buffer(&network_upload_buffer, BUFFER_SIZE);
    // esp_log_level_set("*", ESP_LOG_WARN);
}

// Main function
void app_main(void){
    bus1_sem=xSemaphoreCreateMutex();
    bus2_sem=xSemaphoreCreateMutex();
    socket_mutex1=xSemaphoreCreateMutex();
    socket_mutex2=xSemaphoreCreateMutex();
    network_sem=xSemaphoreCreateBinary();
    setup();
    xSemaphoreGive(network_sem);
    xTaskCreate(set_connection, "set_connection", 2048, NULL, 5, NULL);
    xTaskCreate(read_adc, "read_right_limbs", 3072, &right_sensor, 1, NULL);
    xTaskCreate(read_adc, "read_left_limbs",3072,&left_sensor,1,NULL);
    xTaskCreate(read_adc, "read_pressure",3072, &pressure_sensor, 1, NULL);
    xTaskCreate(network_upload, "send_data", 3072, NULL, 2, NULL);
    xTaskCreate(network_download, "receive_data", 3072, NULL, 2, NULL);
    xTaskCreate(write_actuator, "solenoid",3072, NULL, 1, NULL);
    xTaskCreate(motor_control_task, "motor_control", 3072, NULL, 3, NULL);
}
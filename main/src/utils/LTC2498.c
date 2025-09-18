#include "LTC2498.h"
#include <string.h>

void init_adc(LTC2498* device){
    spi_device_interface_config_t devcfg = {
                    .clock_speed_hz = device->clock_speed_hz,
                    .mode = device->mode,
                    .spics_io_num =device->cs_pin,
                    .queue_size = device->queue_size
    };
    esp_err_t ret= spi_bus_add_device(SPI2_HOST, &devcfg, &device->devhandler);
    if (ret != ESP_OK) {
        ESP_LOGE("ADC", "Failed to add SPI device: %d", ret);
    }
}

void LTC2498_read(LTC2498* device){

    device->tx_buffer=(device->adc_high_command<<24)|(device->adc_low_command<<16);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 32; // bits
    t.tx_buffer = &device->tx_buffer;
    t.rx_buffer = &device->rx_buffer;
    esp_err_t ret=spi_device_transmit(device->devhandler, &t);
    if (ret != ESP_OK) {
        ESP_LOGE("ADC", "Failed to read data: %d", ret);
    }
}
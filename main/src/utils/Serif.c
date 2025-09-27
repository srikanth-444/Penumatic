#include "Serif.h"
char* TAG ="Serif";

void init_device(Serif* device){
    spi_device_interface_config_t devcfg = {
    .clock_speed_hz = device->clock_speed_hz,
    .mode = device->mode,
    .spics_io_num =device->cs_pin,
    .queue_size = device->queue_size,
    };
    spi_bus_add_device(mc33996_device->bus, &devcfg, &device->devhandler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %d", ret);
    }
}
void send_data(Serif* device){
    
        
        memset(&device->t, 0, sizeof(device->t));
        t.length = device->buff_len*8
        t.tx_buffer = device->tx_buffer;
        t.rx_buffer = device->rx_buffer;
        esp_err_t ret=spi_device_transmit(device->devhandler, &t);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read data: %d", ret);
        }
}
void destroy_serif(Serif* device){
    if(!device) return;
    free(device->tx_buffer)
    free(device->rx_buffer)
    free(device)
}

Serif* create_serif(spi_host_device_t bus, uint32_t clk, uint8_t mode, uint8_t qsize, uint8_t buff_len) {
    Serif* dev = malloc(sizeof(Serif));
    if (!dev) return NULL;
    dev->buff_len = buff_len;
    dev->tx_buffer = malloc(buff_len*sizeof(uint8_t));
    dev->rx_buffer = malloc(buff_len*sizeof(uint8_t));
    dev->clock_speed_hz = clk;
    dev->mode = mode;
    dev->queue_size = qsize;
    dev->bus = bus;
    dev->init = init_device;
    dev->send = send_data;
    dev->free_t = destroy_serif;
    return dev;
}
#include "LTC2498.h"
#include <string.h>
static spi_transaction_t t;
void init_adc(LTC2498* device,spi_host_device_t bus,uint32_t clk, uint8_t mode, uint8_t qsize){
    uint8_t buff_len=4;
    device->serif=create_serif(bus,clk,mode,qsize,buff_len);
    device->command[1]=LTC2498_SPEED_1X;
    device->command[2]=0x00;
    device->command[3]=0x00;
}

void LTC2498_read(LTC2498* device){
    gpio_set_direction(device->serif->cs_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(device->serif->cs_pin, 0);
    esp_rom_delay_us(10);
    bool data_ready=(gpio_get_level(MISO_PIN) == 0); 
    gpio_set_level(device->serif->cs_pin, 1);
    if(data_ready){
        memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
        device->serif->send(Device->serif);
        memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
    }
    
}
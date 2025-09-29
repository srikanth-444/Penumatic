#include "LTC2498.h"



void init_adc(LTC2498* device,spi_host_device_t bus,uint32_t clk, uint8_t mode, uint8_t qsize,char type){
    uint8_t buff_len=sizeof(device->command);
    device->serif=create_serif(bus,clk,mode,qsize,buff_len);
    device->command[1]=LTC2498_SPEED_1X;
    device->command[2]=(uint8_t) type;
    device->command[3]=0x00;
}

bool LTC2498_read(LTC2498* device){
    //setup
    gpio_set_direction(MISO_PIN, GPIO_MODE_INPUT); 
    gpio_set_direction(device->serif->cs_pin, GPIO_MODE_OUTPUT);
    //checkdata
    gpio_set_level(device->serif->cs_pin, 0);
    esp_rom_delay_us(10);
    bool data_ready=(gpio_get_level(MISO_PIN) == 0); 
    gpio_set_level(device->serif->cs_pin, 1);
    //spi transaction
    if(data_ready){
        memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
        device->serif->send(device->serif);
        memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
        return true;
    }
    return false;
    
}
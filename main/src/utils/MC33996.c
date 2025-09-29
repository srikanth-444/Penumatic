#include "MC33996.h"

void init_MC33996(MC33996* device,spi_host_device_t bus,uint32_t clk, uint8_t mode, uint8_t qsize){
    uint8_t buff_len=sizeof(device->command);
    device->serif=create_serif(bus,clk,mode,qsize,buff_len);
}

void MC33996_enable_open_load(MC33996* device){
    device->command[0] = OPEN_LOAD_TEST; // Example: ON/OFF command bits
    device->command[1] = ALL_OFF;       // lower 16 bits, high byte
    device->command[2] = ALL_OFF;       // lower 16 bits, low byte
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void MC33996_enable_sfpd_therm(MC33996* device){
    device->command[0] = SFPD;
    device->command[1] = ALL_ON;
    device->command[2] = ALL_ON;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void MC33996_enable_pwm(MC33996* device){
    device->command[0] = PWM;
    device->command[1] = ALL_OFF;       // lower 16 bits, high byte
    device->command[2] = ALL_OFF; 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len); 
}
void MC33996_set_pwm_config(MC33996* device){
    device->command[0] = AND_OR;
    device->command[1] = ALL_OFF;       // lower 16 bits, high byte
    device->command[2] = ALL_OFF; 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void MC33996_set_reset(MC33996* device){
    device->command[0] = RESET;
    device->command[1] = ALL_OFF;       // lower 16 bits, high byte
    device->command[2] = ALL_OFF; 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len); 
}
void MC33996_update(MC33996* device){
    device->command[0] = ON_OFF;
    device->command[1] =    (device->r8 << 0) | 
                            (device->r7 << 1) | 
                            (device->r6 << 2) | 
                            (device->r5 << 3) | 
                            (device->r4 << 4) | 
                            (device->r3 << 5) | 
                            (device->r2 << 6) | 
                            (device->r1 << 7);       
    device->command[2] =    (device->l1 << 0) | 
                            (device->l2 << 1) | 
                            (device->l3 << 2) | 
                            (device->l4 << 3) | 
                            (device->l5 << 4) | 
                            (device->l6 << 5) | 
                            (device->l7 << 6) | 
                            (device->l8 << 7); 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}

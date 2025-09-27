#include "MC33996.h"

void init_actuator(MC33996* device,spi_host_device_t bus,uint32_t clk, uint8_t mode, uint8_t qsize){
    uint8_t buff_len=3;
    device->serif=create_serif(bus,clk,mode,qsize,buff_len);
}

void enable_open_load(MC33996* device){
    device->command[0] = OPEN_LOAD_TEST; // Example: ON/OFF command bits
    device->command[1] = ALL_OFF;       // lower 16 bits, high byte
    device->command[2] = ALL_OFF;       // lower 16 bits, low byte
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(Device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void enable_sfpd_therm(MC33996* device){
    device->command[0] = SFPD;
    device->command[1] = ALL_ON;
    device->command[2] = ALL_ON;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(Device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void enable_pwm(MC33996* device){
    device->command[0] = PWM;
    device->command[1] = ALL_OFF;       // lower 16 bits, high byte
    device->command[2] = ALL_OFF; 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(Device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len) 
}
void set_pwm_config(MC33996* device){
    device->command[0] = AND_OR;
    device->command[1] = ALL_OFF;       // lower 16 bits, high byte
    device->command[2] = ALL_OFF; 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(Device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len) 
}
void set_reset(MC33996* device){
    device->command[0] = RESET;
    device->command[1] = ALL_OFF;       // lower 16 bits, high byte
    device->command[2] = ALL_OFF; 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(Device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len) 
}
void update(MC33996* device){
    device->command[0] = ON_OFF;
    device->command[1] =    (r8 << 0) | 
                            (r7 << 1) | 
                            (r6 << 2) | 
                            (r5 << 3) | 
                            (r4 << 4) | 
                            (r3 << 5) | 
                            (r2 << 6) | 
                            (r1 << 7);       
    device->command[2] =    (l1 << 0) | 
                            (l2 << 1) | 
                            (l3 << 2) | 
                            (l4 << 3) | 
                            (l5 << 4) | 
                            (l6 << 5) | 
                            (l7 << 6) | 
                            (l8 << 7); 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(Device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len) 
}

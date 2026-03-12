#include "DRV8311.h"
void DRV8311_status(DRV8311* device){
    device->command[0] = READ_CMD | DEV_STS1; 
    device->command[1] = 0x00;
    device->command[2] = 0x00;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void DRV8311_SYS_CTRL(DRV8311* device){
    device->command[0] = WRITE_CMD | SYS_CTRL; 
    device->command[1] = 0x00;
    device->command[2] = 0x00; 
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void DRV8311_temperature(DRV8311* device){
    device->command[0] = READ_CMD | OT_STS; 
    device->command[1] = 0x00;
    device->command[2] = 0x00;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void DRV8311_enable_pwmg(DRV8311* device){
    device->command[0] = WRITE_CMD | PWMG_CTRL; 
    device->command[1] = 0x04;
    device->command[2] = 0x00; // Enable PWMG
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void DRV8311_set_Period(DRV8311* device){
    device->command[0] = WRITE_CMD | PWMG_PERIOD; 
    device->command[1] = (device->pwm_period >> 8) & 0xFF;
    device->command[2] = device->pwm_period & 0xFF;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void DRV8311_set_Duty_A(DRV8311* device){
    device->command[0] = WRITE_CMD | PWMG_A_DUTY; 
    device->command[1] = (device->pwm_duty_a >> 8) & 0xFF;
    device->command[2] = device->pwm_duty_a & 0xFF;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void DRV8311_set_Duty_B(DRV8311* device){
    device->command[0] = WRITE_CMD | PWMG_B_DUTY; 
    device->command[1] = (device->pwm_duty_b >> 8) & 0xFF;
    device->command[2] = device->pwm_duty_b & 0xFF;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void DRV8311_set_Duty_C(DRV8311* device){
    device->command[0] = WRITE_CMD | PWMG_C_DUTY; 
    device->command[1] = (device->pwm_duty_c >> 8) & 0xFF;
    device->command[2] = device->pwm_duty_c & 0xFF;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
}
void DRV8311_update_state(DRV8311* device){
    device->command[0] = WRITE_CMD | PWM_STATE;
    device->command[1] = (device->States_in_order[device->state_index] >> 8)&0xFF;
    device->command[2] = device->States_in_order[device->state_index]&0xFF;
    memcpy(device->serif->tx_buffer, device->command, device->serif->buff_len);
    device->serif->send(device->serif);
    memcpy(device->response, device->serif->rx_buffer,device->serif->buff_len);
    device->state_index = (device->state_index + 1) % 6; // Cycle through 6 states
}
void init_drv8311(DRV8311* device,spi_host_device_t bus,uint32_t clk, uint8_t mode, uint8_t cs_pin, uint8_t qsize){
    uint8_t buff_len=sizeof(device->command);
    device->pwm_period=PWM_PRD_OUT; 
    device->pwm_duty_a=PWM_DUTY_OUTA;  
    device->pwm_duty_b=PWM_DUTY_OUTB;
    device->pwm_duty_c=PWM_DUTY_OUTC;
    device->States_in_order=MOTOR_STATE_ORDER;
    device->state_index=0;
    device->serif=create_serif(bus,clk,mode,qsize,cs_pin,buff_len);
    device->serif->init(device->serif);
    DRV8311_SYS_CTRL(device);
    DRV8311_enable_pwmg(device);
    DRV8311_set_Period(device);
    DRV8311_set_Duty_A(device);
    DRV8311_set_Duty_B(device);
    DRV8311_set_Duty_C(device);
}
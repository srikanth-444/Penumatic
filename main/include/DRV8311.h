#ifndef DRV8311_H
#define DRV8311_H
#include "Constant.h"
#include "Serif.h"

#define DEV_STS1        0x00
#define OT_STS          0x04
#define SUP_STS         0x05
#define DRV_STS         0x06
#define SYS_STS         0x07
#define PWM_SYNC_PRD    0x0C
#define FLT_MODE        0x10
#define SYSF_CTRL       0x12
#define DRVF_CTRL       0x13
#define FLT_TCTRL       0x16
#define FLT_CLR         0x17
#define PWMG_PERIOD     0x18 
#define PWMG_A_DUTY     0x19
#define PWMG_B_DUTY     0x1A
#define PWMG_C_DUTY     0x1B 
#define PWM_STATE       0x1C 
#define PWMG_CTRL       0x1D 
#define PWM_CTRL1       0x20 
#define DRV_CTRL        0x22
#define CSA_CTRL        0x23 
#define SYS_CTRL        0x3F
#define WRITE_CMD       0x00
#define READ_CMD        0x80

#define PWM_DUTY_OUTA   0x0000
#define PWM_DUTY_OUTB   0x0000
#define PWM_DUTY_OUTC   0x0000
#define PWM_PRD_OUT     0x07D0 // 2000 in decimal for 20kHz with 40MHz clock


typedef struct{
    uint8_t command[3];
    uint8_t response[3];
    uint16_t pwm_period;
    uint16_t pwm_duty_a;
    uint16_t pwm_duty_b;
    uint16_t pwm_duty_c;
    uint16_t* States_in_order;
    uint8_t state_index;
    Serif* serif;
}DRV8311;

void init_drv8311(DRV8311* device,spi_host_device_t bus,uint32_t clk, uint8_t mode, uint8_t qsize);
void DRV8311_update_state(DRV8311* device);

#endif
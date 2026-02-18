#ifndef MC33996_H
#define MC33996_H
#include "Serif.h"
#include "Constant.h"
#define ALL_ON          0xFF
#define ALL_OFF         0x00
#define ON_OFF          0b00000000
#define OPEN_LOAD_TEST  0b00000100
#define SFPD            0b00001100
#define PWM             0b00010000
#define AND_OR          0b00010100
#define RESET           0b00011000
#define THER_VOLT_SHUT  0b00001000
#define THER_VOLT_RETRY 0b00001011
#define THERMAL_RETRY   0b00001010
#define VOLTAGE_RETRY   0b00001001

#define s15_on 0b10000000
#define s14_on 0b01000000
#define s13_on 0b00100000
#define s12_on 0b00010000
#define s11_on 0b00001000
#define s10_on 0b00000100
#define s9_on  0b00000010
#define s8_on  0b00000001
#define s7_on  0b10000000
#define s6_on  0b01000000
#define s5_on  0b00100000
#define s4_on  0b00010000
#define s3_on  0b00001000
#define s2_on  0b00000100
#define s1_on  0b00000010
#define s0_on  0b00000001

#define s15_off (~s15_on)
#define s14_off (~s14_on) 
#define s13_off (~s13_on) 
#define s12_off (~s12_on)
#define s11_off (~s11_on) 
#define s10_off (~s10_on)
#define s9_off  (~s9_on) 
#define s8_off  (~s8_on) 
#define s7_off  (~s7_on)  
#define s6_off  (~s6_on)  
#define s5_off  (~s5_on)  
#define s4_off  (~s4_on)  
#define s3_off  (~s3_on)  
#define s2_off  (~s2_on)  
#define s1_off  (~s1_on)  
#define s0_off  (~s0_on)  

typedef struct{
    uint8_t command[3];
    uint8_t response[3];
    Serif* serif;
    bool r1,r2,r3,r4,r5,r6,r7,r8,l1,l2,l3,l4,l5,l6,l7,l8;
}MC33996;

void init_MC33996(MC33996* device,spi_host_device_t bus,uint32_t clk, uint8_t mode, uint8_t cs_pin, uint8_t qsize);
void MC33996_enable_open_load(MC33996* device);
void MC33996_enable_sfpd_therm(MC33996* device);
void MC33996_enable_pwm(MC33996* device);
void MC33996_set_pwm_config(MC33996* device);
void MC33996_set_reset(MC33996* device);
void MC33996_update(MC33996* device);

#endif
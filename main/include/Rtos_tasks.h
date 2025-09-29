#ifndef RTOS_TASKS_H
#define RTOS_TASKS_H
#include "Constant.h"
#include "Network.h"
#include "LTC2498.h"
#include "MC33996.h"
#include "DRV8311.h"
#include "circularBuffer.h"
void read_adc(void* arg);
void network_upload(void* arg);
void network_download(void* arg); 
void write_actuator(void*arg);
void motor_control_task(void* arg);
#endif
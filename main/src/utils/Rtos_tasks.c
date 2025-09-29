#include "Rtos_tasks.h"

extern CircularBuffer network_upload_buffer;
extern SemaphoreHandle_t bus1_sem;
extern SemaphoreHandle_t bus2_sem;
extern SemaphoreHandle_t socket_mutex;
extern LTC2498 differential_sensor;
extern LTC2498 pressure_sensor;
extern MC33996 actuator;
extern DRV8311 motor_driver;

void read_adc(void* arg){
    LTC2498 *adc = (LTC2498 *)arg;
    while(1){
        xSemaphoreTake(bus1_sem, portMAX_DELAY); 
        if(LTC2498_read(adc)){
            //righting to network buffer
            uint64_t data_to_send= ((uint64_t)adc->serif->tx_buffer[0] << 56|
                                    (uint64_t)adc->serif->tx_buffer[1] << 48|
                                    (uint64_t)adc->serif->tx_buffer[2] << 40|
                                    (uint64_t)adc->serif->tx_buffer[3] << 32|
                                    (uint64_t)adc->serif->rx_buffer[0] << 24|
                                    (uint64_t)adc->serif->rx_buffer[1] << 16|
                                    (uint64_t)adc->serif->rx_buffer[2] << 8 |
                                    (uint64_t)adc->serif->rx_buffer[3]);
            while(!cb_push(&network_upload_buffer, data_to_send)){
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
        }
        xSemaphoreGive(bus1_sem);
        vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);  
        
    }
}

void write_actuator(void*arg){
    while(1){
        xSemaphoreTake(bus2_sem, portMAX_DELAY);
        MC33996_update(&actuator);
        xSemaphoreGive(bus1_sem);
        vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);     
    }
}

void network_upload(void* arg) {
    while (1){
        if(network_upload_buffer.count > 0) {
            uint64_t data;
            ESP_LOGI("NETWORK", "Sending data from buffer, count: %d", network_upload_buffer.count);
            xSemaphoreTake(socket_mutex, portMAX_DELAY);
            if(cb_pop(&network_upload_buffer, &data) == 0) {
                send_over_net((const char*)&data, sizeof(data));
            }
            xSemaphoreGive(socket_mutex);
        }
        vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);
    }
}

void network_download(void* arg) {
    while (1){
            uint64_t command;
            xSemaphoreTake(socket_mutex, portMAX_DELAY);
            receive_over_net((char*)&command, sizeof(command));
            xSemaphoreGive(socket_mutex);
            differential_sensor.command[0]=(command >> 56) & 0xFF;
            pressure_sensor.command[0]=(command >> 48) & 0xFF;
            actuator.r1=(command >> 47) & 1; 
            actuator.r2=(command >> 46) & 1; 
            actuator.r3=(command >> 45) & 1; 
            actuator.r4=(command >> 44) & 1; 
            actuator.r5=(command >> 43) & 1; 
            actuator.r6=(command >> 42) & 1; 
            actuator.r7=(command >> 41) & 1; 
            actuator.r8=(command >> 40) & 1; 
            actuator.l1=(command >> 39) & 1; 
            actuator.l2=(command >> 38) & 1; 
            actuator.l3=(command >> 37) & 1; 
            actuator.l4=(command >> 36) & 1; 
            actuator.l5=(command >> 35) & 1; 
            actuator.l6=(command >> 34) & 1; 
            actuator.l7=(command >> 33) & 1; 
            actuator.l8=(command >> 32) & 1;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void motor_control_task(void* arg) {
    const TickType_t xPeriod=pdMS_TO_TICKS(100);
    TickType_t xLastWakeTime=xTaskGetTickCount();
    while (1) {
        DRV8311_update_state(&motor_driver);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}


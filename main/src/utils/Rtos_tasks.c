#include "Rtos_tasks.h"

extern CircularBuffer network_upload_buffer;
extern SemaphoreHandle_t bus1_sem;
extern SemaphoreHandle_t bus2_sem;
extern SemaphoreHandle_t socket_mutex1;
extern SemaphoreHandle_t socket_mutex2;
extern SemaphoreHandle_t network_sem;
extern LTC2498 left_sensor;
extern LTC2498 right_sensor;
extern LTC2498 pressure_sensor;
extern MC33996 actuator;
extern DRV8311 motor_driver;
static char* TAG="RTOS_TASKS";

void read_adc(void* arg){
    LTC2498 *adc = (LTC2498 *)arg;
    while(1){
        xSemaphoreTake(bus1_sem, portMAX_DELAY); 
        ESP_LOGI( TAG, "bus1_sem taken by read_adc");
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
            ESP_LOGI(TAG, "data_sending %llu", data_to_send);// as unsigned 8-bit
            while(!cb_push(&network_upload_buffer, data_to_send)){
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        }
        ESP_LOGI( TAG, "bus1_sem given by read_adc");
        xSemaphoreGive(bus1_sem);
        
        vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);  
        
    }
}

void write_actuator(void*arg){
    while(1){
        
        xSemaphoreTake(bus2_sem, portMAX_DELAY);
        ESP_LOGI( TAG, "bus2_sem taken by write_actuator");
        MC33996_update(&actuator);
        ESP_LOGI( TAG, "bus2_sem given by write_actuator");
        xSemaphoreGive(bus2_sem);
        
        vTaskDelay(DATA_ACCUSATION_FREQUENCY_MS / portTICK_PERIOD_MS);     
    }
}

void network_upload(void* arg) {
    while (1){
        if(network_upload_buffer.count > 0) {
            uint64_t data;
            // ESP_LOGI(NETWORK, "Sending data from buffer, count: %d", network_upload_buffer.count);
            xSemaphoreTake(socket_mutex1, portMAX_DELAY);
            ESP_LOGI(TAG, "socket_mutex taken in upload");
            if(cb_pop(&network_upload_buffer, &data)) {
                ESP_LOGI(TAG,"data poped %d",data);
                if(send_over_net((const char*)&data, sizeof(data))<0){
                    ESP_LOGW(TAG,"network_sem given by upload");
                    xSemaphoreGive(network_sem);
                    
                }
            }
            ESP_LOGI(TAG, "socket_mutex given in upload");
            xSemaphoreGive(socket_mutex1);
            
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void network_download(void* arg) {
    while (1){
            
            uint64_t command;
            xSemaphoreTake(socket_mutex2, portMAX_DELAY);
            ESP_LOGI(TAG, "socket_mutex taken by download");
            if (receive_over_net((char*)&command, sizeof(command))<0){
                ESP_LOGI(TAG,"network_sem given by download");
                xSemaphoreGive(network_sem);
                
            }
            ESP_LOGI(TAG, "socket_mutex given by download");
            xSemaphoreGive(socket_mutex2);
            right_sensor.command[0]=(command >> 56) & 0xFF;
            left_sensor.command[0] =(command>>48)& 0xFF;
            pressure_sensor.command[0]=(command >> 40) & 0xFF;
            actuator.r1=(command >> 15) & 1; 
            actuator.r2=(command >> 14) & 1; 
            actuator.r3=(command >> 13) & 1; 
            actuator.r4=(command >> 12) & 1; 
            actuator.r5=(command >> 11) & 1; 
            actuator.r6=(command >> 10) & 1; 
            actuator.r7=(command >> 9) & 1; 
            actuator.r8=(command >> 8) & 1; 
            actuator.l1=(command >> 7) & 1; 
            actuator.l2=(command >> 6) & 1; 
            actuator.l3=(command >> 5) & 1; 
            actuator.l4=(command >> 4) & 1; 
            actuator.l5=(command >> 3) & 1; 
            actuator.l6=(command >> 2) & 1; 
            actuator.l7=(command >> 1) & 1; 
            actuator.l8=(command >> 0) & 1;
            ESP_LOGI(TAG, "right sensor command[0] = %u", right_sensor.command[0]);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void motor_control_task(void* arg) {
    
    const TickType_t xPeriod=pdMS_TO_TICKS(100);
    TickType_t xLastWakeTime=xTaskGetTickCount();
    while (1) {
        xSemaphoreTake(bus2_sem, portMAX_DELAY);
        ESP_LOGI( TAG, "bus2_sem taken by motor_control_task");
        DRV8311_update_state(&motor_driver);
        ESP_LOGI( TAG, "bus2_sem given by motor_control_task");
        xSemaphoreGive(bus2_sem);
        
        vTaskDelayUntil(&xLastWakeTime, xPeriod);  
    }
}
void set_connection(void* arg){
    
    while(1){
        xSemaphoreTake(network_sem, portMAX_DELAY);
        ESP_LOGI(TAG, "network_sem taken by set_connection");
        xSemaphoreTake(bus1_sem, portMAX_DELAY);
        ESP_LOGI( TAG, "bus1_sem taken by set_connection");
        xSemaphoreTake(bus2_sem, portMAX_DELAY);
        ESP_LOGI( TAG, "bus2_sem taken by set_connection");
        xSemaphoreTake(socket_mutex1, portMAX_DELAY);
        ESP_LOGI(TAG, "socket_mutex1 taken by set_connection");
        xSemaphoreTake(socket_mutex2, portMAX_DELAY);
        ESP_LOGI(TAG, "socket_mutex2 taken by set_connection");
        while(!connect_to_server()){
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Connection to server established");
        ESP_LOGI(TAG, "socket_mutex2 given by set_connection");
        xSemaphoreGive(socket_mutex2);
        ESP_LOGI(TAG, "socket_mutex1 given by set_connection");
        xSemaphoreGive(socket_mutex1);
        ESP_LOGI( TAG, "bus2_sem given by set_connection");
        xSemaphoreGive(bus2_sem);
        ESP_LOGI( TAG, "bus1_sem given by set_connection");
        xSemaphoreGive(bus1_sem);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
    }
}


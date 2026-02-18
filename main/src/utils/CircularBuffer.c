#include "CircularBuffer.h"
void init_circular_buffer(CircularBuffer *cb,uint8_t size) {
    cb->buffer = (uint64_t *)malloc(size*sizeof(uint64_t));  // allocate memory
    if (!cb->buffer) {
        // handle malloc failure
        ESP_LOGE(CIRCULAR_BUFF,"Couldn't create buffer");
        return;
    }
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->size = size;
    cb->mutex = xSemaphoreCreateMutex();
}
void cb_free(CircularBuffer *cb) {
    if (cb->buffer) {
        free(cb->buffer);
        cb->buffer = NULL;   // optional, prevents dangling pointer
    }
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->size = 0;
    
}
bool cb_push(CircularBuffer *cb, uint64_t data) {
    xSemaphoreTake(cb->mutex, portMAX_DELAY);
    if (cb->count == cb->size) {
        // Buffer is full
        ESP_LOGW(CIRCULAR_BUFF,"Buffer is full");
        xSemaphoreGive(cb->mutex);
        
        return false;
    }
    cb->buffer[cb->head] = data;
    cb->head = (cb->head + 1) % cb->size;
    cb->count++;
    xSemaphoreGive(cb->mutex);
    return true;
}
bool cb_pop(CircularBuffer *cb, uint64_t *data) {
    xSemaphoreTake(cb->mutex, portMAX_DELAY);
    if (cb->count == 0) {
        xSemaphoreGive(cb->mutex);
        ESP_LOGW(CIRCULAR_BUFF,"Buffer is empty");
        return false;
    }
    *data = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % cb->size;
    cb->count--;
    xSemaphoreGive(cb->mutex);
    return true;
}
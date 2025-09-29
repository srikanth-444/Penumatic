#ifndef Circular_H
#define Circular_H
#include "Constant.h"



  // Define the size of the circular buffer

typedef struct {
    uint64_t* buffer;
    uint8_t head;
    uint8_t tail;
    uint8_t count;
    uint8_t size;
    SemaphoreHandle_t mutex;
} CircularBuffer;

void init_circular_buffer(CircularBuffer *cb,uint8_t size);
void cb_free(CircularBuffer *cb);
int cb_push(CircularBuffer *cb, uint64_t data);
int cb_pop(CircularBuffer *cb, uint64_t* data);

#endif
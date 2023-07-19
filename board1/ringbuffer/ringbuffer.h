#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "mpu6050/mpu6050.h"

#define DYTE_TYPE float

typedef struct
{
    uint32_t head;
    uint32_t tail;
    uint32_t size;
    DYTE_TYPE *array;
} RingBuffer_Handle;

#define ringq_is_empty(q) (q->head == q->tail)
#define ringq_is_full(q) (((q->tail + 1) % q->size) == q->head)

int8_t RingBufInit(RingBuffer_Handle *ringb, DYTE_TYPE *array_ptr, uint32_t size);
int8_t RingBufFree(RingBuffer_Handle *ringb);
int8_t RingBufPush(RingBuffer_Handle *ringb, DYTE_TYPE data);
int8_t RingBufPoll(RingBuffer_Handle *ringb, DYTE_TYPE *val);
DYTE_TYPE RingBufIndex(RingBuffer_Handle *ringb, int32_t index);
void RingBufFullPush(RingBuffer_Handle *ringb, DYTE_TYPE data);
void RingBufDisplay(RingBuffer_Handle *ringb);

#ifdef __cplusplus
}
#endif

#endif

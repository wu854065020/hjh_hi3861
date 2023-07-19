/*
    环形缓冲区，通过头索引和尾索引来索引数组，可以很方便地将旧变量去除，塞入新变量
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuffer.h"

int8_t RingBufInit(RingBuffer_Handle *ringqp, DYTE_TYPE *array_ptr, uint32_t size)
{
    ringqp->array = array_ptr;
    ringqp->size = size;
    ringqp->head = 0;
    ringqp->tail = 0;
    return 0;
}

int8_t RingBufFree(RingBuffer_Handle *ringqp)
{
    hi_free(ringqp->array);
    return 0;
}

int8_t RingBufPush(RingBuffer_Handle *ringqp, DYTE_TYPE data)
{
    if (ringq_is_full(ringqp))
    {
        // printf("ringq is full.\n");
        return -2;
    }
    ringqp->array[ringqp->tail] = data;
    ringqp->tail = (ringqp->tail + 1) % ringqp->size;
    return 0;
}

int8_t RingBufPoll(RingBuffer_Handle *ringqp, DYTE_TYPE *val)
{
    if (ringq_is_empty(ringqp))
    {
        // printf("ringq is empty.\n");
        return -3;
    }
    *val = ringqp->array[ringqp->head];
    ringqp->head = (ringqp->head + 1) % ringqp->size;
    return 0;
}

DYTE_TYPE RingBufIndex(RingBuffer_Handle *ringb, int32_t index)
{
    if ((((ringb->tail - ringb->head) % ringb->size) > index) && index >= 0)
    {
        return ringb->array[(ringb->head + index) % ringb->size];
    }
}

void RingBufFullPush(RingBuffer_Handle *ringb, DYTE_TYPE data)
{
    if (ringq_is_full(ringb))
    {
        ringb->head = (ringb->head + 1) % ringb->size;
        ringb->array[ringb->tail] = data;
        ringb->tail = (ringb->tail + 1) % ringb->size;
    }
    else
    {
        RingBufPush(ringb, data);
    }
}

void RingBufDisplay(RingBuffer_Handle *ringqp)
{
    int i = 0;
    uint32_t head = ringqp->head;
    uint32_t tail = ringqp->tail;
    uint32_t size = ringqp->size;

    if (ringq_is_empty(ringqp))
    {
        // printf("ringq is empty.\n");
        return;
    }
    while (head != tail)
    {
        // printf("%04d ", ringqp->array[head]);
        i++;
        if (i == 5)
        {
            // printf("\n");
            i = 0;
        }
        head = (head + 1) % (size);
    }
    printf("\n");
    return;
}

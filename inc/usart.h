#pragma once

#include "main.h"
#include "cl_queue.h"

void USART_Config(void);

CL_QUEUE_DECL(usart1SendQueue);

static inline CL_Result_t Usart1_AddSendByte(volatile uint8_t in)
{
    return CL_QueueAdd(&usart1SendQueue, (void *)&in);
}

static inline CL_Result_t Usart1_PollSendByte(volatile uint8_t* out)
{
    return CL_QueuePoll(&usart1SendQueue, (void *)out);
}


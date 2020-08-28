#ifndef CANOPEN_FIFO_H
#define CANOPEN_FIFO_H

#include <stdint.h>
extern void canOpenFifoInit(void);
extern uint32_t canOpenFifoWrite(void* data, uint32_t data_len);
extern uint32_t canOpenFifoRead(void* data,uint32_t data_ken);
extern uint32_t canOpenFifoGetLen(void);

#endif


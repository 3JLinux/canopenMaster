#include "fifo.h"
#include "canopen_fifo.h"

//fifo参数
#define CANOPEN_BUF_LEN 512
FIFO_T ringCANopenBuf;
uint8_t canopen_buf[CANOPEN_BUF_LEN];


/*******************************************
 * 函数原型：	void canOpenFifoInit(void)
 * 功能：		fifo初始化
 * 输入：		void
 * 返回：		void
 * ****************************************/
void canOpenFifoInit(void)
{
	fifo_Create(&ringCANopenBuf, canopen_buf, CANOPEN_BUF_LEN);
}


/*******************************************
 * 函数原型：	uint32_t canOpenFifiWrite(void* data, uint32_t data_len)
 * 功能：		数据写入fifo
 * 输入：		void* data 数据指针
				int32_t data_len 数据长度
 * 返回：		写入的数据长度
 ******************************************/
 uint32_t canOpenFifoWrite(void* data, uint32_t data_len)
 {
	uint32_t len;
	len = fifo_Enqueue(&ringCANopenBuf, data, data_len);
	return len;
 }


/******************************************
 * 函数原型：	uint32_t canOpenFifoRead(void* data, uint32_t data_len)
 * 功能：		数据从fifo读出
 * 输入：		void* data 数据指针
				int32_t data_len 数据长度
 * 返回：		读出的数据长度
 *****************************************/
uint32_t canOpenFifoRead(void* data, uint32_t data_len)
{
	uint32_t len;
	len = fifo_Dequeue(&ringCANopenBuf, data, data_len);
	return len;
}


/*****************************************
 * 函数原型：	uint32_t canOpenFifoGetLen(void)
 * 功能：		获取fifo中数据长度
 * 输入：		void
 * 返回：		void
*****************************************/
uint32_t canOpenFifoGetLen(void)
{
	return get_fifo_used_length(&ringCANopenBuf);
}


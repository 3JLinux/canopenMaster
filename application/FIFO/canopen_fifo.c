#include "fifo.h"
#include "canopen_fifo.h"

//fifo����
#define CANOPEN_BUF_LEN 512
FIFO_T ringCANopenBuf;
uint8_t canopen_buf[CANOPEN_BUF_LEN];


/*******************************************
 * ����ԭ�ͣ�	void canOpenFifoInit(void)
 * ���ܣ�		fifo��ʼ��
 * ���룺		void
 * ���أ�		void
 * ****************************************/
void canOpenFifoInit(void)
{
	fifo_Create(&ringCANopenBuf, canopen_buf, CANOPEN_BUF_LEN);
}


/*******************************************
 * ����ԭ�ͣ�	uint32_t canOpenFifiWrite(void* data, uint32_t data_len)
 * ���ܣ�		����д��fifo
 * ���룺		void* data ����ָ��
				int32_t data_len ���ݳ���
 * ���أ�		д������ݳ���
 ******************************************/
 uint32_t canOpenFifoWrite(void* data, uint32_t data_len)
 {
	uint32_t len;
	len = fifo_Enqueue(&ringCANopenBuf, data, data_len);
	return len;
 }


/******************************************
 * ����ԭ�ͣ�	uint32_t canOpenFifoRead(void* data, uint32_t data_len)
 * ���ܣ�		���ݴ�fifo����
 * ���룺		void* data ����ָ��
				int32_t data_len ���ݳ���
 * ���أ�		���������ݳ���
 *****************************************/
uint32_t canOpenFifoRead(void* data, uint32_t data_len)
{
	uint32_t len;
	len = fifo_Dequeue(&ringCANopenBuf, data, data_len);
	return len;
}


/*****************************************
 * ����ԭ�ͣ�	uint32_t canOpenFifoGetLen(void)
 * ���ܣ�		��ȡfifo�����ݳ���
 * ���룺		void
 * ���أ�		void
*****************************************/
uint32_t canOpenFifoGetLen(void)
{
	return get_fifo_used_length(&ringCANopenBuf);
}


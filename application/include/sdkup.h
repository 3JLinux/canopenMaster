/*
 * main.h
 *
 *  Created on: 2020��8��7��
 *      Author: yaodidi
 */

#ifndef SDKUPDATA_MAIN_H_
#define SDKUPDATA_MAIN_H_

#include <stdint.h>

#define DEVICE_NUM	20
#define UPDATE_PACKAGE_LEN 128
#define SEND_BUFFER_HEAD_LEN 16
#define SEND_BUFFER_LEN 136
#define FRAME_HEAD_SKD	0x5a
#define FRAME_HEAD_DATA	0x5b
#define UP_ID_SEN_FIRST	0x181
#define UP_ID_RCV_FIRST	0x201
#define UP_ID_RCV_LAST	0x27F
#define UP_ID_SEN_LAST 0x1FF
#define 	SDK_MAC_LEN 4
#define 	TRAVERSE_LEN 2

//#define SDK_DATA_B(k)	(SDK_DATA_B ## k)
#define VERIFY_FAILED		-1
//#define VERIFY_FRAME_ERR 	0
#define VERIFY_WAIT_FRAME	0
#define	VERIFY_SUCCEED		1
#define VERIFY_WAIT_TIME	50




#define CLEAR_PTR_ARRAY(K)\
for(uint8_t i=0;i<8;i++)\
{\
	*(*((K)+i)) = 0;\
}


typedef struct
{
	uint16_t	id;
	uint8_t		rcv_flag;

	union {
		uint8_t		data[8];
		struct{
			uint8_t data0;		// ֡ͷ��ʶ/�Ͱ�λ�ְ�����
			uint8_t data1;		// ���ݷ���/�߰�λ�ְ�����
			uint8_t data2;		// �ӻ�����/�汾��/ID�Ͱ�λ
			uint8_t data3;		// id�߰�λ
			uint8_t data4;		// �����ȵͰ�λ
			uint8_t data5;		// �����ȵ�16Ϊ
			uint8_t data6;		// �����ȵ�24λ
			uint8_t data7;		// �����ȸ�32λ
		}bit;
	}un;
}SDKbuffer_st;


typedef enum
{
	CMD_NOMODULE = 0x00,
	CMD_REGISTER = 0x01,
	CMD_HEARTBEAT = 0x02,
	CMD_GET_MODULE = 0x03,
	CMD_GET_DATA = 0x04,
	CMD_SET_DATA = 0x05,
	CMD_SEND_MSG = 0x06,
	CMD_AUTO_LOAD_DATA = 0x07,
	CMD_RECIVE_FAILED = 0X08,
	CMD_RECIVE_SUCCEED = 0X09,
	CMD_UPGRADE = 0xf0,
	CMD_UPGRADE_REQUEST = 0xf1,
	CMD_UPGRADE_PK = 0xf2,
	CMD_UPGRADE_PKRCV = 0xf3,
	CMD_UPGRADE_FINISH = 0xf4,
}Command;


typedef enum
{
	DISCONNECT = 0,
	PARSE_FIRST_HEADER_FRAME = 1,
	PARSE_SECOND_HEADER_FRAME = 2,
	CMD_IS_SET_DATA_HANDLER = 3,
	CMD_IS_REGISTER_HANDLER = 4,
	///
	REGISTER_SUCCEED = 8,
	///
	POST_BACK_MSG = 5,
	COMMIT_DATA_FRAME = 6,
	JUDGE_NEED_TO_UPDATE = 7,
	UPDATING = 11,

	F0_SEND = 12,
	F1_RECV = 13,
	UPDATE_SEND_PACKAGE = 14,
	UPDATE_RESEND_PACKAGE = 15,
	UPDATE_WAIT_POST_BACK = 16,
	UPDATE_SUCCEED = 17,
	UPDATE_FAILED = 18
}CanClientState;


typedef struct
{

	uint16_t wdevice_id[DEVICE_NUM];	// �������豸ID
	uint8_t wdevice_mac[DEVICE_NUM][SDK_MAC_LEN];	// �������豸��ӦMAC��ַ
	uint8_t wdevice_num;							// �������豸����
	union{
		uint8_t byte16[16];
		struct {
			uint8_t device[TRAVERSE_LEN];			// �豸����
			uint8_t ver;							// SDK �汾��
			uint8_t jfkj1;
			uint8_t jfkj2;
			uint8_t jfkj3;
			uint8_t jfkj4;
			uint8_t jfkj5;
		}type;
	}Dhead;

	struct{
		uint32_t data_len;			// SDK���ݳ���
		uint32_t data_len_offset;	// SDK�����Է��ͳ���
		CanClientState state;		// ����״̬
		Command slave_cmd;			// �ӻ���������
		uint16_t package_sum;		// �ְ�����
		uint16_t buffer_offset;		// ��
		uint16_t timeout_count;		// ��ʱʱ�����
		uint16_t packageCRC;		// CRCУ��
		uint8_t package_resend_count;	// �ط�����
		uint8_t package_timeount_count;	// ��ʱ�ط��Ĵ���
	}DMsg;

}UPdata_st;

extern SDKbuffer_st SDKcan_buffer;
extern void SDK_process_pthread(void);




#endif /* SDKUPDATA_MAIN_H_ */

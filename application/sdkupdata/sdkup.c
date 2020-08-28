/*
 * sdkup.c
 *
 *  Created on: 2020��8��18��
 *      Author: yaodidi
 */
/*
 * main.c
 *
 *  Created on: 2020��8��7��
 *      Author: yaodidi
 */

#include "sdkup.h"
#include <pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include <time.h>
#include"CRC.h"
#include "memb.h"
#include "endnodemem.h"	// �˴�Ϊ����������ͷ�ļ�
#include "canfestival.h"  // �˴�Ϊ���canfestival��ͷ�ļ�
#include "TestMaster.h"
#include "sysprintf.h"
#include "WriteLog.h"

extern  uint32_t TestMaster_obj1800_COB_ID_used_by_PDO;	/* 384 */
extern  uint32_t TestMaster_obj1401_COB_ID_used_by_PDO;
UPdata_st SDK = {{0},{0},0,{0},{0,0,UPDATING,CMD_NOMODULE,0,0,0,0,0,0}};
SDKbuffer_st SDKcan_buffer;
uint8_t send_buffer_head[SEND_BUFFER_HEAD_LEN];
uint8_t send_buffer[SEND_BUFFER_LEN];

uint8_t *SDK_Data_ptr[8]={&SDK_DATA_B0,&SDK_DATA_B1,&SDK_DATA_B2,&SDK_DATA_B3,&SDK_DATA_B4,&SDK_DATA_B5,&SDK_DATA_B6,&SDK_DATA_B7};
uint8_t *SDK_Data_Rcv_ptr[8]={&SDK_RCV_B0,&SDK_RCV_B1,&SDK_RCV_B2,&SDK_RCV_B3,&SDK_RCV_B4,&SDK_RCV_B5,&SDK_RCV_B6,&SDK_RCV_B7};

/*******************************************
 * ����ԭ�ͣ�		void getSDKmessage(void)
 * ���ܣ�		��ȡSDK����Ϣ
 * ���룺		void
 * ���أ�		void
 * ****************************************/
void getSDKmessage(void)
{
	struct stat st;
	stat("./testbin.bin",&st);
	SDK.DMsg.data_len = (uint32_t)(st.st_size) - 16;
	SDK.DMsg.package_sum = (SDK.DMsg.data_len + UPDATE_PACKAGE_LEN-1) / UPDATE_PACKAGE_LEN;
	printf("*********SDK.DMsg.data_len=%d**********\n***********SDK.DMsg.package_sum=%d*************\n",SDK.DMsg.data_len,SDK.DMsg.package_sum);
}

/*******************************************
 * ����ԭ�ͣ�		getSDKdata(void *prt,FILE *fp,uint8_t readlen)
 * ���ܣ�		��ȡSDK����
 * ���룺		*ptr		��ȡ���ݴ������
 * 			*fp			�򿪵��ļ�����ָ��
 * 			readlen		��ȡ���ݳ���
 * ���أ�		void
 * ****************************************/
void getSDKdata(void *ptr,FILE *fp,uint8_t readlen)
{
	fread(ptr,sizeof(uint8_t),readlen,fp);
}

/*******************************************
 * ����ԭ�ͣ�		void setSDKhead(void *ptr,uint8_t num)
 * ���ܣ�		д��̼����Ϳ�ʼ֡֡ͷ
 * ���룺		ptr			SDK��Ϣ
 * 			num			ID
 * ���أ�		void
 * ****************************************/
void setSDKhead(void *ptr,uint8_t num)
{
	uint16_t crc;
	UPdata_st *p;
	p = (UPdata_st*)ptr;

	send_buffer_head[0] = FRAME_HEAD_SKD;									// �̼���ʼ����֡ ֡ͷָʾ
	send_buffer_head[1] = (uint8_t)CMD_UPGRADE;					// �̼���ʼ����֡ ���ݷ���ָʾ �����·�
	send_buffer_head[2] = (uint8_t)(p->wdevice_id[num]&0xff);
	send_buffer_head[3] = (uint8_t)((p->wdevice_id[num]>>8)&0xff);

	send_buffer_head[4] = (uint8_t)( p->DMsg.data_len  & 0x000000ff);
	send_buffer_head[5] = (uint8_t)(( p->DMsg.data_len & 0x0000ff00)>>8);
	send_buffer_head[6] = (uint8_t)(( p->DMsg.data_len & 0x00ff0000)>>16);
	send_buffer_head[7] = (uint8_t)(( p->DMsg.data_len & 0xff000000)>>24);

	send_buffer_head[8] =	(uint8_t)( p->DMsg.package_sum & 0x00ff);
	send_buffer_head[9] =	(uint8_t)((p->DMsg.package_sum & 0xff00)>>8);

	send_buffer_head[10] = (uint8_t)(p->Dhead.type.ver);

	send_buffer_head[11] = 0xff;
	send_buffer_head[12] = 0xff;
	send_buffer_head[13] = 0xff;

	crc = usMBCRC16(send_buffer_head,14);
	send_buffer_head[14] = (uint8_t)(crc & 0x00ff);
	send_buffer_head[15] = (uint8_t)((crc & 0xff00) >> 8);

}


/*******************************************
 * ����ԭ�ͣ�		void setSDKpackage(uint8_t fnum,uint16_t pnum)
 * ���ܣ�		д��̼����Ϳ�ʼ֡֡β
 * ���룺		fnum		��Ч�ֽ���
 * 			pnum		�����ܳ���
 * ���أ�		void
 * ****************************************/
void setSDKpackage(uint8_t fnum,uint16_t pnum)
{
	uint16_t crc;
	uint8_t famenum;
	uint16_t packagenum;

	famenum = fnum;
	packagenum = pnum;
	if(fnum%8)					// ������֡����8���ֽ�ʱ
	{
		famenum += 8;			// ����һ֡����
		famenum -= famenum%8;
	}

	send_buffer[famenum] = FRAME_HEAD_DATA;			// SDK���ݷ��ͱ�ʶ

	send_buffer[famenum+1] = CMD_UPGRADE_PK;		// �����·����ݷ���
	send_buffer[famenum+2] = (uint8_t)(packagenum & 0x00ff);	// �ڼ���
	send_buffer[famenum+3] = (uint8_t)((packagenum & 0xff00) >> 8);
	send_buffer[famenum+4] = fnum;					// ��Ч���ݳ���
	send_buffer[famenum+5] = 0xff;

	crc = usMBCRC16(&send_buffer[128],6);
	send_buffer[famenum+6] = (uint8_t)(crc & 0x00ff);
	send_buffer[famenum+7] = (uint8_t)((crc & 0xff00) >> 8);

}

/*******************************************
 * ����ԭ�ͣ�		void sendSDK(uint8_t *ptr,uint32_t len)
 * ���ܣ�		���͹̼�������
 * ���룺		*ptr		���淢�����ݵ�����
 * 			len			���鳤��
 * ���أ�		void
 * ****************************************/
void sendSDK(uint8_t *ptr,uint32_t len)
{
	uint32_t farmnum;
	uint32_t k = 100;
	Message clearsendbuff={0,0,0,{0,0,0,0,0,0,0,0}};

	farmnum = len%8 == 0 ? (len/8):(len/8 +1);	//�ж��ܹ�Ҫ���ͼ���

	for(uint32_t i=0;i<farmnum;i++)
	{
		for (uint8_t j =0;j<8;j++)
		{
			if ((len%8) && (i == farmnum-2))	//����������֡����һ֡����
			{
				if ((int8_t)(len%8-j) > 0)
				{
					*(SDK_Data_ptr[j]) = (ptr[i*8+j]);
				}
				else
				{
					*(SDK_Data_ptr[j]) = 0;		// ʣ������
				}
			}
			else
			{
				*(SDK_Data_ptr[j]) = (ptr[i*8+j]);
			}
		}
		sendPDOevent(&TestMaster_Data);			 // canopen���ͺ���

//		for (uint8_t i=0;i<8;i++)
//		{
//			printf("SDK_Data_ptr[i]=%x\n",*(SDK_Data_ptr[i]));
//		}
//
//		CLEAR_PTR_ARRAY(SDK_Data_ptr);// ������һ֡��ն����ֵ�������
		while(k)
		{
			k--;
			usleep(10);				// ÿ1ms����һ�� ��ֹPDO���ͻ�������
		}
		TestMaster_Data.PDO_status[0].last_message = clearsendbuff; //��շ��͵ı���Ļ��汣֤��ͬ���ݿ��Է���
//		usleep(5000);
		k=100;
	}
}


/*******************************************
 * ����ԭ�ͣ�		void receiveSDKcan(SDKbuffer_st *ptr, Message m)
 * ���ܣ�		��ȡ�����ذ���Ϣ��ʹ��������־λ
 * ���룺		*ptr		�����û���
 * 			m			cansocket �ڽ�������
 * ���أ�		void
 * ****************************************/
void receiveSDKcan(SDKbuffer_st *ptr, Message *m)
{
//	printf("********************receivetest***************************\n");
	if (m->cob_id >=UP_ID_RCV_FIRST && m->cob_id <= UP_ID_RCV_LAST) // �жϽ��������Ƿ�Ϊ����ID
	{
		ptr->id = m->cob_id;
		ptr->rcv_flag = 1;
		memcpy(ptr->un.data,m->data,8);
	}
}




/*******************************************
 * ����ԭ�ͣ�		int8_t receiveVerify(uint_8 *ptr)
 * ���ܣ�		У�����
 * ���룺		*ptr		��У������ָʾ ��ʼ֡�����ݷ���֡
 * ���أ�		void
 * ****************************************/
int8_t receiveVerify(void)
{
	static uint8_t verifynum = 1;
	uint8_t ret = 0;

	if(SDKcan_buffer.rcv_flag)
	{
//		for (uint8_t i=0;i<8;i++)
//		{
//			printf("SDK_Data_Rcv_ptr[i]=%x\n",*(SDK_Data_Rcv_ptr[i]));
//		}
		switch (*SDK_Data_Rcv_ptr[0])
		{
			case 0x5a:
				if (SDK.DMsg.state >= UPDATE_SEND_PACKAGE)		// ����Ϊ֡ͷ����״̬
				{
					ret = VERIFY_FAILED;
					break;
				}
				if (*SDK_Data_Rcv_ptr[1] == CMD_UPGRADE_REQUEST)		// �жϷ��ͷ���
				{
					SDK.DMsg.slave_cmd = *SDK_Data_Rcv_ptr[2];//SDKcan_buffer.un.bit.data2;
					if (SDK.DMsg.slave_cmd == CMD_RECIVE_SUCCEED)		// �жϴӻ��ϱ�����
					{
						ret = VERIFY_SUCCEED;
					}
					else
					{
						ret = VERIFY_FAILED;
					}
				}
				else
				{
					ret = VERIFY_FAILED;
				}

				break;

			case 0x5b:
				if (SDK.DMsg.state < UPDATE_SEND_PACKAGE)
				{
					ret = VERIFY_FAILED;
					break;
				}
				if (*SDK_Data_Rcv_ptr[1] == CMD_UPGRADE_PKRCV)
				{
					SDK.DMsg.slave_cmd = *SDK_Data_Rcv_ptr[2];//SDKcan_buffer.un.bit.data2;
					if (SDK.DMsg.slave_cmd == CMD_RECIVE_SUCCEED)
					{
						ret = VERIFY_SUCCEED;
						if (SDK.DMsg.data_len - SDK.DMsg.data_len_offset >= 128)
						{
							SDK.DMsg.data_len_offset+=128;
						}
						else
						{
							SDK.DMsg.data_len_offset = SDK.DMsg.data_len;
						}

					}
					else
					{
						ret = VERIFY_FAILED;
					}

				}
				else
				{
					ret = VERIFY_FAILED;
				}

				break;

			default :
				ret = VERIFY_FAILED;
				break;

		}
		SDKcan_buffer.rcv_flag = 0;
		CLEAR_PTR_ARRAY(SDK_Data_Rcv_ptr);// ������һ֡��ն����ֵ�������
		return ret;
	}
	else
	{
		return VERIFY_WAIT_FRAME;
	}
}


/*******************************************
 * ����ԭ�ͣ�		uint8_t traverselist_bydevicetype(const uint8_t *pcMac)
 * ���ܣ�		���������ȡ����ID��MAC��ַ
 * ���룺		*ptr	�豸����
 * ���أ�		�����Ƿ�ȡ����Ҫ�����豸�ı�־
 * ****************************************/
uint8_t traverselist_bydevicetype(const uint8_t *pcMac)
{
	NODE_INFO *pnode = NULL;
	static NODE_INFO *pnodesave = NULL;


	while(SDK.wdevice_num < DEVICE_NUM)		//
	{
		if (pnodesave == NULL)
		{
			pnode = endNodeListHead(); // ��ȡ����ͷ
		}
		else
		{
			pnode = pnodesave;
		}
		for(; pnode != NULL; pnode = endNodeListNext(pnode))	//��������
		{
			//mac addr and netid is same,the node was in list
			if (mem_cmp(pcMac, pnode->ubaHWGGMacAddr, TRAVERSE_LEN) == 0)	// �ȶ������е��豸��
			{
				SDK.wdevice_id[SDK.wdevice_num] = pnode->nodeCANOpenID;	// ��ȡ�������豸ID

				for (uint8_t k=0;k<4;k++)
				{
					SDK.wdevice_mac[SDK.wdevice_num][k] = pnode->ubaHWGGMacAddr[k];	// ��ȡ�������豸MAC��ַ
				}
				SDK.wdevice_num++;													// ��ȡ�������豸����

			}
			if ( SDK.wdevice_num == DEVICE_NUM)										// ���������豸�������������������
			{
				pnodesave = pnode;													// ���浱ǰ����λ��
				printf("array will overflow");
				return 1;
			}
		}
		pnodesave = NULL;
	return 0;
	}
}

int8_t upprocess(uint8_t num,FILE *fp)
{
	int8_t receive;
	uint8_t left_bytes;

	uint32_t PDO1_COBID = 0;
	uint32_t PDO2_COBID = 0;
	uint32_t size = sizeof(uint32_t);

//	time_t t;
//	struct tm *local;
	switch(SDK.DMsg.state)
	{
		case UPDATING:						// ������ʼ
			SDK.DMsg.package_resend_count = 0;	// �����ط�����
//			SDK.DMsg.package_timeount_count = 0;	// ���㳬ʱ����
			SDK.DMsg.timeout_count = 0;
//			TestMaster_obj1800_COB_ID_used_by_PDO = SDK.wdevice_id[num] + UP_ID_RCV_FIRST;
//			TestMaster_obj1401_COB_ID_used_by_PDO = SDK.wdevice_id[num] + UP_ID_RCV_FIRST;
			PDO1_COBID = SDK.wdevice_id[num] + UP_ID_RCV_FIRST;	// �޸Ľ���ID
			PDO2_COBID = SDK.wdevice_id[num] + UP_ID_RCV_FIRST;	// �޸ķ���ID
			/*****************************************
			 * Define RPDOs to match slave ID=0x40 TPDOs*
			 *****************************************/
			writeLocalDict( &TestMaster_Data, /*CO_Data* d*/
					0x1401, /*UNS16 index*/
					0x01, /*UNS8 subind*/
					&PDO1_COBID, /*void * pSourceData,*/
					&size, /* UNS8 * pExpectedSize*/
					RW);  /* UNS8 checkAccess */


			/*****************************************
			 * Define TPDOs to match slave ID=0x40 RPDOs*
			 *****************************************/
			writeLocalDict( &TestMaster_Data, /*CO_Data* d*/
					0x1800, /*UNS16 index*/
					0x01, /*UNS8 subind*/
					&PDO2_COBID, /*void * pSourceData,*/
					&size, /* UNS8 * pExpectedSize*/
					RW);  /* UNS8 checkAccess */

			setSDKhead(&SDK,num);
			sendSDK(send_buffer_head,16);
			SDK.DMsg.state = F1_RECV;
		break;

		case F0_SEND:								// �ط�

			SDK.DMsg.timeout_count = 0;
//			t = time(NULL);
//			local = localtime(&t);
//			printf("**************tiemsec = %d ",local->tm_sec);
			SDK.DMsg.package_resend_count ++;
			sendSDK(send_buffer_head,16);
			SDK.DMsg.state = F1_RECV;
			if (SDK.DMsg.package_resend_count > 11)
			{
				SDK.DMsg.state = UPDATE_FAILED;
			}
				break;
		case F1_RECV:							// �ȴ���֡
			receive = receiveVerify();
			if (VERIFY_FAILED == receive)		// �������
			{
				SDK.DMsg.state = F0_SEND;
				SDK.DMsg.timeout_count = 0;
			}
			else if (VERIFY_WAIT_FRAME == receive)	// ��ʱ
			{
				SDK.DMsg.timeout_count ++;
			}
			else if (VERIFY_SUCCEED == receive)
			{
				SDK.DMsg.state = UPDATE_SEND_PACKAGE;
			}

			if (SDK.DMsg.timeout_count >= VERIFY_WAIT_TIME) // ��ʱ
			{
				SDK.DMsg.state = F0_SEND;
				SDK.DMsg.timeout_count = 0;
//				SDK.DMsg.package_timeount_count++;
			}
			// �жϻذ��Ƿ���ȷ����ȷ������ȥ
		break;


		case UPDATE_SEND_PACKAGE:				// ����֡��������
			SDK.DMsg.package_resend_count = 0;
			SDK.DMsg.timeout_count = 0;

			if (SDK.DMsg.data_len - SDK.DMsg.data_len_offset >=128)	// ���ȷ���SDK�����������ݲ�ֵС��һ��������
			{
				fread(&send_buffer,1,128,fp);
				setSDKpackage(128,(SDK.DMsg.data_len_offset/128+1));
				sendSDK(send_buffer,136);
			}
			else
			{
				left_bytes = SDK.DMsg.data_len - SDK.DMsg.data_len_offset;
				fread(&send_buffer,1,left_bytes,fp);
				setSDKpackage(left_bytes,(SDK.DMsg.data_len_offset/128+1));
				sendSDK(send_buffer,left_bytes+8);
			}
			SDK.DMsg.state = UPDATE_WAIT_POST_BACK;
			break;

		case UPDATE_RESEND_PACKAGE:							// ����֡�ط�
			SDK.DMsg.package_resend_count ++;
			SDK.DMsg.timeout_count = 0;
			if (SDK.DMsg.data_len - SDK.DMsg.data_len_offset >=128)
			{
				sendSDK(send_buffer,136);
			}
			else
			{
				left_bytes = SDK.DMsg.data_len - SDK.DMsg.data_len_offset;
				sendSDK(send_buffer,left_bytes+8);
			}

			SDK.DMsg.state = UPDATE_WAIT_POST_BACK;
			if (SDK.DMsg.package_resend_count > 11)
			{
				SDK.DMsg.state = UPDATE_FAILED;
			}
			break;
		case UPDATE_WAIT_POST_BACK:							// �ȴ��ذ�
			receive = receiveVerify();
			if (VERIFY_FAILED == receive)
			{
				SDK.DMsg.state = UPDATE_RESEND_PACKAGE;
				SDK.DMsg.timeout_count = 0;
			}
			else if (VERIFY_WAIT_FRAME == receive)
			{
				SDK.DMsg.timeout_count ++;
			}
			else if (VERIFY_SUCCEED == receive)
			{
				if(SDK.DMsg.data_len == SDK.DMsg.data_len_offset)
				{
					SDK.DMsg.state = UPDATE_SUCCEED;
				}
				else
				{
					SDK.DMsg.state = UPDATE_SEND_PACKAGE;
				}

			}

			if (SDK.DMsg.timeout_count >= VERIFY_WAIT_TIME)			// ��ʱ
			{
				SDK.DMsg.state = UPDATE_RESEND_PACKAGE;
				SDK.DMsg.package_timeount_count ++;					// ��ʱ�����ط����ݼ�1
				SDK.DMsg.timeout_count = 0;
			}
			break;

		case UPDATE_SUCCEED:										// �������

			//����豸�����ɹ�����Ϣ�����ʱ��ID ���豸���͵ȵ�
			printf("*******************updatasucceed******************\n");
			WRITELOGFILE(5,"updatasucceed");
			SDK.DMsg.state = UPDATING;								// ����״̬
			SDK.DMsg.data_len_offset = 0;							// ���÷��ͳ���
			fseek(fp,16,SEEK_SET);									// ����SDK�ļ�ָ��
			return 1;

			break;


		case UPDATE_FAILED:											// ��������

			//����豸����ʧ����Ϣ��ID �豸MAC��ַ
			printf("*******************updatafalied******************\n");
			fseek(fp,16,SEEK_SET);
			SDK.DMsg.data_len_offset = 0;
			SDK.DMsg.state = UPDATING;
			return -1;
			break;

	}
	return 0;
}





void sdkup(void)
{
	static uint8_t uploop = 0;
	static uint8_t flag;
	static uint8_t loopwhile = 1;
	FILE *fp;
	fp = fopen("./testbin.bin","r+b");
//	SDK.wdevice_num = 2;	// test code please delete
//	SDK.wdevice_id[0]=0;	// test code please delete
//	SDK.wdevice_id[1]=1;	// test code please delete
	fread(SDK.Dhead.byte16,1,16,fp);
	getSDKmessage();			// ��ȡSDK��Ϣ

	while (loopwhile)
	{
		switch (uploop)
		{
			case 0:
			flag = traverselist_bydevicetype(SDK.Dhead.type.device);
//			printf("*********flag=%d***********\n",flag);
			uploop = 1;
			loopwhile = SDK.wdevice_num;
//			printf("*********loopwhile=%d**********\n",loopwhile);
			break;
			case 1:

				for (uint8_t i=0;i<SDK.wdevice_num;)
				{
					while ( 0 == upprocess(i,fp))
					{
						usleep(100000);
					}
					i++;
//					printf("*********looptest**********\n");
				}
				SDK.wdevice_num = 0;
				if (flag)
				{

					uploop = 0;
				}
				else
				{
					uploop = 2;
				}
//				printf("*********uploop=%d*********SDK.wdevice_num=%d\n",uploop,SDK.wdevice_num);
			break;
			case 2:
				fclose(fp);
				uploop = 0;
				break;

		}
	}

//	printf("data_len = %d\n,package_sum =%d\n",SDK.DMsg.data_len,SDK.DMsg.package_sum);

}

void* sdkup_thread_fun(void* ptr)
{
	sdkup();
	return NULL;
}

pthread_t sdkup_thread_id;


void SDK_process_pthread(void)
{
	int temp;
	if ((temp = pthread_create(&sdkup_thread_id,NULL,sdkup_thread_fun,NULL)) != 0)
	{
		printf("SDK_process_pthread create ERROR!\nERRORCODE=%x\r\n",temp);
	}
	else
	{
		printf("SDK_process_pthread create SUCCESS!\r\n");
	}

}




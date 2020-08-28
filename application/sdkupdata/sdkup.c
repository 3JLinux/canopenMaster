/*
 * sdkup.c
 *
 *  Created on: 2020年8月18日
 *      Author: yaodidi
 */
/*
 * main.c
 *
 *  Created on: 2020年8月7日
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
#include "endnodemem.h"	// 此处为添加链表操作头文件
#include "canfestival.h"  // 此处为添加canfestival库头文件
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
 * 函数原型：		void getSDKmessage(void)
 * 功能：		获取SDK包信息
 * 输入：		void
 * 返回：		void
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
 * 函数原型：		getSDKdata(void *prt,FILE *fp,uint8_t readlen)
 * 功能：		读取SDK数据
 * 输入：		*ptr		读取数据存放数组
 * 			*fp			打开的文件返回指针
 * 			readlen		读取数据长度
 * 返回：		void
 * ****************************************/
void getSDKdata(void *ptr,FILE *fp,uint8_t readlen)
{
	fread(ptr,sizeof(uint8_t),readlen,fp);
}

/*******************************************
 * 函数原型：		void setSDKhead(void *ptr,uint8_t num)
 * 功能：		写入固件发送开始帧帧头
 * 输入：		ptr			SDK信息
 * 			num			ID
 * 返回：		void
 * ****************************************/
void setSDKhead(void *ptr,uint8_t num)
{
	uint16_t crc;
	UPdata_st *p;
	p = (UPdata_st*)ptr;

	send_buffer_head[0] = FRAME_HEAD_SKD;									// 固件开始升级帧 帧头指示
	send_buffer_head[1] = (uint8_t)CMD_UPGRADE;					// 固件开始升级帧 数据方向指示 主机下发
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
 * 函数原型：		void setSDKpackage(uint8_t fnum,uint16_t pnum)
 * 功能：		写入固件发送开始帧帧尾
 * 输入：		fnum		有效字节数
 * 			pnum		数据总长度
 * 返回：		void
 * ****************************************/
void setSDKpackage(uint8_t fnum,uint16_t pnum)
{
	uint16_t crc;
	uint8_t famenum;
	uint16_t packagenum;

	famenum = fnum;
	packagenum = pnum;
	if(fnum%8)					// 当数据帧不满8个字节时
	{
		famenum += 8;			// 做加一帧处理
		famenum -= famenum%8;
	}

	send_buffer[famenum] = FRAME_HEAD_DATA;			// SDK数据发送标识

	send_buffer[famenum+1] = CMD_UPGRADE_PK;		// 主机下发数据方向
	send_buffer[famenum+2] = (uint8_t)(packagenum & 0x00ff);	// 第几包
	send_buffer[famenum+3] = (uint8_t)((packagenum & 0xff00) >> 8);
	send_buffer[famenum+4] = fnum;					// 有效数据长度
	send_buffer[famenum+5] = 0xff;

	crc = usMBCRC16(&send_buffer[128],6);
	send_buffer[famenum+6] = (uint8_t)(crc & 0x00ff);
	send_buffer[famenum+7] = (uint8_t)((crc & 0xff00) >> 8);

}

/*******************************************
 * 函数原型：		void sendSDK(uint8_t *ptr,uint32_t len)
 * 功能：		发送固件包函数
 * 输入：		*ptr		保存发送数据的数组
 * 			len			数组长度
 * 返回：		void
 * ****************************************/
void sendSDK(uint8_t *ptr,uint32_t len)
{
	uint32_t farmnum;
	uint32_t k = 100;
	Message clearsendbuff={0,0,0,{0,0,0,0,0,0,0,0}};

	farmnum = len%8 == 0 ? (len/8):(len/8 +1);	//判断总共要发送几次

	for(uint32_t i=0;i<farmnum;i++)
	{
		for (uint8_t j =0;j<8;j++)
		{
			if ((len%8) && (i == farmnum-2))	//若发送数据帧不满一帧长度
			{
				if ((int8_t)(len%8-j) > 0)
				{
					*(SDK_Data_ptr[j]) = (ptr[i*8+j]);
				}
				else
				{
					*(SDK_Data_ptr[j]) = 0;		// 剩下清零
				}
			}
			else
			{
				*(SDK_Data_ptr[j]) = (ptr[i*8+j]);
			}
		}
		sendPDOevent(&TestMaster_Data);			 // canopen发送函数

//		for (uint8_t i=0;i<8;i++)
//		{
//			printf("SDK_Data_ptr[i]=%x\n",*(SDK_Data_ptr[i]));
//		}
//
//		CLEAR_PTR_ARRAY(SDK_Data_ptr);// 发送完一帧清空对象字典内数据
		while(k)
		{
			k--;
			usleep(10);				// 每1ms发送一次 防止PDO发送缓冲区满
		}
		TestMaster_Data.PDO_status[0].last_message = clearsendbuff; //清空发送的保存的缓存保证相同数据可以发送
//		usleep(5000);
		k=100;
	}
}


/*******************************************
 * 函数原型：		void receiveSDKcan(SDKbuffer_st *ptr, Message m)
 * 功能：		获取升级回包信息，使能升级标志位
 * 输入：		*ptr		保存用缓存
 * 			m			cansocket 内接收数据
 * 返回：		void
 * ****************************************/
void receiveSDKcan(SDKbuffer_st *ptr, Message *m)
{
//	printf("********************receivetest***************************\n");
	if (m->cob_id >=UP_ID_RCV_FIRST && m->cob_id <= UP_ID_RCV_LAST) // 判断接收内容是否为升级ID
	{
		ptr->id = m->cob_id;
		ptr->rcv_flag = 1;
		memcpy(ptr->un.data,m->data,8);
	}
}




/*******************************************
 * 函数原型：		int8_t receiveVerify(uint_8 *ptr)
 * 功能：		校验接收
 * 输入：		*ptr		待校验数据指示 开始帧或数据返回帧
 * 返回：		void
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
				if (SDK.DMsg.state >= UPDATE_SEND_PACKAGE)		// 若不为帧头发送状态
				{
					ret = VERIFY_FAILED;
					break;
				}
				if (*SDK_Data_Rcv_ptr[1] == CMD_UPGRADE_REQUEST)		// 判断发送方向
				{
					SDK.DMsg.slave_cmd = *SDK_Data_Rcv_ptr[2];//SDKcan_buffer.un.bit.data2;
					if (SDK.DMsg.slave_cmd == CMD_RECIVE_SUCCEED)		// 判断从机上报命令
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
		CLEAR_PTR_ARRAY(SDK_Data_Rcv_ptr);// 发送完一帧清空对象字典内数据
		return ret;
	}
	else
	{
		return VERIFY_WAIT_FRAME;
	}
}


/*******************************************
 * 函数原型：		uint8_t traverselist_bydevicetype(const uint8_t *pcMac)
 * 功能：		遍历链表获取升级ID、MAC地址
 * 输入：		*ptr	设备类型
 * 返回：		返回是否取完需要升级设备的标志
 * ****************************************/
uint8_t traverselist_bydevicetype(const uint8_t *pcMac)
{
	NODE_INFO *pnode = NULL;
	static NODE_INFO *pnodesave = NULL;


	while(SDK.wdevice_num < DEVICE_NUM)		//
	{
		if (pnodesave == NULL)
		{
			pnode = endNodeListHead(); // 获取链表头
		}
		else
		{
			pnode = pnodesave;
		}
		for(; pnode != NULL; pnode = endNodeListNext(pnode))	//遍历链表
		{
			//mac addr and netid is same,the node was in list
			if (mem_cmp(pcMac, pnode->ubaHWGGMacAddr, TRAVERSE_LEN) == 0)	// 比对链表中的设备号
			{
				SDK.wdevice_id[SDK.wdevice_num] = pnode->nodeCANOpenID;	// 获取待升级设备ID

				for (uint8_t k=0;k<4;k++)
				{
					SDK.wdevice_mac[SDK.wdevice_num][k] = pnode->ubaHWGGMacAddr[k];	// 获取待升级设备MAC地址
				}
				SDK.wdevice_num++;													// 获取待升级设备数量

			}
			if ( SDK.wdevice_num == DEVICE_NUM)										// 若待升级设备数量超过缓存最大数量
			{
				pnodesave = pnode;													// 保存当前链表位置
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
		case UPDATING:						// 升级开始
			SDK.DMsg.package_resend_count = 0;	// 清零重发次数
//			SDK.DMsg.package_timeount_count = 0;	// 清零超时次数
			SDK.DMsg.timeout_count = 0;
//			TestMaster_obj1800_COB_ID_used_by_PDO = SDK.wdevice_id[num] + UP_ID_RCV_FIRST;
//			TestMaster_obj1401_COB_ID_used_by_PDO = SDK.wdevice_id[num] + UP_ID_RCV_FIRST;
			PDO1_COBID = SDK.wdevice_id[num] + UP_ID_RCV_FIRST;	// 修改接收ID
			PDO2_COBID = SDK.wdevice_id[num] + UP_ID_RCV_FIRST;	// 修改发送ID
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

		case F0_SEND:								// 重发

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
		case F1_RECV:							// 等待回帧
			receive = receiveVerify();
			if (VERIFY_FAILED == receive)		// 命令错误
			{
				SDK.DMsg.state = F0_SEND;
				SDK.DMsg.timeout_count = 0;
			}
			else if (VERIFY_WAIT_FRAME == receive)	// 超时
			{
				SDK.DMsg.timeout_count ++;
			}
			else if (VERIFY_SUCCEED == receive)
			{
				SDK.DMsg.state = UPDATE_SEND_PACKAGE;
			}

			if (SDK.DMsg.timeout_count >= VERIFY_WAIT_TIME) // 超时
			{
				SDK.DMsg.state = F0_SEND;
				SDK.DMsg.timeout_count = 0;
//				SDK.DMsg.package_timeount_count++;
			}
			// 判断回包是否正确若正确则跳下去
		break;


		case UPDATE_SEND_PACKAGE:				// 数据帧开发发送
			SDK.DMsg.package_resend_count = 0;
			SDK.DMsg.timeout_count = 0;

			if (SDK.DMsg.data_len - SDK.DMsg.data_len_offset >=128)	// 当先发送SDK数据与总数据差值小于一个包数量
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

		case UPDATE_RESEND_PACKAGE:							// 数据帧重发
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
		case UPDATE_WAIT_POST_BACK:							// 等待回包
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

			if (SDK.DMsg.timeout_count >= VERIFY_WAIT_TIME)			// 超时
			{
				SDK.DMsg.state = UPDATE_RESEND_PACKAGE;
				SDK.DMsg.package_timeount_count ++;					// 因超时导致重发数据加1
				SDK.DMsg.timeout_count = 0;
			}
			break;

		case UPDATE_SUCCEED:										// 升级完成

			//添加设备升级成功的信息，如耗时，ID ，设备类型等等
			printf("*******************updatasucceed******************\n");
			WRITELOGFILE(5,"updatasucceed");
			SDK.DMsg.state = UPDATING;								// 重置状态
			SDK.DMsg.data_len_offset = 0;							// 重置发送长度
			fseek(fp,16,SEEK_SET);									// 重置SDK文件指针
			return 1;

			break;


		case UPDATE_FAILED:											// 升级错误

			//添加设备升级失败信息，ID 设备MAC地址
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
	getSDKmessage();			// 获取SDK信息

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




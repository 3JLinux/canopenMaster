#include "canopenAnalysis.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "delay.h"
#include "canopen_fifo.h"
#include "nodeApp.h"

extern CO_Data TestMaster_Data;
pthread_t canOpenAnalysis_threadFlag; 

static uint8_t canOpenAnalysis(void)
{
	
	uint8_t buf[FIFO_BUF_MAX_LEN] = {0};
	uint32_t len = 0;
	uint32_t i;
	uint16_t canOpenCmd = 0;
	uint8_t canOpenDataLen = 0;
	uint32_t readNetTimeCount = 0;
	uint8_t slave_COB_ID = 0;
	uint8_t slave_MAC[SLAVE_MAC_MAX_LEN] = {0};
	uint32_t dict_size = 0;
	uint32_t slave_addr;
	uint8_t slave_nodeID;
	uint32_t abortCode;
	uint8_t count = 1;
	static uint8_t err,retcode;
	err=err;
	retcode=retcode;
	len = canOpenFifoGetLen();
	if(len > 0)
	{
		canOpenFifoRead(buf, len);
		printf("canOpenFifoRead:");
		for(i=0;i<len;i++)
		{
			printf("0x%X ",buf[i]);
		}
		printf("\r\n");
		for(i=0; i<len; i++)
		{
			canOpenCmd = (buf[i]&0x80) + ((buf[i+1]<<8)&0xFF00);
			if(canOpenCmd == TPDO2_CMD)
			{
				canOpenDataLen = buf[i+CANOPEN_DATA_LEN_BIT];
				if(canOpenDataLen == CANOPEN_DATA_MAX_LEN)
				{
					if(buf[i+CANOPEN_DATABIT1] == OD_SLAVE_LOGIN_HEAD && buf[i+CANOPEN_DATABIT4] == OD_SLAVE_LOGIN_TAIL)
					{
						switch(buf[i+CANOPEN_DATABIT2])
						{
							case OD_SLAVE_LOGIN_CMD:
								slave_MAC[0] = buf[CANOPEN_DATABIT8];
								slave_MAC[1] = buf[CANOPEN_DATABIT7];
								slave_MAC[2] = buf[CANOPEN_DATABIT6];
								slave_MAC[3] = buf[CANOPEN_DATABIT5];
								if(slave_MAC[0] != 0 && slave_MAC[1] != 0 && slave_MAC[2]!= 0 && slave_MAC[3] != 0)
								{
									node_addMAC(slave_MAC);
									slave_nodeID = buf[i] - 0x80;
									if(buf[i+CANOPEN_DATABIT3] == 0 || buf[i+CANOPEN_DATABIT3] != slave_nodeID)
									{
										slave_COB_ID = node_getID(slave_MAC);
										dict_size = sizeof(UNS32);
										slave_addr = TRANSMIT_SDO_CMD + slave_nodeID;
										retcode = writeLocalDict(&TestMaster_Data, (UNS16)CLIENT_SDO_INDEX, (UNS8)1, &slave_addr, &dict_size, 1);
										slave_addr = RECEIVE_SDO_CMD +slave_nodeID;
										retcode = writeLocalDict(&TestMaster_Data, (UNS16)CLIENT_SDO_INDEX, (UNS8)2, &slave_addr, &dict_size, 1);
										dict_size = sizeof(UNS8);
										retcode = writeLocalDict(&TestMaster_Data, (UNS16)CLIENT_SDO_INDEX, (UNS8)3, &slave_nodeID, &dict_size, 1);
										err = writeNetworkDict(&TestMaster_Data, slave_nodeID, OD_SLAVE_LOGIN_INDEX, OD_SLAVE_LOGIN_SUBINDEX, count, uint32, &slave_COB_ID, 1);
										while(getWriteResultNetworkDict(&TestMaster_Data, slave_nodeID, &abortCode) == SDO_DOWNLOAD_IN_PROGRESS)
										{
											delay_ms(50);
											readNetTimeCount++;
											if(readNetTimeCount >= READ_NET_TIMEOUT)
											{
												return 1;
											}
										}
									}
									else
									{
										node_dec_online(slave_MAC);
									}
								}
								break;
							default:
								break;
						}
					}
				}
			}
		}
	}
	return 0;
}




void* canOpenAnalysis_thread(void* ptr)
{
	static uint32_t  timer_count = 0;
	while(1)
	{
		timer_count++;
		canOpenAnalysis();
		if(timer_count >= NODE_NET_CHECK_TIME_S)
		{
			timer_count = 0;
			node_check_in_out_net();
		}
		delay_s(1);
	}
}


void canOpenAnalysis_threadCreate(void)
{
	int temp;
	memset(&canOpenAnalysis_threadFlag, 0, sizeof(canOpenAnalysis_threadFlag));
	if((temp = pthread_create(&canOpenAnalysis_threadFlag, NULL, canOpenAnalysis_thread, NULL)) != 0)
	{
		printf("canOpenAnalysis_thread create ERROR!\r\n");
	}
	else
	{
		printf("canOpenAnalysis_thread creat SUCCESS!\r\n");
	}
}


void canOpenAnalysis_threadWait(void)
{
	if(canOpenAnalysis_threadFlag != 0)
	{
		pthread_join(canOpenAnalysis_threadFlag, NULL);
		printf("canOpenAnalysis_thread END\r\n");
	}
}


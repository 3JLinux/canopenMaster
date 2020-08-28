#include "battery_app.h"
#include "delay.h"
//#include "stm32_canfestival.h"
//#include "ftl.h"
//#include "nand.h"
//#include "malloc.h"
#include "nodeApp.h"


uint8_t battery_config[BATTERY_CONFIG_SIZE] = {0};
uint8_t battery_control[BATTERY_CONTROL_NUM] = {0};
uint8_t battery_info1[BATTERY_INFO1_NUM] = {0};
uint8_t battery_info2[BATTERY_INFO2_NUM] = {0};
uint8_t battery_info3[BATTERY_INFO3_NUM] = {0};
uint8_t battery_loss_count[BATTERY_LOSS_COUNT_NUM] = {0};
extern CO_Data TestMaster_Data;

/*
//canOpenAnalysis�������ȼ�
#define batte_query_TASK_PRIO		9
//canOpenAnalysis�����ջ��С
#define batte_query_STK_SIZE 		512
//canOpenAnalysis������ƿ�
StaticTask_t batte_queryTaskTCB;
//canOpenAnalysis�����ջ
StackType_t batte_queryTASK_STK[batte_query_STK_SIZE];
//canOpenAnalysis������
TaskHandle_t batte_queryTask_Handler;
//canOpenAnalysis������
void batte_query_task(void *pvParameters);

//��ֵ�ź������
SemaphoreHandle_t BinarySemaphore;	//��ֵ�ź������
*/

/*****************************************************
����ԭ�ͣ� 	static uint8_t GetBatteryConfig(CO_Data* d,UNS8 nodeId,UNS16 index)
���ܣ�			����sdo���Ķ�ȡ��ز���
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
						UNS16 index
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
static uint8_t GetBatteryConfig(CO_Data* d, UNS8 nodeId, UNS16 index)
{
	uint8_t type = uint32;
	BATTERY_CONFIG* buf = NULL;
	uint32_t size = SIZE_4BYTES;
	uint32_t abortCode;
	uint8_t i;
	uint8_t buf_offset = 0;;
	uint32_t readNetTimeCount = 0;
	buf = (BATTERY_CONFIG*)battery_config;
	for(i=1;i<=BATTERY_CONFIG_NUM;i++)
	{
		//�����ֵ����������͵Ķ�Ӧ����
		switch(i)
		{
			case CASE1:
				type = uint32;
				size = SIZE_4BYTES;
			break;
			case CASE2:
			case CASE3:
			case CASE4:
			case CASE5:
			case CASE6:
			case CASE7:
				type = uint16;
				size = SIZE_2BYTES;
			break;
			case CASE8:
			case CASE9:
			case CASE10:
			case CASE11:
			case CASE12:
			case CASE13:
				type = int32;
				size = SIZE_4BYTES;
			break;
			case CASE14:
			case CASE15:
			case CASE16:
			case CASE17:
			case CASE18:
			case CASE19:
			case CASE20:
			case CASE21:
				type = uint16;
				size = SIZE_2BYTES;
			break;
			case CASE22:
			case CASE23:
			case CASE24:
			case CASE25:
				type = int16;
				size = SIZE_2BYTES;
			break;
			default:
			break;
		}
		readNetworkDict(d, nodeId,index, i, type, 0); //����SDO���Ķ�ȡ�ӻ�����
		while (getReadResultNetworkDict (d, nodeId, buf+buf_offset, &size,&abortCode) == SDO_UPLOAD_IN_PROGRESS) //������д���ֵ��ͷ�����
		{
			//��ʱ���
			delay_ms(10);
			readNetTimeCount++;
			if(readNetTimeCount>=READ_NET_TIMEOUT)
			{
				
				return 1;
			}
		}
		buf_offset += (uint8_t)size;
	}
	return 0;
}


//�ú�����Ҫ���߳��ڵ��ã�ʵ���̵߳��Ƚ��н��ս���
/*****************************************************
����ԭ�ͣ� 	uint8_t GetBatteryConfig_app(CO_Data* d,UNS8 nodeId)
���ܣ�			��ȡ��ز���
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
uint8_t GetBatteryConfig_app(CO_Data* d, UNS8 nodeId)
{
	return GetBatteryConfig(d,nodeId,BATTERY_CONFIG_ADDR);
}


/*****************************************************
����ԭ�ͣ� 	uint8_t SetBatteryConfig(CO_Data* d, UNS8 nodeId, UNS16 index, UNS8 subIndex, UNS8 dataType, void* data)
���ܣ�			����sdo����д��ز���
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
						UNS16 index �����ֵ�����
						UNS8 subIndex �����ֵ�������
						UNS8 dataType ��������
						void* data ����ָ��
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
uint8_t SetBatteryConfig(CO_Data* d, UNS8 nodeId, UNS16 index, UNS8 subIndex, UNS8 dataType, void* data)
{
	uint32_t abortCode;
	uint8_t count;
	uint32_t readNetTimeCount = 0;
	writeNetworkDict(d, nodeId, index, subIndex, count, dataType, data, 0);
	while (getWriteResultNetworkDict (d, nodeId, &abortCode) == SDO_DOWNLOAD_IN_PROGRESS)
	{
		//��ʱ���
		delay_ms(50);
		readNetTimeCount++;
		if(readNetTimeCount>=READ_NET_TIMEOUT)
		{
			
			return 1;
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	uint8_t SetBatteryConfig_app(CO_Data* d, UNS8 nodeId, UNS16 index, UNS8 subIndex, UNS8 dataType, void* data)
���ܣ�			д��ز���
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
						UNS16 index �����ֵ�����
						UNS8 subIndex �����ֵ������� 0xFFʱΪ��д��������������
						UNS8 dataType ��������
						void* data ����ָ��
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
uint8_t SetBatteryConfig_app(CO_Data* d, UNS8 nodeId, UNS8 subIndex, UNS8 dataType, void* data)
{
	uint8_t i;
	uint8_t type = uint32;
	if(subIndex != WRITE_ALL_OD)
	{
		if(subIndex<BATTERY_CONFIG_NUM)
		{
			return SetBatteryConfig(d, nodeId, BATTERY_CONFIG_ADDR, subIndex, dataType, data);
		}
		else
		{
			return 1;
		}
	}
	else
	{
		for(i=1; i<=BATTERY_CONFIG_NUM; i++)
		{
			switch(i)
			{
				case CASE1:
				type = uint32;
			break;
			case CASE2:
			case CASE3:
			case CASE4:
			case CASE5:
			case CASE6:
			case CASE7:
				type = uint16;
			break;
			case CASE8:
			case CASE9:
			case CASE10:
			case CASE11:
			case CASE12:
			case CASE13:
				type = int32;
			break;
			case CASE14:
			case CASE15:
			case CASE16:
			case CASE17:
			case CASE18:
			case CASE19:
			case CASE20:
			case CASE21:
				type = uint16;
			break;
			case CASE22:
			case CASE23:
			case CASE24:
			case CASE25:
				type = int16;
			break;
			default:
			break;
			}
			if(SetBatteryConfig(d, nodeId, BATTERY_CONFIG_ADDR, i, type, data))
			{
				return 1;
			}
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	static uint8_t GetBatteryControl(CO_Data* d,UNS8 nodeId,UNS16 index)
���ܣ�			����sdo���Ķ�ȡ��ز���
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
						UNS16 index
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
static uint8_t GetBatteryControl(CO_Data* d,UNS8 nodeId,UNS16 index)
{
	uint8_t type = uint32;
	BATTERY_CONTROL* buf = NULL;
	uint32_t size = SIZE_4BYTES;
	uint32_t abortCode;
	uint8_t i;
	uint8_t buf_offset = 0;;
	uint32_t readNetTimeCount = 0;
	buf = (BATTERY_CONTROL*)battery_control;
	for(i=1;i<=BATTERY_CONTROL_NUM;i++)
	{
		type = uint8;
		size = SIZE_1BYTES;
		readNetworkDict(d, nodeId,index, i, type, 0); //����SDO���Ķ�ȡ�ӻ�����
		while (getReadResultNetworkDict (d, nodeId, buf+buf_offset, &size,&abortCode) == SDO_UPLOAD_IN_PROGRESS) //������д���ֵ��ͷ�����
		{
			//��ʱ���
			delay_ms(10);
			readNetTimeCount++;
			if(readNetTimeCount>=READ_NET_TIMEOUT)
			{
				
				return 1;
			}
		}
		buf_offset += (uint8_t)size;
	}
	return 0;
}

/*****************************************************
����ԭ�ͣ� 	uint8_t GetBatteryControl_app(CO_Data* d, UNS8 nodeId)
���ܣ�			��ȡ��ؿ��Ʋ���
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
uint8_t GetBatteryControl_app(CO_Data* d, UNS8 nodeId)
{
	return GetBatteryControl(d,nodeId,BATTERY_CONTROL_ADDR);
}


/*****************************************************
����ԭ�ͣ� 	uint8_t BatteryControlBootloader(CO_Data* d, UNS8 nodeId)
���ܣ�			���Ƶ�ؽ��뿪������
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
uint8_t BatteryControlBootloader(CO_Data* d, UNS8 nodeId)
{
	uint32_t abortCode;
	uint8_t count;
	uint32_t readNetTimeCount = 0;
	uint8_t dataType = boolean;
	CROTROL_STATUS data = CROTROL_ENABLE;
	writeNetworkDict(d, nodeId, BATTERY_CONTROL_ADDR, BATTERY_BOOTLOADER_SUBINDEX, count, dataType, &data, 0);
	while (getWriteResultNetworkDict (d, nodeId, &abortCode) == SDO_DOWNLOAD_IN_PROGRESS)
	{
		//��ʱ���
		delay_ms(50);
		readNetTimeCount++;
		if(readNetTimeCount>=READ_NET_TIMEOUT)
		{
			
			return 1;
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	uint8_t BatteryControlReboot(CO_Data* d, UNS8 nodeId)
���ܣ�			���Ƶ������
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
uint8_t BatteryControlReboot(CO_Data* d, UNS8 nodeId)
{
	uint32_t abortCode;
	uint8_t count;
	uint32_t readNetTimeCount = 0;
	uint8_t dataType = boolean;
	CROTROL_STATUS data = CROTROL_ENABLE;
	writeNetworkDict(d, nodeId, BATTERY_CONTROL_ADDR, BATTERY_REBOOT_SUBINDEX, count, dataType, &data, 0);
	while (getWriteResultNetworkDict (d, nodeId, &abortCode) == SDO_DOWNLOAD_IN_PROGRESS)
	{
		//��ʱ���
		delay_ms(50);
		readNetTimeCount++;
		if(readNetTimeCount>=READ_NET_TIMEOUT)
		{
			
			return 1;
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	uint8_t BatteryControlShutdown(CO_Data* d, UNS8 nodeId)
���ܣ�			���Ƶ�عػ�
���룺			CO_Data* d
						UNS8 nodeId �ӻ�ID
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
uint8_t BatteryControlShutdown(CO_Data* d, UNS8 nodeId)
{
	uint32_t abortCode;
	uint8_t count;
	uint32_t readNetTimeCount = 0;
	uint8_t dataType = boolean;
	CROTROL_STATUS data = CROTROL_ENABLE;
	writeNetworkDict(d, nodeId, BATTERY_CONTROL_ADDR, BATTERY_SHUTDOWN_SUBINDEX, count, dataType, &data, 0);
	while (getWriteResultNetworkDict (d, nodeId, &abortCode) == SDO_DOWNLOAD_IN_PROGRESS)
	{
		//��ʱ���
		delay_ms(50);
		readNetTimeCount++;
		if(readNetTimeCount>=READ_NET_TIMEOUT)
		{
			
			return 1;
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	uint8_t BatteryControlShutdown(CO_Data* d, UNS8 nodeId)
���ܣ�			���Ƶ�عػ�
���룺			CO_Data* d
						UNS8 nodeId
						CROTROL_STATUS data
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
uint8_t BatteryControlLowPowerPtotect(CO_Data* d, UNS8 nodeId, CROTROL_STATUS data)
{
	uint32_t  abortCode;
	uint8_t count;
	uint32_t readNetTimeCount = 0;
	uint8_t dataType = boolean;
	writeNetworkDict(d, nodeId, BATTERY_CONTROL_ADDR, BATTERY_LOWPOWER_PROTECT_SUBINDEX, count, dataType, &data, 0);
	while (getWriteResultNetworkDict (d, nodeId, &abortCode) == SDO_DOWNLOAD_IN_PROGRESS)
	{
		//��ʱ���
		delay_ms(50);
		readNetTimeCount++;
		if(readNetTimeCount>=READ_NET_TIMEOUT)
		{
			
			return 1;
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	static uint8_t sendSDO_app(CO_Data* d, UNS8 nodeId, UNS16 index, UNS8 subIndex, UNS8 dataType, void* buf, UNS32 *size)
���ܣ�
���룺			CO_Data* d
						UNS8 nodeId
						UNS16 index
						UNS8 subIndex
						UNS8 dataType
						void* buf
						UNS32 *size
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
static uint8_t sendSDO_app(CO_Data* d, UNS8 nodeId, UNS16 index, UNS8 subIndex, UNS8 dataType, void* buf, UNS32 *size)
{
	uint32_t abortCode;
	uint32_t readNetTimeCount = 0;
	readNetworkDict(&TestMaster_Data, nodeId, index, subIndex, dataType, 0); //����SDO���Ķ�ȡ�ӻ�����
	while (getReadResultNetworkDict (d, nodeId, buf, size, &abortCode) == SDO_UPLOAD_IN_PROGRESS) //������д���ֵ��ͷ�����
	{
		//��ʱ���
		delay_ms(10);
		readNetTimeCount++;
		if(readNetTimeCount>=READ_NET_TIMEOUT)
		{
			
			return 1;
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	static uint8_t GetBatteryInfo1(CO_Data* d, UNS8 nodeId)
���ܣ�			����sdo���Ķ�ȡ��ز���
���룺			CO_Data* d
						UNS8 nodeId
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
static uint8_t GetBatteryInfo1(CO_Data* d, UNS8 nodeId)
{
	uint8_t type = uint32;
	BETTERY_INFO1* buf = NULL;
	uint32_t size = SIZE_4BYTES;
	uint8_t subIndex = CASE1;
	uint8_t i;
	buf = (BETTERY_INFO1*)battery_info1;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->Version)), &size))
	{
		return 1;
	}
	subIndex = CASE2;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->RemainTime)), &size))
	{
		return 1;
	}
	subIndex = CASE3;
	type = uint32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->DesignCapacity)), &size))
	{
		return 1;
	}
	subIndex = CASE4;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->Status)), &size))
	{
		return 1;
	}
	subIndex = CASE5;
	type = int32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->Current)), &size))
	{
		return 1;
	}
	type = uint16;
	size = SIZE_2BYTES;
	for(i=CASE6;i<=CASE17;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(buf->CellVoltage + i - CASE6), &size))
		{
			return 1;
		}
	}
	type = int16;
	size = SIZE_2BYTES;
	for(i=CASE18;i<=CASE19;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(buf->CellTemp + i - CASE18), &size))
		{
			return 1;
		}
	}
	subIndex = CASE20;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->PcbTemp)), &size))
	{
		return 1;
	}
	subIndex = CASE21;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->SOC)), &size))
	{
		return 1;
	}
	subIndex = CASE22;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->CycleTimes)), &size))
	{
		return 1;
	}
	subIndex = CASE23;
	type = int16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->MosTemp)), &size))
	{
		return 1;
	}
	subIndex = CASE24;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->OverDsgCount)), &size))
	{
		return 1;
	}
	subIndex = CASE27;
	type = int32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO1_ADDR, subIndex, type, (void*)(&(buf->utc)), &size))
	{
		return 1;
	}
	return 0;
}

/*****************************************************
����ԭ�ͣ� 	static uint8_t GetBatteryInfo2(CO_Data* d, UNS8 nodeId)
���ܣ�			����sdo���Ķ�ȡ��ز���
���룺			CO_Data* d
						UNS8 nodeId
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
static uint8_t GetBatteryInfo2(CO_Data* d, UNS8 nodeId)
{
	uint8_t type = uint32;
	BETTERY_INFO2* buf = NULL;
	uint32_t size = SIZE_4BYTES;
	uint8_t subIndex = CASE1;
	uint8_t i;
	buf = (BETTERY_INFO2*)battery_info2;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->Version)), &size))
	{
		return 1;
	}
	subIndex = CASE2;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->RemainTime)), &size))
	{
		return 1;
	}
	subIndex = CASE3;
	type = uint32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->DesignCapacity)), &size))
	{
		return 1;
	}
	subIndex = CASE4;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->RealCapacity)), &size))
	{
		return 1;
	}
	subIndex = CASE5;
	type = int32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->Status)), &size))
	{
		return 1;
	}
	subIndex = CASE6;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->SOC)), &size))
	{
		return 1;
	}
	subIndex = CASE7;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->CycleTimes)), &size))
	{
		return 1;
	}
	subIndex = CASE8;
	type = uint32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->Voltage)), &size))
	{
		return 1;
	}
	subIndex = CASE9;
	type = int32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->Current)), &size))
	{
		return 1;
	}
	subIndex = CASE10;
	type = int16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->MosTemp)), &size))
	{
		return 1;
	}
	subIndex = CASE11;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->PcbTemp)), &size))
	{
		return 1;
	}
	type = int16;
	size = SIZE_2BYTES;
	for(i=CASE12;i<=CASE15;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(buf->CellTemp + i - CASE12), &size))
		{
			return 1;
		}
	}
	type = uint16;
	size = SIZE_2BYTES;
	for(i=CASE16;i<=CASE31;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(buf->CellVoltage + i - CASE16), &size))
		{
			return 1;
		}
	}
	subIndex = CASE34;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->OverDsgCount)), &size))
	{
		return 1;
	}
	subIndex = CASE35;
	if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(&(buf->utc)), &size))
	{
		return 1;
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	static uint8_t GetBatteryInfo3(CO_Data* d, UNS8 nodeId)
���ܣ�			����sdo���Ķ�ȡ��ز���
���룺			CO_Data* d
						UNS8 nodeId
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
static uint8_t GetBatteryInfo3(CO_Data* d, UNS8 nodeId)
{
	uint8_t type = uint32;
	BETTERY_INFO3* buf = NULL;
	uint32_t size = SIZE_4BYTES;
	uint8_t subIndex = CASE1;
	uint8_t i;
	buf = (BETTERY_INFO3*)battery_info3;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->Version)), &size))
	{
		return 1;
	}
	subIndex = CASE2;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->RemainTime)), &size))
	{
		return 1;
	}
	subIndex = CASE3;
	type = uint32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->DesignCapacity)), &size))
	{
		return 1;
	}
	subIndex = CASE4;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->RealCapacity)), &size))
	{
		return 1;
	}
	subIndex = CASE5;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->Status)), &size))
	{
		return 1;
	}
	subIndex = CASE6;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->SOC)), &size))
	{
		return 1;
	}
	subIndex = CASE7;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->CycleTimes)), &size))
	{
		return 1;
	}
	subIndex = CASE8;
	type = uint32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->Voltage)), &size))
	{
		return 1;
	}
	subIndex = CASE9;
	type = int32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->Current)), &size))
	{
		return 1;
	}
	subIndex = CASE10;
	type = int16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->MosTemp)), &size))
	{
		return 1;
	}
	subIndex = CASE11;
	if(sendSDO_app(d, nodeId, BATTERY_INFO3_ADDR, subIndex, type, (void*)(&(buf->PcbTemp)), &size))
	{
		return 1;
	}
	for(i=CASE12;i<=CASE15;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(buf->CellTemp + i - CASE12), &size))
		{
			return 1;
		}
	}
	type = uint16;
	size = SIZE_2BYTES;
	for(i=CASE16;i<=CASE33;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_INFO2_ADDR, subIndex, type, (void*)(buf->CellVotage + i - CASE16), &size))
		{
			return 1;
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	void GetBatteryInfo_app(CO_Data* d, UNS8 nodeId, uint16_t version)
���ܣ�			����sdo���Ķ�ȡ���info
���룺			CO_Data* d
						UNS8 nodeId
						uint16_t version
���أ�			0 ʧ��
						���� info_num
*****************************************************/
uint8_t GetBatteryInfo_app(CO_Data* d, UNS8 nodeId, uint16_t version)
{
	if((version>>8) < 0x0C)
	{
		if(GetBatteryInfo1(d, nodeId))
		{
			return 0;
		}
		return 1;
	}
	else if((version>>8) <= 0x10)
	{
		if(GetBatteryInfo2(d, nodeId))
		{
			return 0;
		}
		return 2;
	}
	else if((version>>8) <= 0x1C)
	{
		if(GetBatteryInfo3(d, nodeId))
		{
			return 0;
		}
		return 3;
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	static uint8_t GetLossCount(CO_Data* d, UNS8 nodeId)
���ܣ�			����sdo���Ķ�ȡ��ز���
���룺			CO_Data* d
						UNS8 nodeId
���أ�			0 �ɹ�
						1 ʧ��
*****************************************************/
static uint8_t GetLossCount(CO_Data* d, UNS8 nodeId)
{
	uint8_t type = uint32;
	BETTERY_LOSS_COUNT* buf = NULL;
	uint32_t size = SIZE_4BYTES;
	uint8_t subIndex = CASE1;
	uint8_t i;
	buf = (BETTERY_LOSS_COUNT*)battery_loss_count;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->Version)), &size))
	{
		return 1;
	}
	subIndex = CASE2;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->BatVersion)), &size))
	{
		return 1;
	}
	subIndex = CASE3;
	type = uint32;
	size = SIZE_4BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->BatStatus)), &size))
	{
		return 1;
	}
	subIndex = CASE4;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->Rserved)), &size))
	{
		return 1;
	}
	subIndex = CASE5;
	type = uint16;
	size = SIZE_2BYTES;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->Reboot)), &size))
	{
		return 1;
	}
	subIndex = CASE6;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->Cycles)), &size))
	{
		return 1;
	}
	subIndex = CASE7;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->Soh)), &size))
	{
		return 1;
	}
	subIndex = CASE8;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->DynamicSOH)), &size))
	{
		return 1;
	}
	subIndex = CASE9;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->DsgOverCurrent)), &size))
	{
		return 1;
	}
	subIndex = CASE10;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->ChgOverCurrent)), &size))
	{
		return 1;
	}
	subIndex = CASE11;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->OverTempChg)), &size))
	{
		return 1;
	}
	subIndex = CASE12;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->OverTempDsg)), &size))
	{
		return 1;
	}
	subIndex = CASE13;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->UnderTempChg)), &size))
	{
		return 1;
	}
	subIndex = CASE14;
	if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(&(buf->UnderTempDsg)), &size))
	{
		return 1;
	}
	for(i=CASE15;i<=CASE16;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(buf->CellOverVoltage + i - CASE14), &size))
		{
			return 1;
		}
	}
	for(i=CASE17;i<=CASE18;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(buf->CellUnderVoltage + i - CASE16), &size))
		{
			return 1;
		}
	}
	for(i=CASE19;i<=CASE20;i++)
	{
		subIndex = i;
		if(sendSDO_app(d, nodeId, BATTERY_LOSS_COUNT_ADDR, subIndex, type, (void*)(buf->CellError + i - CASE19), &size))
		{
			return 1;
		}
	}
	return 0;
}


/*****************************************************
����ԭ�ͣ� 	static uint8_t GetLossCount(CO_Data* d, UNS8 nodeId)
���ܣ�			����sdo���Ķ�ȡ��ز���
���룺			CO_Data* d
						UNS8 nodeId
���أ�			0 ʧ��
						��ذ汾��
*****************************************************/
uint16_t GetLossCount_app(CO_Data* d, UNS8 nodeId)
{
	if(GetLossCount(d, nodeId))
	{
		return 0;
	}
	else
	{
		return ((BETTERY_LOSS_COUNT*)battery_loss_count)->BatVersion;
	}
}



/*****************************************************
����ԭ�ͣ� 	void battery_ftl_write(UNS8 nodeId, uint8_t info_num)
���ܣ�			����ز����浽����Ƭ��nand flash
���룺			UNS8 nodeId
						uint8_t info_num ��ذ汾���
���أ�			void
*****************************************************/
/*
void battery_ftl_write(UNS8 nodeId, uint8_t info_num)
{
	BATTERY_FTL_DATA* buf;
	buf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);
	buf->COB_ID = nodeId;
	memcpy(buf->Mac, node_get_mac(nodeId), MAC_LEN);
	memcpy(buf->Data, battery_config, BATTERY_CONFIG_SIZE);
	memcpy(buf->Data+BATTERY_CONFIG_SIZE, battery_control, BATTERY_CONTROL_NUM);
	switch(info_num)
	{
		case CASE1:
			buf->DataLen = BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM+BATTERY_INFO1_NUM+BATTERY_LOSS_COUNT_NUM;
			memcpy(buf->Data+BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM, battery_info1, BATTERY_INFO1_NUM);
			memcpy(buf->Data+BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM+BATTERY_INFO1_NUM, battery_loss_count, BATTERY_LOSS_COUNT_NUM);
		break;
		case CASE2:
			buf->DataLen = BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM+BATTERY_INFO2_NUM+BATTERY_LOSS_COUNT_NUM;
			memcpy(buf->Data+BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM, battery_info2, BATTERY_INFO2_NUM);
			memcpy(buf->Data+BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM+BATTERY_INFO2_NUM, battery_loss_count, BATTERY_LOSS_COUNT_NUM);
		break;
		case CASE3:
			buf->DataLen = BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM+BATTERY_INFO3_NUM+BATTERY_LOSS_COUNT_NUM;
			memcpy(buf->Data+BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM, battery_info3, BATTERY_INFO3_NUM);
			memcpy(buf->Data+BATTERY_CONFIG_SIZE+BATTERY_CONTROL_NUM+BATTERY_INFO3_NUM, battery_loss_count, BATTERY_LOSS_COUNT_NUM);
		break;
		default:
		break;
	}
	
	FTL_WriteSectors((uint8_t*)buf,nodeId-1 , NAND_ECC_SECTOR_SIZE, 1);
}*/


/*****************************************************
����ԭ�ͣ� 	void batte_query_task(void* pvParameters)
���ܣ�			batte_query_task�߳�
���룺
���أ�			void
*****************************************************/
/*
void batte_query_task(void* pvParameters)
{
	uint8_t i;
	uint8_t mac[MAC_LEN];
	uint16_t version;
	uint8_t info_num;
	BaseType_t err=pdFALSE;
	while(1)
	{
		err=xSemaphoreTake(BinarySemaphore,portMAX_DELAY);	//��ȡ�ź���
		for(i=2;i<FIRE_NODE_MAX_NUM;i++)
		{
			if(node_check_id(i))
			{
				memcpy(mac,node_get_mac(i),MAC_LEN);
				switch(mac[0])
				{
					case BATTERY_MAC:
						GetBatteryConfig_app(&TestMaster_Data, i);
						GetBatteryControl_app(&TestMaster_Data, i);
						version = GetLossCount_app(&TestMaster_Data, i);
						info_num = GetBatteryInfo_app(&TestMaster_Data, i, version);
						battery_ftl_write(i, info_num);
					break;
					default:
					break;
				}
			}
		}
		delay_ms(1000);
	}
}*/


/*****************************************************
����ԭ�ͣ� 	void batte_query_taskCreate(void)
���ܣ�			����void batte_query_task�߳�
���룺			void
���أ�			void
*****************************************************/
/*
void batte_query_taskCreate(void)
{
	taskENTER_CRITICAL();           //�����ٽ���
  //����canOpenAnalysis_task����
	xTaskCreate((TaskFunction_t	)batte_query_task,		
										(const char* 	)"batte_query_task",		
										(uint32_t 		)batte_query_STK_SIZE,	
										(void* 		  	)NULL,				
										(UBaseType_t 	)batte_query_TASK_PRIO, 	
										(TaskHandle_t*  )&batte_queryTask_Handler);
	BinarySemaphore=xSemaphoreCreateBinary();
	taskEXIT_CRITICAL();            //�˳��ٽ���
}*/

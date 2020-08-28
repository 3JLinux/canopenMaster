#ifndef BATTERY_APP_H
#define BATTERY_APP_H

//#include "sys.h"
#include <unistd.h>
#include <stdint.h>
#include "TestMaster.h"



//��ȡ�����Ϣ����
#define CASE1			1
#define CASE2			2
#define CASE3			3
#define CASE4			4
#define CASE5			5
#define CASE6			6
#define CASE7			7
#define CASE8			8
#define CASE9			9
#define CASE10		10
#define CASE11		11
#define CASE12		12
#define CASE13		13
#define CASE14		14
#define CASE15		15
#define CASE16		16
#define CASE17		17
#define CASE18		18
#define CASE19		19
#define CASE20		20
#define CASE21		21
#define CASE22		22
#define CASE23		23
#define CASE24		24
#define CASE25		25
#define CASE26		26
#define CASE27		27
#define CASE28		28
#define CASE29		29
#define CASE30		30
#define CASE31		31
#define CASE32		32
#define CASE33		33
#define CASE34		34
#define CASE35		35
#define CASE36		36

#define WRITE_ALL_OD	0xFF

#define SIZE_4BYTES    				4
#define SIZE_2BYTES    				2
#define SIZE_1BYTES    				1

#define BATTERY_CONFIG_SIZE 	64
#define BATTERY_CONFIG_ADDR 	0x2000
#define BATTERY_CONTROL_ADDR	0x2001
#define BATTERY_INFO1_ADDR		0x2002
#define BATTERY_INFO2_ADDR		0x2003
#define BATTERY_INFO3_ADDR		0x2004
#define BATTERY_LOSS_COUNT_ADDR 0x2005


#define BATTERY_CONFIG_NUM		25
#define BATTERY_CONTROL_NUM		6
#define BATTERY_INFO1_NUM			27
#define BATTERY_INFO2_NUM			35
#define BATTERY_INFO3_NUM			37
#define BATTERY_LOSS_COUNT_NUM 55

#define BATTERY_BOOTLOADER_SUBINDEX		0x03
#define BATTERY_REBOOT_SUBINDEX				0x04
#define BATTERY_SHUTDOWN_SUBINDEX			0x05
#define BATTERY_LOWPOWER_PROTECT_SUBINDEX	0x06

#define READ_NET_TIMEOUT			300	 //300*(10ms)

#define MAC_LEN		4

#define BATTERY_MAC 0x10

typedef struct 
{
	uint32_t DesignCapacity;              //mAh���������
	uint16_t NormalVoltage;               //mV����о��Ƶ�ѹ
	uint16_t FullVoltage;                 //mV����о�����ѹ
	uint16_t CutOffVoltage;               //mV����о������ѹ
	uint16_t Series;                      //s����о����
	uint16_t DischargeRate;               //0.1C���ŵ籶��
	uint16_t ChargeRate;                  //0.1C����籶��
	int32_t ChargeCurrent;                //mA�������ֵ
	int32_t DischargeCurrent;             //mA���ŵ���ֵ
	int32_t ChargeOverCurrent;            //mA����������ֵ
	int32_t ChargeOverCurrentRecovery;    //mA���������ָ�
	int32_t DischargeOverCurrent;         //mA���ŵ������ֵ
	int32_t DischargeOverCurrentRecovery; //mA���ŵ������ֵ�ָ�
	uint16_t OverVoltage;                 //mV����ѹ��ֵ
	uint16_t OverVoltageRecovery;         //mV����ѹ�ָ�
	uint16_t UnderVoltage;                //mV��Ƿѹ��ֵ
	uint16_t UnderVoltageRecovery;        //mV��Ƿѹ�ָ�
	uint16_t BalanceThreshold;            //mV��ƽ����ֵ
	uint16_t BalanceError;                //mV��ƽ��ѹ��
	uint16_t PowerOffVoltage;             //mV���ϵ��ѹ
	uint16_t WarningSOC;                  //0.1%���������
	int16_t OverTemperture;               //0.1��C��������ֵ
	int16_t OverTempertureRecovery;       //0.1��C�����»ָ�
	int16_t UnderTemperature;             //0.1��C��������ֵ
	int16_t UnderTempertureRecovery;      //0.1��C�����»ָ�
}__attribute__((packed)) BATTERY_CONFIG;

typedef struct
{
	uint8_t ProgramStatus;
	uint8_t RunningStatus;
	uint8_t Bootloader;
	uint8_t StarupOrReboot;
	uint8_t Shutdown;
	uint8_t LowPowerPtotect;
}__attribute__((packed)) BATTERY_CONTROL;

typedef enum
{
	Bootloader=0x01,
	Application
}PROGRAM_STATUS;

typedef enum
{
	Shutdown=0x01,
	PowerOn,
	Standby,
	Init
}RUNNING_STATUS;

typedef enum
{
	CROTROL_DISABLE,
	CROTROL_ENABLE
}CROTROL_STATUS;

/* Status:
Bit[0]: ���Ƿѹ 0��������1����о��ѹ��
Bit[1]���ŵ���� 0��������1������
Bit[2]������״̬ 0��������1��������
Bit[3]�����ȱ�־ 0���رգ�1�����ڼ���
Bit[5:4]����о�¶� 0��������1���ͣ�2���ߣ�3���쳣
Bit[7:6]��PCB�¶� 0��������1���ͣ�2���ߣ�3���쳣
Bit[8]����ͷ�ɶ� 0��������1���ɶ�
Bit[9]��������־ 0��������1������������
Bit[10]��BMS�쳣 0��������1���쳣
Bit[11]��Flash״̬ 0��������1���쳣
Bit[12]��RTC״̬ 0��������1���쳣
Bit[13]��ʱ��δͬ�� 0��������1��δͬ��
Bit[14]����о״̬ 0��������1���쳣
Bit[15]���Ƚ�״̬ 0��������1���ɻ��Ƚ�
Bit[16]����о��ѹ 0��������1����о��ѹ��
Bit[17]�����ű�־ 0��������1������
Bit[18]����оΣ���¶Ⱦ��� 0��������1������
Bit[23:19]��Ԥ��
Bit[24]���͵�ѹ����״̬ 0��������1���ޱ���
Bit[25]���ɿ�ͨ��״̬ 0��������1���ж�
Bit[26]��Ԥ����״̬ 0���ޣ�1��Ԥ����
Bit[27]�������� 0��������1���ر�
Bit[28]����翪�� 0���򿪣�1���ر�
Bit[29]���ŵ翪�� 0���򿪣�1���ر�
Bit[30]���������� 0���ŵ磻1�����
Bit[31]���������־ 0���ޣ�1��������
*/
typedef struct
{
	uint16_t Version;
	uint16_t RemainTime;
	uint32_t DesignCapacity;
	uint32_t Status;
	int32_t Current;
	uint16_t CellVoltage[12];
	int16_t CellTemp[2];
	int16_t PcbTemp;
	uint16_t SOC;
	uint16_t CycleTimes;
	int16_t MosTemp;
	uint16_t OverDsgCount;
	uint8_t Rserved[2];
	uint32_t utc;
}__attribute__((packed)) BETTERY_INFO1;

typedef struct
{
	uint16_t Version;
	uint16_t RemainTime;
	uint32_t DesignCapacity;
	uint32_t RealCapacity;
	uint32_t Status;
	uint16_t SOC;
	uint16_t CycleTimes;
	uint32_t Voltage;
	int32_t Current;
	int16_t MosTemp;
	int16_t PcbTemp;
	int16_t CellTemp[4];
	int16_t CellVoltage[16];
	uint8_t Reserved[2];
	uint16_t OverDsgCount;
	uint32_t utc;
}__attribute__((packed)) BETTERY_INFO2;

typedef struct
{
	uint16_t Version;
	uint16_t RemainTime;
	uint32_t DesignCapacity;
	uint32_t RealCapacity;
	uint32_t Status;
	uint16_t SOC;
	uint16_t CycleTimes;
	uint32_t Voltage;
	int32_t Current;
	int16_t MosTemp;
	int16_t PcbTemp;
	int16_t CellTemp[4];
	uint16_t CellVotage[18];
	uint8_t Reserved[4];
}__attribute__((packed)) BETTERY_INFO3;

typedef struct
{
	uint16_t Version;
	uint16_t BatVersion;
	uint32_t BatStatus;
	uint32_t Rserved;
	uint16_t Reboot;
	uint16_t Cycles;
	uint16_t Soh;
	uint16_t DynamicSOH;
	uint16_t DsgOverCurrent;
	uint16_t ChgOverCurrent;
	uint16_t OverTempChg;
	uint16_t OverTempDsg;
	uint16_t UnderTempChg;
	uint16_t UnderTempDsg;
	uint16_t CellOverVoltage[2];
	uint16_t CellUnderVoltage[2];
	uint16_t CellError[2];
	uint8_t Reserved2[34];
	uint16_t Crc;
}__attribute__((packed)) BETTERY_LOSS_COUNT;

typedef struct
{
	uint8_t COB_ID;
	uint8_t Mac[MAC_LEN];
	uint8_t DataLen;
	uint8_t* Data;
}BATTERY_FTL_DATA;


uint8_t GetBatteryConfig_app(CO_Data* d, UNS8 nodeId);
void canOpenAnalysis_taskCreate(void);
uint8_t GetBatteryControl_app(CO_Data* d, UNS8 nodeId);
uint8_t BatteryControlBootloader(CO_Data* d, UNS8 nodeId);
uint8_t BatteryControlReboot(CO_Data* d, UNS8 nodeId);
uint8_t BatteryControlShutdown(CO_Data* d, UNS8 nodeId);
uint8_t BatteryControlLowPowerPtotect(CO_Data* d, UNS8 nodeId, CROTROL_STATUS data);

#endif




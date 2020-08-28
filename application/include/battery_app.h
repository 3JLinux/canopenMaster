#ifndef BATTERY_APP_H
#define BATTERY_APP_H

//#include "sys.h"
#include <unistd.h>
#include <stdint.h>
#include "TestMaster.h"



//读取电池信息参数
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
	uint32_t DesignCapacity;              //mAh，设计容量
	uint16_t NormalVoltage;               //mV，电芯标称电压
	uint16_t FullVoltage;                 //mV，电芯满电电压
	uint16_t CutOffVoltage;               //mV，电芯截至电压
	uint16_t Series;                      //s，电芯数量
	uint16_t DischargeRate;               //0.1C，放电倍率
	uint16_t ChargeRate;                  //0.1C，充电倍率
	int32_t ChargeCurrent;                //mA，充电阈值
	int32_t DischargeCurrent;             //mA，放电阈值
	int32_t ChargeOverCurrent;            //mA，充电过流阈值
	int32_t ChargeOverCurrentRecovery;    //mA，充电过流恢复
	int32_t DischargeOverCurrent;         //mA，放电过流阈值
	int32_t DischargeOverCurrentRecovery; //mA，放电过流阈值恢复
	uint16_t OverVoltage;                 //mV，过压阈值
	uint16_t OverVoltageRecovery;         //mV，过压恢复
	uint16_t UnderVoltage;                //mV，欠压阈值
	uint16_t UnderVoltageRecovery;        //mV，欠压恢复
	uint16_t BalanceThreshold;            //mV，平衡阈值
	uint16_t BalanceError;                //mV，平衡压差
	uint16_t PowerOffVoltage;             //mV，断电电压
	uint16_t WarningSOC;                  //0.1%，警告电量
	int16_t OverTemperture;               //0.1°C，过温阈值
	int16_t OverTempertureRecovery;       //0.1°C，过温恢复
	int16_t UnderTemperature;             //0.1°C，低温阈值
	int16_t UnderTempertureRecovery;      //0.1°C，低温恢复
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
Bit[0]: 电池欠压 0：正常；1：电芯电压低
Bit[1]：放电电流 0：正常；1：过高
Bit[2]：电量状态 0：正常；1：电量低
Bit[3]：加热标志 0：关闭；1：正在加热
Bit[5:4]：电芯温度 0：正常；1：低；2：高；3：异常
Bit[7:6]：PCB温度 0：正常；1：低；2：高；3：异常
Bit[8]：插头松动 0：正常；1：松动
Bit[9]：开机标志 0：正常；1：非正常开机
Bit[10]：BMS异常 0：正常；1：异常
Bit[11]：Flash状态 0：正常；1：异常
Bit[12]：RTC状态 0：正常；1：异常
Bit[13]：时间未同步 0：正常；1：未同步
Bit[14]：电芯状态 0：正常；1：异常
Bit[15]：迫降状态 0：正常；1：飞机迫降
Bit[16]：电芯电压 0：正常；1：电芯电压高
Bit[17]：过放标志 0：正常；1：过放
Bit[18]：电芯危险温度警告 0：正常；1：警告
Bit[23:19]：预留
Bit[24]：低电压保护状态 0：正常；1：无保护
Bit[25]：飞控通信状态 0：正常；1：中断
Bit[26]：预加热状态 0：无；1：预加热
Bit[27]：充电电流 0：正常；1：关闭
Bit[28]：充电开关 0：打开；1：关闭
Bit[29]：放电开关 0：打开；1：关闭
Bit[30]：电流方向 0：放电；1：充电
Bit[31]：充满电标志 0：无；1：充满电
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




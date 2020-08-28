#ifndef CANOPENANALYSIS_H
#define CANOPENANALYSIS_H

#include <unistd.h>
#include <stdint.h>
#include "TestMaster.h"
#include "battery_app.h"
//分配从机ID参数
#define TPDO2_CMD	0x280
#define CANOPEN_DATA_MAX_LEN	8	
#define CANOPEN_DATA_LEN_BIT	3
#define CANOPEN_DATABIT1		4
#define CANOPEN_DATABIT2		5
#define CANOPEN_DATABIT3		6
#define CANOPEN_DATABIT4		7
#define CANOPEN_DATABIT5		8
#define CANOPEN_DATABIT6		9
#define CANOPEN_DATABIT7		10
#define CANOPEN_DATABIT8		11
#define OD_SLAVE_LOGIN_HEAD		0x7E
#define OD_SLAVE_LOGIN_CMD		0X0E
#define OD_SLAVE_LOGIN_TAIL		0x7F
#define OD_SLAVE_LOGIN_INDEX	0x5000
#define OD_SLAVE_LOGIN_SUBINDEX	0x03

#define CLIENT_SDO_INDEX		0x1280
#define RECEIVE_SDO_CMD			0x580
#define TRANSMIT_SDO_CMD		0x600

#define FIFO_BUF_MAX_LEN		256
#define SLAVE_MAC_MAX_LEN		4

#define NODE_NET_CHECK_TIME_S	60

extern void canOpenAnalysis_threadCreate(void);
extern void canOpenAnalysis_threadWait(void);

#endif


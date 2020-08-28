#ifndef ENDNODEMEM_H
#define ENDNODEMEM_H

#include <unistd.h>
#include <stdint.h>

#define MAX_END_NODES			200
#define NODE_MAC_LEN			4
#define NODE_ID_LEN				1

#define NODE_IN_NET			0x01
#define NODE_OUT_NET			0x00
#define NODE_CHECK_TIMS_S		(24*3600+500)
#define NODE_DELECT_TIMS_S  (24*3600+500)

typedef struct _node_info
{
	struct _node_info *next;
	uint8_t nodeNetState;
	uint8_t ubaHWGGMacAddr[NODE_MAC_LEN];
	uint8_t nodeCANOpenID;
	unsigned long lastRevPacketTime;
}NODE_INFO;


NODE_INFO *endNodeListHead(void);
NODE_INFO * endNodeListNext(NODE_INFO *pnode);
NODE_INFO * endNodeListadd(const NODE_INFO *pcnode);
void endNodeListRemove(NODE_INFO *pnode);
void endNodeListUpdate(const uint8_t *pcNode);
void endNodeListOutLine(const uint8_t *pcNode);

void endNodeListInit(void);
void endNodeListPrint(void);
//typedef struct _fire_node
//{
//	uint8_t ubLen;
//	uint8_t ubaSrcMac[HWGG_NODE_MAC_LEN];
//	uint8_t ubaDstMac[HWGG_NODE_MAC_LEN];
//	uint8_t ubCmd;
//	uint8_t ubaData[];
//}FIRE_NODE;

uint8_t endNodeInListAddID(uint8_t *mac);
void endNodeListOutNetCheck(void);
uint8_t* endNodeListID_GetMac(const uint8_t nodeID);
uint8_t endNodeListID_check(const uint8_t nodeID);

#endif


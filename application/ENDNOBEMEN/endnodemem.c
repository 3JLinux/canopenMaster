#include "endnodemem.h"
#include "memb.h"
#include "list.h"
#include <string.h>
#include "sysprintf.h"
#include "delay.h"

MEMB(endnodemem, NODE_INFO, MAX_END_NODES);
LIST(endnodelist);
static int nodes = 0;

/*****************************************************
函数原型： 	void endNodeListInit(void)
功能：			链表初始化
输入：			无
返回：			无
*****************************************************/
void endNodeListInit(void)
{
	memb_init(&endnodemem);
	list_init(endnodelist);
	nodes = 0;
}

/*****************************************************
函数原型： 	NODE_INFO *endNodeListHead(void)
功能：			返回链表头指针
输入：			无
返回：			NODE_INFO *  链表头指针
*****************************************************/
NODE_INFO *endNodeListHead(void)
{
	return list_head(endnodelist);
}

/*---------------------------------------------------------------------------*/
/*****************************************************
函数原型： 	NODE_INFO * endNodeListNext(NODE_INFO *pnode)
功能：			链表下一个结构体指针
输入：			无
返回：			NODE_INFO *
*****************************************************/
NODE_INFO * endNodeListNext(NODE_INFO *pnode)
{
	if(pnode != NULL) 
	{
		NODE_INFO *n = list_item_next(pnode);
		return n;
	}
	return NULL;
}


/*---------------------------------------------------------------------------*/
/*****************************************************
函数原型： 	int endNodeNums(void)
功能：			链表长度
输入：			无
返回：			int nodes
*****************************************************/
int endNodeNums(void)
{
	return nodes;
}


/*****************************************************
函数原型： 	NODE_INFO * endNodeInListByMac(const uint8_t *pcMac)
功能：			遍历链表查找mac是否在链表内
输入：			const uint8_t *pcMac 设备mac
返回：			NODE_INFO * pnode 在链表所在位置指针
*****************************************************/
/*
* \note this function is used to check node is in list or not
*/
NODE_INFO * endNodeInListByMac(const uint8_t *pcMac)
{
	NODE_INFO *pnode = NULL;

	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		//mac addr and netid is same,the node was in list
		if (mem_cmp(pcMac, pnode->ubaHWGGMacAddr, NODE_MAC_LEN) == 0)
		{
			return pnode;
		}
	}

	return NULL;
}


/*****************************************************
函数原型： 	NODE_INFO * endNodeListadd(const NODE_INFO *pcnode)
功能：			将新的结构插入到链表
输入：			NODE_INFO *pcnode 需要插入的结构体
返回：			NODE_INFO *pnode 新添加结构体的链表指针
*****************************************************/
NODE_INFO * endNodeListadd(const NODE_INFO *pcnode)
{
	NODE_INFO *pnode;

	pnode = endNodeInListByMac(pcnode->ubaHWGGMacAddr);
	if (pnode != NULL)
	{
		MEM_DUMP(8, "mac", pcnode->ubaHWGGMacAddr, NODE_MAC_LEN);
		XPRINTF((8, "the node is in list\r\n"));
		//update receive packet time and node net sate
		pnode->lastRevPacketTime = clock_seconds( );
		pnode->nodeNetState = NODE_IN_NET;
		return pnode;
	}
    /* Allocate a routing entry and populate it. */
	pnode = memb_alloc(&endnodemem);
	if (pnode == NULL)
	{
		XPRINTF((6, "no space for node\r\n"));	
		return NULL;
	}
	
	//copy node info to pnode
	*pnode = *pcnode;
	pnode->lastRevPacketTime = clock_seconds( );
	pnode->nodeNetState = NODE_IN_NET;	
	/* add new routes first - assuming that there is a reason to add this
       and that there is a packet coming soon. */
    list_add(endnodelist, pnode);
    nodes++;
	return pnode;
}


/*****************************************************
函数原型： 	void endNodeListRemove(NODE_INFO *pnode)
功能：			删除链表中的结构体
输入：			NODE_INFO *pnode 要删除的结构体指针
返回：			无
*****************************************************/
void endNodeListRemove(NODE_INFO *pnode)
{
	list_remove(endnodelist, pnode);
	memb_free(&endnodemem, pnode);
	nodes--;
}


/*****************************************************
函数原型： 	void endNodeListUpdate(const uint8_t *pcNode)
功能：			更新设备状态（上线）
输入：			const uint8_t *pcNode 设备mac
返回：			无
*****************************************************/
void endNodeListUpdate(const uint8_t *pcNode)
{
	NODE_INFO *pnode = NULL;

	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		if (mem_cmp(pnode->ubaHWGGMacAddr, pcNode, NODE_MAC_LEN) == 0)
		{
			pnode->lastRevPacketTime = clock_seconds( );
			pnode->nodeNetState = NODE_IN_NET;
		}
	}
}

/*****************************************************
函数原型： 	void endNodeListUpdate(const uint8_t *pcNode)
功能：			更新设备状态（离线）
输入：			const uint8_t *pcNode 设备mac
返回：			无
*****************************************************/
void endNodeListOutLine(const uint8_t *pcNode)
{
	NODE_INFO *pnode = NULL;

	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		if (mem_cmp(pnode->ubaHWGGMacAddr, pcNode, NODE_MAC_LEN) == 0)
		{
			pnode->lastRevPacketTime = clock_seconds( );
			pnode->nodeNetState = NODE_OUT_NET;
		}
	}
}

/*****************************************************
函数原型： 	void endNodeListPeriodicCheck(void)
功能：			查询时间戳，判断设备在线离线状态
输入：			无
返回：			无
*****************************************************/
//_periodic
void endNodeListPeriodicCheck(void)
{
	NODE_INFO *pnode = NULL;

	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		if ((pnode->lastRevPacketTime + NODE_CHECK_TIMS_S) > clock_seconds())
		{
			pnode->nodeNetState = NODE_IN_NET;
		}
		else
		{
			pnode->nodeNetState = NODE_OUT_NET;
		}
	}	
}

/*****************************************************
函数原型： 	void endNodeListPrint(void)
功能：			打印链表
输入：			无
返回：			无
*****************************************************/
void endNodeListPrint(void)
{
	NODE_INFO *pnode = NULL;

	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		PRINTF("nodesta = %d\n", pnode->nodeNetState);
		PRINTF("rev time = %d\n", pnode->lastRevPacketTime);
		MEM_DUMP(0, "NMAC", pnode->ubaHWGGMacAddr, NODE_MAC_LEN);
		MEM_DUMP(0, "NID", &(pnode->nodeCANOpenID), NODE_ID_LEN);
		PRINTF("----------------------\n");
	}	
}



/*****************************************************
函数原型： 	uint8_t endNodeInListAddID(uint8_t *pcMac)
功能：			给设备分配COB-ID
输入：			uint8_t *mac 需要分配ID的设备mac
返回：			0 分配失败
						其他 分配成功的ID号
*****************************************************/
uint8_t endNodeInListAddID(uint8_t *pcMac)
{
	NODE_INFO *pnode = NULL;
	NODE_INFO *mnode = NULL;
	uint8_t node_ID = 2;
	uint8_t i;
	pnode = endNodeInListByMac(pcMac);
	if(pnode!=NULL)
	{
		if(pnode->nodeCANOpenID == 0)
		{
			for(i=0;i<MAX_END_NODES;i++)
			{
				for(mnode = endNodeListHead(); mnode != NULL; mnode = endNodeListNext(mnode)) 
				{
					if(mnode->nodeCANOpenID == node_ID)
					{
						node_ID++;
						break;
					}
				}
				if(node_ID == i+1)
				{
					pnode->nodeCANOpenID = node_ID;
					return pnode->nodeCANOpenID;
				}
			}
		}
		else
		{
			return pnode->nodeCANOpenID;
		}
	}
	return 0;
}


/*****************************************************
函数原型： 	void endNodeListOutNetCheck(void)
功能：			查询时间戳，判断设备在线离线状态,离线超过NODE_DELECT_TIMS_S则删除
输入：			无
返回：			无
*****************************************************/
void endNodeListOutNetCheck(void)
{
	NODE_INFO *pnode = NULL;
	
	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		if(pnode->nodeNetState == NODE_OUT_NET)
		{
			if((pnode->lastRevPacketTime + NODE_DELECT_TIMS_S) > clock_seconds())
			{
				endNodeListRemove(pnode);
			}
		}
		else
		{
			if ((pnode->lastRevPacketTime + NODE_CHECK_TIMS_S) > clock_seconds())
			{
				endNodeListOutLine(pnode->ubaHWGGMacAddr);
			}
//			else
//			{
//				pnode->nodeNetState = NODE_IN_NET;
//			}
		}
	}
}


/*****************************************************
函数原型： 	void endNodeListID_InNet(const uint8_t nodeID)
功能：			设备上线（根据COB-ID号进行状态更新）
输入：			const uint8_t nodeID
返回：			无
*****************************************************/
void endNodeListID_InNet(const uint8_t nodeID)
{
	NODE_INFO *pnode = NULL;

	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		if (pnode->nodeCANOpenID == nodeID)
		{
			pnode->lastRevPacketTime = clock_seconds( );
			pnode->nodeNetState = NODE_IN_NET;
			break;
		}
	}
}


/*****************************************************
函数原型： 	uint8_t* endNodeListID_GetMac(const uint8_t nodeID)
功能：			根据设备id查询设备mac
输入：			const uint8_t nodeID
返回：			uint8_t* 设备mac指针
*****************************************************/
uint8_t* endNodeListID_GetMac(const uint8_t nodeID)
{
	NODE_INFO *pnode = NULL;
	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		if (pnode->nodeCANOpenID == nodeID)
		{
			return pnode->ubaHWGGMacAddr;
		}
	}
	return NULL;
}


/*****************************************************
函数原型： 	uint8_t* endNodeListID_GetMac(const uint8_t nodeID)
功能：			查询id是否存在链表
输入：			const uint8_t nodeID
返回：			0 不存在
						其他 COB-ID
*****************************************************/
uint8_t endNodeListID_check(const uint8_t nodeID)
{
	NODE_INFO *pnode = NULL;
	for(pnode = endNodeListHead(); pnode != NULL; pnode = endNodeListNext(pnode)) 
	{
		if (pnode->nodeCANOpenID == nodeID)
		{
			if(pnode->nodeNetState == NODE_IN_NET)
			{
				return pnode->nodeCANOpenID;
			}
		}
	}
	return 0;
}

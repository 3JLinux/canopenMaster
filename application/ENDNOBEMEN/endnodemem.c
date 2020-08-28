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
����ԭ�ͣ� 	void endNodeListInit(void)
���ܣ�			�����ʼ��
���룺			��
���أ�			��
*****************************************************/
void endNodeListInit(void)
{
	memb_init(&endnodemem);
	list_init(endnodelist);
	nodes = 0;
}

/*****************************************************
����ԭ�ͣ� 	NODE_INFO *endNodeListHead(void)
���ܣ�			��������ͷָ��
���룺			��
���أ�			NODE_INFO *  ����ͷָ��
*****************************************************/
NODE_INFO *endNodeListHead(void)
{
	return list_head(endnodelist);
}

/*---------------------------------------------------------------------------*/
/*****************************************************
����ԭ�ͣ� 	NODE_INFO * endNodeListNext(NODE_INFO *pnode)
���ܣ�			������һ���ṹ��ָ��
���룺			��
���أ�			NODE_INFO *
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
����ԭ�ͣ� 	int endNodeNums(void)
���ܣ�			������
���룺			��
���أ�			int nodes
*****************************************************/
int endNodeNums(void)
{
	return nodes;
}


/*****************************************************
����ԭ�ͣ� 	NODE_INFO * endNodeInListByMac(const uint8_t *pcMac)
���ܣ�			�����������mac�Ƿ���������
���룺			const uint8_t *pcMac �豸mac
���أ�			NODE_INFO * pnode ����������λ��ָ��
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
����ԭ�ͣ� 	NODE_INFO * endNodeListadd(const NODE_INFO *pcnode)
���ܣ�			���µĽṹ���뵽����
���룺			NODE_INFO *pcnode ��Ҫ����Ľṹ��
���أ�			NODE_INFO *pnode ����ӽṹ�������ָ��
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
����ԭ�ͣ� 	void endNodeListRemove(NODE_INFO *pnode)
���ܣ�			ɾ�������еĽṹ��
���룺			NODE_INFO *pnode Ҫɾ���Ľṹ��ָ��
���أ�			��
*****************************************************/
void endNodeListRemove(NODE_INFO *pnode)
{
	list_remove(endnodelist, pnode);
	memb_free(&endnodemem, pnode);
	nodes--;
}


/*****************************************************
����ԭ�ͣ� 	void endNodeListUpdate(const uint8_t *pcNode)
���ܣ�			�����豸״̬�����ߣ�
���룺			const uint8_t *pcNode �豸mac
���أ�			��
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
����ԭ�ͣ� 	void endNodeListUpdate(const uint8_t *pcNode)
���ܣ�			�����豸״̬�����ߣ�
���룺			const uint8_t *pcNode �豸mac
���أ�			��
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
����ԭ�ͣ� 	void endNodeListPeriodicCheck(void)
���ܣ�			��ѯʱ������ж��豸��������״̬
���룺			��
���أ�			��
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
����ԭ�ͣ� 	void endNodeListPrint(void)
���ܣ�			��ӡ����
���룺			��
���أ�			��
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
����ԭ�ͣ� 	uint8_t endNodeInListAddID(uint8_t *pcMac)
���ܣ�			���豸����COB-ID
���룺			uint8_t *mac ��Ҫ����ID���豸mac
���أ�			0 ����ʧ��
						���� ����ɹ���ID��
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
����ԭ�ͣ� 	void endNodeListOutNetCheck(void)
���ܣ�			��ѯʱ������ж��豸��������״̬,���߳���NODE_DELECT_TIMS_S��ɾ��
���룺			��
���أ�			��
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
����ԭ�ͣ� 	void endNodeListID_InNet(const uint8_t nodeID)
���ܣ�			�豸���ߣ�����COB-ID�Ž���״̬���£�
���룺			const uint8_t nodeID
���أ�			��
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
����ԭ�ͣ� 	uint8_t* endNodeListID_GetMac(const uint8_t nodeID)
���ܣ�			�����豸id��ѯ�豸mac
���룺			const uint8_t nodeID
���أ�			uint8_t* �豸macָ��
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
����ԭ�ͣ� 	uint8_t* endNodeListID_GetMac(const uint8_t nodeID)
���ܣ�			��ѯid�Ƿ��������
���룺			const uint8_t nodeID
���أ�			0 ������
						���� COB-ID
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

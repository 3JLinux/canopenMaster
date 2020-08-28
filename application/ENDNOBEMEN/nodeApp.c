#include "nodeApp.h"
#include "endnodemem.h"
#include "string.h"
#include "delay.h"
#include "sysprintf.h"




typedef struct
{
	uint8_t ubaNodeAddr[4];
}FIRE_NODE_ADDR;

//typedef struct _fire_node_data
//{
//	uint8_t ublen;
//	uint8_t ubadata[127];
//}FIRE_NODE_DATA;

//static FIRE_NODE_DATA stFireData;

typedef struct
{
	uint16_t node_num;    
	FIRE_NODE_ADDR nodeArray[FIRE_NODE_MAX_NUM];
}FIRE_NODE_INFO;

static FIRE_NODE_INFO fire_node_info;

#define DEC_MAC_LEN 4

static void addFireNodeToList(const uint8_t *pcFrame);
static bool nodeBelognToMe(const NODE_INFO *pcNode);
static void removeNodeNotBelongToMe(void);


/*****************************************************
����ԭ�ͣ� 	void fire_node_info_test_init(void)
���ܣ�			��ʼ���豸�б�
���룺			��
���أ�			��
*****************************************************/
void fire_node_info_init(void)
{
	fire_node_info.node_num = 0;
	memset(fire_node_info.nodeArray[0].ubaNodeAddr,0,DEC_MAC_LEN);
	endNodeListInit();
}

/*****************************************************
����ԭ�ͣ� 	int8_t update_fire_node_info(uint8_t* write_dec_mac,uint8_t dec_mac_total_num)
���ܣ�			�����б�����
���룺			uint8_t* write_dec_mac    д���macָ��
						uint8_t dec_mac_total_num mac����
���أ�			int8_t ����д���mac������ʧ�ܷ���-1
*****************************************************/
int8_t update_fire_node_info(uint8_t* write_dec_mac,uint8_t dec_mac_total_num)
{
	uint8_t i;
	if((write_dec_mac != NULL) && (dec_mac_total_num>0))
	{
		for(i=0;i<dec_mac_total_num;i++)
		{
			memcpy(fire_node_info.nodeArray[i].ubaNodeAddr,write_dec_mac+i*4,DEC_MAC_LEN);
		}
		fire_node_info.node_num = dec_mac_total_num;
		for(i=0;i<(dec_mac_total_num);i++)
		{
			addFireNodeToList(fire_node_info.nodeArray[i].ubaNodeAddr);
		}
		
		removeNodeNotBelongToMe();
		return fire_node_info.node_num;
	}
	return -1;
}

/*****************************************************
����ԭ�ͣ� 	extern uint8_t get_node_mac_total_num(void)
���ܣ�			�����б�mac����������rs485���ã�
���룺			��
���أ�		  uint8_t fire_node_info->node_num
*****************************************************/
extern uint8_t get_node_mac_total_num(void)
{
	return fire_node_info.node_num;
}

/*****************************************************
����ԭ�ͣ� 	extern uint8_t get_node_mac(uint8_t* dec_mac,uint8_t dec_num)
���ܣ�			��ȡ�б���mac����rs485���ã�
���룺			uint8_t* dec_mac ��Ż�ȡmac��ָ��
						uint8_t dec_num mac��ţ��ڼ�����
���أ�			uint8_t fire_node_info->node_num - dec_num - 1 ���滹���ڵ�mac����
*****************************************************/
extern uint8_t get_node_mac(uint8_t* dec_mac,uint8_t dec_num)
{
	if((dec_num+1<=fire_node_info.node_num) && dec_mac != NULL)
	{
		memcpy(dec_mac,fire_node_info.nodeArray[dec_num].ubaNodeAddr,DEC_MAC_LEN);
		return fire_node_info.node_num - dec_num - 1;
	}
	return 0;
}

/*****************************************************
����ԭ�ͣ� 	const void* extgdbdevGetDeviceSettingInfoSt(void)
���ܣ�			��ȡ�б�ָ��
���룺			��
���أ�		  void*
*****************************************************/
const static void* extgdbdevGetDeviceSettingInfoSt(void)
{
	return &fire_node_info;
}


/*****************************************************
����ԭ�ͣ� 	extern bool nodeBelognToMeByMac(const uint8_t *pcMac)
���ܣ�			�Ƿ��б��ڵ�mac
���룺		  const uint8_t *pcMac �豸mac
���أ�		  bool true/false
*****************************************************/
extern bool nodeBelognToMeByMac(const uint8_t *pcMac)
{
	const FIRE_NODE_INFO *pfireNodeInfo = (const FIRE_NODE_INFO *)extgdbdevGetDeviceSettingInfoSt();
	int i = 0;

	//if node information docment is save in flash, all node need to save to list
//	if (pfireNodeInfo->node_num == 0)
//		return true;

	for (i = 0; i < pfireNodeInfo->node_num; i++)
	{
		if (mem_cmp(pcMac, pfireNodeInfo->nodeArray[i].ubaNodeAddr, NODE_MAC_LEN) == 0)
		{
			return true;
		}
	}
	return false;
}

/*****************************************************
����ԭ�ͣ� 	extern void node_dec_online(const uint8_t *pcMac)
���ܣ�			�豸����
���룺			const uint8_t *pcMac �豸mac
���أ�		  ��
*****************************************************/
extern void node_dec_online(const uint8_t *pcMac)
{
	endNodeListUpdate(pcMac);
}

/*****************************************************
����ԭ�ͣ� 	extern void node_dec_outline(const uint8_t *pcMac)
���ܣ�			�豸����
���룺			const uint8_t *pcMac �豸mac
���أ�		  ��
*****************************************************/
extern void node_dec_outline(const uint8_t *pcMac)
{
	endNodeListOutLine(pcMac);
}


/*****************************************************
����ԭ�ͣ� 	static void addFireNodeToList(const uint8_t *pcFrame)
���ܣ�			��������
���룺			const uint8_t *pcFrame �豸mac
���أ�		  ��
*****************************************************/
static void addFireNodeToList(const uint8_t *pcFrame)
{
	NODE_INFO stnodeInfo;
	const uint8_t *pFrame = NULL;

	if (pcFrame == NULL)
		return;

	pFrame = (const uint8_t *)pcFrame;

	stnodeInfo.lastRevPacketTime = clock_seconds( );
	stnodeInfo.next = NULL;
	stnodeInfo.nodeNetState = NODE_IN_NET;
	memcpy(stnodeInfo.ubaHWGGMacAddr, pFrame, NODE_MAC_LEN);
	stnodeInfo.nodeCANOpenID = 0;
	
	endNodeListadd((const NODE_INFO *)&stnodeInfo);
}

/*****************************************************
����ԭ�ͣ� 	static bool nodeBelognToMe(const NODE_INFO *pcNode)
���ܣ�			��ѯ�б��е�mac�Ƿ������豸�б�
���룺			const NODE_INFO *pcNode ����ָ��
���أ�		  bool
*****************************************************/
static bool nodeBelognToMe(const NODE_INFO *pcNode)
{
	const FIRE_NODE_INFO *pfireNodeInfo = (const FIRE_NODE_INFO *)extgdbdevGetDeviceSettingInfoSt();
	int i = 0;

	//if node information docment is save in flash, all node need to save to list
	if (pfireNodeInfo->node_num == 0)
		return true;

	for (i = 0; i < pfireNodeInfo->node_num; i++)
	{
		if (mem_cmp(pcNode->ubaHWGGMacAddr, pfireNodeInfo->nodeArray[i].ubaNodeAddr, NODE_MAC_LEN) == 0)
		{
			return true;
		}
	}

	return false;
}

/*****************************************************
����ԭ�ͣ� 	static void removeNodeNotBelongToMe(void)
���ܣ�			ɾ���������豸�б������ָ��
���룺			��
���أ�		  ��
*****************************************************/
static void removeNodeNotBelongToMe(void)
{
	NODE_INFO *pnode = NULL;
	bool isTrue = false;

	const FIRE_NODE_INFO *pfireNodeInfo = (const FIRE_NODE_INFO *)extgdbdevGetDeviceSettingInfoSt();

	//not have node info docment
	if (pfireNodeInfo->node_num == 0)
		return;

	for(pnode = (NODE_INFO *)endNodeListHead(); pnode != NULL; pnode = (NODE_INFO *)endNodeListNext(pnode)) 
	{
		isTrue = nodeBelognToMe((const NODE_INFO *)pnode);
		if (!isTrue)
		{
			endNodeListRemove(pnode);
		}
	}
}


//static bool nodeIsNotInList(const uint8_t * pcMac)
//{
//	NODE_INFO *pnode = NULL;
//	
//	for(pnode = (NODE_INFO *)endNodeListHead(); pnode != NULL; pnode = (NODE_INFO *)endNodeListNext(pnode))
//	{
//		if (mem_cmp(pnode->ubaHWGGMacAddr, pcMac, NODE_MAC_LEN) == 0)
//		{
//			return false;
//		}
//	}

//	return true;
//}

void fillNotNetNodeInfo(uint8_t* data)
{
	uint8_t dev_num;
	const FIRE_NODE_INFO *pfireNodeInfo = (const FIRE_NODE_INFO *)extgdbdevGetDeviceSettingInfoSt();
	//const FIRE_NODE_INFO *pFireNode = (const FIRE_NODE_INFO *)extgdbdevGetDeviceSettingInfoSt(LABLE_FIRE_NODE_INFO);	
	int i = 0;
	NODE_INFO *pnode = NULL;
//	int j = 0; 


	//clear fire node info
	if (data != NULL)
	{
		memset(data, 0, sizeof(200));
	}

	if (pfireNodeInfo->node_num == 0)
	{
		data = NULL;
		return ;
	}

	for(pnode = (NODE_INFO *)endNodeListHead(); pnode != NULL; pnode = (NODE_INFO *)endNodeListNext(pnode))
	{
		if (pnode->nodeNetState == NODE_OUT_NET)
		{
			memcpy(data+2+i*NODE_MAC_LEN, pnode->ubaHWGGMacAddr, NODE_MAC_LEN);
			XPRINTF((10, "i1 = %d\n", i));
			MEM_DUMP(10, "OUT1", pnode->ubaHWGGMacAddr, NODE_MAC_LEN);
		}
	}

	/*node is in flash but not in ram list, is not in net*/
//	for (j = 0; j < pfireNodeInfo->node_num; j++)
//	{
//		if ( nodeIsNotInList((const uint8_t*)pfireNodeInfo->nodeArray[j].ubaNodeAddr))
//		{
//			memcpy(pFireInfo->nodeArray[i++].ubaNodeAddr, pfireNodeInfo->nodeArray[j].ubaNodeAddr, NODE_MAC_LEN);
//			XPRINTF((10, "i = %d\n", i));
//			MEM_DUMP(10, "OUT", pfireNodeInfo->nodeArray[j].ubaNodeAddr, NODE_MAC_LEN);
//		}
//	}

	dev_num = i;
	data[0] = dev_num;
	data[1] = 0;
}


/*****************************************************
����ԭ�ͣ� 	void node_printf(void)
���ܣ�			��ӡ�б�
���룺			��
���أ�		  ��
*****************************************************/
void node_printf(void)
{
	endNodeListPrint();
}


/*****************************************************
����ԭ�ͣ� 	uint8_t node_getID(uint8_t* pcMac)
���ܣ�			����mac��ȡ�豸ID
���룺			��
���أ�		  ��
*****************************************************/
uint8_t node_getID(uint8_t* pcMac)
{
	return endNodeInListAddID(pcMac);
}


/*****************************************************
����ԭ�ͣ� 	void node_addMAC(uint8_t* pcMac)
���ܣ�			��mac��ӵ�����
���룺			��
���أ�		  ��
*****************************************************/
void node_addMAC(uint8_t* pcMac)
{
	addFireNodeToList(pcMac);
}


/*****************************************************
����ԭ�ͣ� 	void node_check_in_out_net(void)
���ܣ�			��ѯʱ������ж��豸��������״̬,���߳���NODE_DELECT_TIMS_S��ɾ��
���룺			��
���أ�		  ��
*****************************************************/
void node_check_in_out_net(void)
{
	endNodeListOutNetCheck();
}


/*****************************************************
����ԭ�ͣ� 	uint8_t* node_get_mac(const uint8_t nodeID)
���ܣ�			�����豸id��ѯ�豸mac
���룺			const uint8_t nodeID
���أ�		  uint8_t* �豸macָ��
*****************************************************/
uint8_t* node_get_mac(const uint8_t nodeID)
{
	return endNodeListID_GetMac(nodeID);
}

/*****************************************************
����ԭ�ͣ� 	uint8_t node_check_id(const uint8_t nodeID)
���ܣ�			��ѯ�豸id�Ƿ����������
���룺			const uint8_t nodeID
���أ�		  0 ������
						���� �豸id
*****************************************************/
uint8_t node_check_id(const uint8_t nodeID)
{
	endNodeListID_check(nodeID);
}


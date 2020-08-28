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
函数原型： 	void fire_node_info_test_init(void)
功能：			初始化设备列表
输入：			无
返回：			无
*****************************************************/
void fire_node_info_init(void)
{
	fire_node_info.node_num = 0;
	memset(fire_node_info.nodeArray[0].ubaNodeAddr,0,DEC_MAC_LEN);
	endNodeListInit();
}

/*****************************************************
函数原型： 	int8_t update_fire_node_info(uint8_t* write_dec_mac,uint8_t dec_mac_total_num)
功能：			更新列表数据
输入：			uint8_t* write_dec_mac    写入的mac指针
						uint8_t dec_mac_total_num mac个数
返回：			int8_t 返回写入的mac个数，失败返回-1
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
函数原型： 	extern uint8_t get_node_mac_total_num(void)
功能：			返回列表mac总数量（供rs485调用）
输入：			无
返回：		  uint8_t fire_node_info->node_num
*****************************************************/
extern uint8_t get_node_mac_total_num(void)
{
	return fire_node_info.node_num;
}

/*****************************************************
函数原型： 	extern uint8_t get_node_mac(uint8_t* dec_mac,uint8_t dec_num)
功能：			获取列表内mac（供rs485调用）
输入：			uint8_t* dec_mac 存放获取mac的指针
						uint8_t dec_num mac序号（第几个）
返回：			uint8_t fire_node_info->node_num - dec_num - 1 后面还存在的mac个数
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
函数原型： 	const void* extgdbdevGetDeviceSettingInfoSt(void)
功能：			获取列表指针
输入：			无
返回：		  void*
*****************************************************/
const static void* extgdbdevGetDeviceSettingInfoSt(void)
{
	return &fire_node_info;
}


/*****************************************************
函数原型： 	extern bool nodeBelognToMeByMac(const uint8_t *pcMac)
功能：			是否列表内的mac
输入：		  const uint8_t *pcMac 设备mac
返回：		  bool true/false
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
函数原型： 	extern void node_dec_online(const uint8_t *pcMac)
功能：			设备上线
输入：			const uint8_t *pcMac 设备mac
返回：		  无
*****************************************************/
extern void node_dec_online(const uint8_t *pcMac)
{
	endNodeListUpdate(pcMac);
}

/*****************************************************
函数原型： 	extern void node_dec_outline(const uint8_t *pcMac)
功能：			设备离线
输入：			const uint8_t *pcMac 设备mac
返回：		  无
*****************************************************/
extern void node_dec_outline(const uint8_t *pcMac)
{
	endNodeListOutLine(pcMac);
}


/*****************************************************
函数原型： 	static void addFireNodeToList(const uint8_t *pcFrame)
功能：			更新链表
输入：			const uint8_t *pcFrame 设备mac
返回：		  无
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
函数原型： 	static bool nodeBelognToMe(const NODE_INFO *pcNode)
功能：			查询列表中的mac是否属于设备列表
输入：			const NODE_INFO *pcNode 链表指针
返回：		  bool
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
函数原型： 	static void removeNodeNotBelongToMe(void)
功能：			删除不属于设备列表的链表指针
输入：			无
返回：		  无
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
函数原型： 	void node_printf(void)
功能：			打印列表
输入：			无
返回：		  无
*****************************************************/
void node_printf(void)
{
	endNodeListPrint();
}


/*****************************************************
函数原型： 	uint8_t node_getID(uint8_t* pcMac)
功能：			根据mac获取设备ID
输入：			无
返回：		  无
*****************************************************/
uint8_t node_getID(uint8_t* pcMac)
{
	return endNodeInListAddID(pcMac);
}


/*****************************************************
函数原型： 	void node_addMAC(uint8_t* pcMac)
功能：			将mac添加到链表
输入：			无
返回：		  无
*****************************************************/
void node_addMAC(uint8_t* pcMac)
{
	addFireNodeToList(pcMac);
}


/*****************************************************
函数原型： 	void node_check_in_out_net(void)
功能：			查询时间戳，判断设备在线离线状态,离线超过NODE_DELECT_TIMS_S则删除
输入：			无
返回：		  无
*****************************************************/
void node_check_in_out_net(void)
{
	endNodeListOutNetCheck();
}


/*****************************************************
函数原型： 	uint8_t* node_get_mac(const uint8_t nodeID)
功能：			根据设备id查询设备mac
输入：			const uint8_t nodeID
返回：		  uint8_t* 设备mac指针
*****************************************************/
uint8_t* node_get_mac(const uint8_t nodeID)
{
	return endNodeListID_GetMac(nodeID);
}

/*****************************************************
函数原型： 	uint8_t node_check_id(const uint8_t nodeID)
功能：			查询设备id是否存在链表内
输入：			const uint8_t nodeID
返回：		  0 不存在
						其他 设备id
*****************************************************/
uint8_t node_check_id(const uint8_t nodeID)
{
	endNodeListID_check(nodeID);
}


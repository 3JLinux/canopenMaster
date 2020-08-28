/*
This file is part of CanFestival, a library implementing CanOpen Stack. 


Copyright (C): Edouard TISSERANT and Francis DUPIN


See COPYING file for copyrights details.


This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.


This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.


You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/*!
** @file   sync.c
** @author Edouard TISSERANT and Francis DUPIN
** @date   Tue Jun  5 09:32:32 2007
**
** @brief
**
**
*/

#include "data.h"
#include "sync.h"
#include "canfestival.h"
#include "sysdep.h"

/* Prototypes for internals functions */

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param id                                                                                       
**/  
void SyncAlarm(CO_Data* d, UNS32 id);
UNS32 OnCOB_ID_SyncUpdate(CO_Data* d, const indextable * unsused_indextable, 
	UNS8 unsused_bSubindex);

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param id                                                                                       
**/   
void SyncAlarm(CO_Data* d, UNS32 id)
{
	sendSYNC(d) ;
}

/*!                                                                                                
** This is called when Index 0x1005 is updated.                                                                                                
**                                                                                                 
** @param d                                                                                        
** @param unsused_indextable                                                                       
** @param unsused_bSubindex                                                                        
**                                                                                                 
** @return                                                                                         
**/  
UNS32 OnCOB_ID_SyncUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	startSYNC(d);
	return 0;
}

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
**/ 
void startSYNC(CO_Data* d)
{
	if(d->syncTimer != TIMER_NONE){		// 如果之前有同步回调
		stopSYNC(d);					// 删除当前回调任务
	}

	RegisterSetODentryCallBack(d, 0x1005, 0, &OnCOB_ID_SyncUpdate);
	RegisterSetODentryCallBack(d, 0x1006, 0, &OnCOB_ID_SyncUpdate);

	if(*d->COB_ID_Sync & 0x40000000ul && *d->Sync_Cycle_Period) // 判断同步周期是否为零，ID是否为0x40000000？这个地方会成立么？
	{
		d->syncTimer = SetAlarm(								// 创建一个回调函数，用于发送同步段信息
				d,
				0 /*No id needed*/,
				&SyncAlarm,
				US_TO_TIMEVAL(*d->Sync_Cycle_Period), 
				US_TO_TIMEVAL(*d->Sync_Cycle_Period));
	}
}

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
**/   
void stopSYNC(CO_Data* d)
{
    RegisterSetODentryCallBack(d, 0x1005, 0, NULL);
    RegisterSetODentryCallBack(d, 0x1006, 0, NULL);
	d->syncTimer = DelAlarm(d->syncTimer);
}


/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param cob_id                                                                                   
**                                                                                                 
** @return                                                                                         
**/  
UNS8 sendSYNCMessage(CO_Data* d)
{
  Message m;
  
  MSG_WAR(0x3001, "sendSYNC ", 0);
  
  m.cob_id = (UNS16)UNS16_LE(*d->COB_ID_Sync);
  m.rtr = NOT_A_REQUEST;
  m.len = 0;
  
  return canSend(d->canHandle,&m);
}


/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param cob_id                                                                                   
**                                                                                                 
** @return                                                                                         
**/  
UNS8 sendSYNC(CO_Data* d)
{
  UNS8 res;
  res = sendSYNCMessage(d);		// 发送同步段信息
  proceedSYNC(d) ; 
  return res ;
}

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param m                                                                                        
**                                                                                                 
** @return                                                                                         
**/ 
UNS8 proceedSYNC(CO_Data* d)
{

  UNS8 res;
  
  MSG_WAR(0x3002, "SYNC received. Proceed. ", 0);
  
  (*d->post_sync)(d);

  /* only operational state allows PDO transmission */
  if(! d->CurrentCommunicationState.csPDO) 		//若为非操作状态下则直接返回
    return 0;

  res = _sendPDOevent(d, 1/*isSyncEvent*/ );
  
  /*Call user app callback*/
  (*d->post_TPDO)(d);
  
  return res;
  
}


void _post_sync(CO_Data* d){}
void _post_TPDO(CO_Data* d){}

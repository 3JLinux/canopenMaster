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
** @file   timer.c
** @author Edouard TISSERANT and Francis DUPIN
** @date   Tue Jun  5 09:32:32 2007
**
** @brief
**
**
*/

/* #define DEBUG_WAR_CONSOLE_ON */
/* #define DEBUG_ERR_CONSOLE_ON */

#include <applicfg.h>
#include "timers.h"

/*  ---------  The timer table --------- */
s_timer_entry timers[MAX_NB_TIMER] = {{TIMER_FREE, NULL, NULL, 0, 0, 0},};

TIMEVAL total_sleep_time = TIMEVAL_MAX;
TIMER_HANDLE last_timer_raw = -1;

#define min_val(a,b) ((a<b)?a:b)

/*!
** -------  Use this to declare a new alarm ------
**
** @param d
** @param id
** @param callback
** @param value
** @param period
**
** @return
**/
TIMER_HANDLE SetAlarm(CO_Data* d, UNS32 id, TimerCallback_t callback, TIMEVAL value, TIMEVAL period)
{
	TIMER_HANDLE row_number;
	s_timer_entry *row;

	/* in order to decide new timer setting we have to run over all timer rows */
	for(row_number=0, row=timers; row_number <= last_timer_raw + 1 && row_number < MAX_NB_TIMER; row_number++, row++)
	{
		if (callback && 	/* if something to store */
		   row->state == TIMER_FREE) /* and empty row */   // 判断 函数指针是否不为空，判断是否无回调任务 -- 找到一个空的任务载体
		{	/* just store */
			TIMEVAL real_timer_value;
			TIMEVAL elapsed_time;

			if (row_number == last_timer_raw + 1) last_timer_raw++; // 如果row_number大于之前最大的编号 那么将编号自加一次

			elapsed_time = getElapsedTime();						// 获取上一次进入中断后过去的时间
			/* set next wakeup alarm if new entry is sooner than others, or if it is alone */
			real_timer_value = value;
			real_timer_value = min_val(real_timer_value, TIMEVAL_MAX);	// 去小值

			if (total_sleep_time > elapsed_time && total_sleep_time - elapsed_time > real_timer_value) //若完整休眠时间大于elapsed_time 且 加上设置的值后还是小于
			{
				total_sleep_time = elapsed_time + real_timer_value;	// 将完整休眠时间重新设置，并写入
				setTimer(real_timer_value);
			}
			row->callback = callback;								// 将回调函数防入任务载体中
			row->d = d;
			row->id = id;
			row->val = value + elapsed_time;						// 重新设置进入中断周期，即上一次进入中断后多长时间进入下一次中断，即回调函数间隔执行时间
			row->interval = period;									// 用于设定该回调任务为周期任务还是执行一次任务
			row->state = TIMER_ARMED;
			return row_number;
		}
	}

	return TIMER_NONE;
}

/*!
**  -----  Use this to remove an alarm ----
**
** @param handle
**
** @return
**/
TIMER_HANDLE DelAlarm(TIMER_HANDLE handle)
{
	/* Quick and dirty. system timer will continue to be trigged, but no action will be preformed. */
	MSG_WAR(0x3320, "DelAlarm. handle = ", handle);
	if(handle != TIMER_NONE)
	{
		if(handle == last_timer_raw)
			last_timer_raw--;
		timers[handle].state = TIMER_FREE;		// 除能对应函数
	}
	return TIMER_NONE;
}

/*!
** ------  TimeDispatch is called on each timer expiration ----
**
**/
int tdcount=0;
void TimeDispatch(void)
{
	TIMER_HANDLE i;
	TIMEVAL next_wakeup = TIMEVAL_MAX; /* used to compute when should normaly occur next wakeup */
	/* First run : change timer state depending on time */
	/* Get time since timer signal */
	UNS32 overrun = (UNS32)getElapsedTime();	//计算中断到当前语句经过的时间

	TIMEVAL real_total_sleep_time = total_sleep_time + overrun;		//获取真正休眠时间

	s_timer_entry *row;

	for(i=0, row = timers; i <= last_timer_raw; i++, row++)
	{
		if (row->state & TIMER_ARMED) /* if row is active */
		{
			if (row->val <= real_total_sleep_time) /* to be trigged */ // 真正休眠时间大于设定的时间
			{
				if (!row->interval) /* if simply outdated */ //为0 即为非周期性的回调执行后就使能，只执行一次
				{
					row->state = TIMER_TRIG; /* ask for trig */	// state：2 为执行一次任务  state：3周期执行任务
				}
				else /* or period have expired */
				{
					/* set val as interval, with 32 bit overrun correction, */
					/* modulo for 64 bit not available on all platforms     */
					row->val = row->interval - (overrun % (UNS32)row->interval);// 重新给val赋值确保实时性  此为计算overrun对回调函数的时间影响
					row->state = TIMER_TRIG_PERIOD; /* ask for trig, periodic */
					/* Check if this new timer value is the soonest */
					if(row->val < next_wakeup)		// 如果当前val值小于下一次进入中断的值
						next_wakeup = row->val;		// 将下一次进入中断的值修改为val
				}
			}
			else
			{
				/* Each armed timer value in decremented. */
				row->val -= real_total_sleep_time;		//如果有回调函数时间没到 则减掉真正休眠的时间，即计算下一次进入中断所需要的时间

				/* Check if this new timer value is the soonest */
				if(row->val < next_wakeup)
					next_wakeup = row->val;
			}
		}
	}

	/* Remember how much time we should sleep. */
	total_sleep_time = next_wakeup;

	/* Set timer to soonest occurence */
	setTimer(next_wakeup);// 设置下一次进入中断的时间

	/* Then trig them or not. */	// 运行回调函数，即是否启动回调函数 如：ConsumerHeartbeatAlarm ProducerHeartbeatAlarm，
	for(i=0, row = timers; i<=last_timer_raw; i++, row++)
	{
		if (row->state & TIMER_TRIG)						// 是否存在回调函数
		{
			row->state &= ~TIMER_TRIG; /* reset trig state (will be free if not periodic) */	// 如果为2则运行一次，清零  下次执行if (row->state & TIMER_ARMED)时为否
			if(row->callback)								// 判断函数指针是否有效
				(*row->callback)(row->d, row->id); /* trig ! */
		}
	}
}

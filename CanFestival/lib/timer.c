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
		   row->state == TIMER_FREE) /* and empty row */   // �ж� ����ָ���Ƿ�Ϊ�գ��ж��Ƿ��޻ص����� -- �ҵ�һ���յ���������
		{	/* just store */
			TIMEVAL real_timer_value;
			TIMEVAL elapsed_time;

			if (row_number == last_timer_raw + 1) last_timer_raw++; // ���row_number����֮ǰ���ı�� ��ô������Լ�һ��

			elapsed_time = getElapsedTime();						// ��ȡ��һ�ν����жϺ��ȥ��ʱ��
			/* set next wakeup alarm if new entry is sooner than others, or if it is alone */
			real_timer_value = value;
			real_timer_value = min_val(real_timer_value, TIMEVAL_MAX);	// ȥСֵ

			if (total_sleep_time > elapsed_time && total_sleep_time - elapsed_time > real_timer_value) //����������ʱ�����elapsed_time �� �������õ�ֵ����С��
			{
				total_sleep_time = elapsed_time + real_timer_value;	// ����������ʱ���������ã���д��
				setTimer(real_timer_value);
			}
			row->callback = callback;								// ���ص�������������������
			row->d = d;
			row->id = id;
			row->val = value + elapsed_time;						// �������ý����ж����ڣ�����һ�ν����жϺ�೤ʱ�������һ���жϣ����ص��������ִ��ʱ��
			row->interval = period;									// �����趨�ûص�����Ϊ����������ִ��һ������
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
		timers[handle].state = TIMER_FREE;		// ���ܶ�Ӧ����
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
	UNS32 overrun = (UNS32)getElapsedTime();	//�����жϵ���ǰ��侭����ʱ��

	TIMEVAL real_total_sleep_time = total_sleep_time + overrun;		//��ȡ��������ʱ��

	s_timer_entry *row;

	for(i=0, row = timers; i <= last_timer_raw; i++, row++)
	{
		if (row->state & TIMER_ARMED) /* if row is active */
		{
			if (row->val <= real_total_sleep_time) /* to be trigged */ // ��������ʱ������趨��ʱ��
			{
				if (!row->interval) /* if simply outdated */ //Ϊ0 ��Ϊ�������ԵĻص�ִ�к��ʹ�ܣ�ִֻ��һ��
				{
					row->state = TIMER_TRIG; /* ask for trig */	// state��2 Ϊִ��һ������  state��3����ִ������
				}
				else /* or period have expired */
				{
					/* set val as interval, with 32 bit overrun correction, */
					/* modulo for 64 bit not available on all platforms     */
					row->val = row->interval - (overrun % (UNS32)row->interval);// ���¸�val��ֵȷ��ʵʱ��  ��Ϊ����overrun�Իص�������ʱ��Ӱ��
					row->state = TIMER_TRIG_PERIOD; /* ask for trig, periodic */
					/* Check if this new timer value is the soonest */
					if(row->val < next_wakeup)		// �����ǰvalֵС����һ�ν����жϵ�ֵ
						next_wakeup = row->val;		// ����һ�ν����жϵ�ֵ�޸�Ϊval
				}
			}
			else
			{
				/* Each armed timer value in decremented. */
				row->val -= real_total_sleep_time;		//����лص�����ʱ��û�� ������������ߵ�ʱ�䣬��������һ�ν����ж�����Ҫ��ʱ��

				/* Check if this new timer value is the soonest */
				if(row->val < next_wakeup)
					next_wakeup = row->val;
			}
		}
	}

	/* Remember how much time we should sleep. */
	total_sleep_time = next_wakeup;

	/* Set timer to soonest occurence */
	setTimer(next_wakeup);// ������һ�ν����жϵ�ʱ��

	/* Then trig them or not. */	// ���лص����������Ƿ������ص����� �磺ConsumerHeartbeatAlarm ProducerHeartbeatAlarm��
	for(i=0, row = timers; i<=last_timer_raw; i++, row++)
	{
		if (row->state & TIMER_TRIG)						// �Ƿ���ڻص�����
		{
			row->state &= ~TIMER_TRIG; /* reset trig state (will be free if not periodic) */	// ���Ϊ2������һ�Σ�����  �´�ִ��if (row->state & TIMER_ARMED)ʱΪ��
			if(row->callback)								// �жϺ���ָ���Ƿ���Ч
				(*row->callback)(row->d, row->id); /* trig ! */
		}
	}
}

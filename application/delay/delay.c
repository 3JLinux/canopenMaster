#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "delay.h"
#include <stdio.h>

static volatile uint32_t current_seconds = 0;
static struct itimerval oldtv;
static void set_timer(void)
{
	struct itimerval itv;
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, &oldtv);
}


void shut_timer(void)
{
	struct itimerval itv;
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, &oldtv);
}


void signal_handler(uint32_t m)
{
	current_seconds++;
	printf("current_seconds:%d\r\n",current_seconds);
}


void delay_timer_init(void)
{
	signal(SIGALRM, (__sighandler_t)signal_handler);
	set_timer();
}


uint32_t clock_seconds(void)
{
	return current_seconds;
}

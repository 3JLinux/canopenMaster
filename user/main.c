#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

//#include "canfestival.h"
#include "canopen_app.h"
#include "TestMaster.h"

#include <pthread.h>
#include <sys/time.h>
#include <stdint.h>
#include "canopenAnalysis.h"
#include "canopen_fifo.h"


pthread_t running_stop_thread_flag;

void* running_stop_thread(void* ptr)
{
	signal(SIGINT, SIG_DFL);
	pause();
}

void running_stop_thread_create(void)
{
	int temp;
	memset(&running_stop_thread_flag, 0, sizeof(running_stop_thread_flag));
	if((temp = pthread_create(&running_stop_thread_flag,NULL,running_stop_thread,NULL)) != 0)
	{
		printf("running_stop_thread create ERROR!\r\n");
	}
	else
	{
		printf("running_stop_thread create SUCCESS!\r\n");
	}
	
}


void running_stop_thread_wait(void)
{
	if(running_stop_thread_flag != 0)
	{
		pthread_join(running_stop_thread_flag, NULL);
		printf("running_stop_thread END\r\n");
	}
}


int main(int argc,char **argv)
{
	canOpenFifoInit();
	canopen_app_init(argc, argv);
	canopen_app_thread_create();
	SDK_process_pthread();
	running_stop_thread_create();
	canOpenAnalysis_threadCreate();
	canopen_app_thread_wait();
	return 0;
}

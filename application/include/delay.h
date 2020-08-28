#ifndef DELAY_H
#define DELAY_H

#include <unistd.h>
#include <stdint.h>

#define delay_ms(a) usleep((a)*1000L)
#define delay_s(a)	sleep(a)

void shut_timer(void);
void delay_timer_init(void);
uint32_t clock_seconds(void);

#endif



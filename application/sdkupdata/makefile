#! gmake


CC = arm-linux-gnueabihf-gcc
OPT_CFLAGS = -O2
CFLAGS = $(OPT_CFLAGS)
PROG_CFLAGS =  -DDEBUG_WAR_CONSOLE_ON -DDEBUG_ERR_CONSOLE_ON -DDEBUG_MSG_CONSOLE_ON -g
OS_NAME = unix
ARCH_NAME = arm
C99 = -std=gnu99
#PREFIX = /home/jjj/Desktop/CanFestival-3-de1fc3261f21/myinstall
TARGET = unix
#CAN_DRIVER = can_socket
TIMERS_DRIVER = sdkup
INCLUDES = -I./ -I../include -I../../CanFestival/include/ -I../../CanFestival/include/$(TARGET) -I../../CanFestival/include/timers_unix  -I../../CanFestival/src/mkf

OBJS = $(TIMERS_DRIVER).o

#SRC_HFILES = ../../include/$(TIMERS_DRIVER)/timerscfg.h

#TARGET_HFILES = $(DESTDIR)$(PREFIX)/include/canfestival/timerscfg.h

all: driver

driver: $(OBJS)

%.o: %.c
	$(CC) $(C99) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ -c $<

#install:
#	mkdir -p $(DESTDIR)$(PREFIX)/include/canfestival
#	cp $(SRC_HFILES) $(DESTDIR)$(PREFIX)/include/canfestival

uninstall:
	rm -f $(TARGET_HFILES)

clean:
	rm -f $(OBJS)

mrproper: clean


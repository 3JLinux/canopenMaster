#! gmake

CC = arm-linux-gnueabihf-gcc
OPT_CFLAGS = -O2
CFLAGS = $(OPT_CFLAGS)
PROG_CFLAGS =  -DDEBUG_WAR_CONSOLE_ON -DDEBUG_ERR_CONSOLE_ON -DDEBUG_MSG_CONSOLE_ON -g
OS_NAME = unix
ARCH_NAME = arm
C99 = -std=gnu99
BINUTILS_PREFIX = arm-linux-gnueabihf-

PREFIX = ../../install

TARGET = unix

APPLICATION_01 = battery_app
APPLICATION_02 = canopenAnalysis
APPLICATION_03 = memb
APPLICATION_04 = delay
APPLICATION_05 = endnodemem
APPLICATION_06 = nodeApp
APPLICATION_07 = canopen_fifo
APPLICATION_08 = fifo
APPLICATION_09 = list
APPLICATION_10 = CRC
APPLICATION_11 = sdkup
APPLICATION_12 = sysprintf
APPLICATION_13 = xprintf
APPLICATION_14 = 
APPLICATION_15 = 
APPLICATION_16 = 
APPLICATION_17 = 
APPLICATION_18 = 

PATH_APP_01 = ../canopen
PATH_APP_02 = ../COMMON
PATH_APP_03 = ../delay
PATH_APP_04 = ../ENDNOBEMEN
PATH_APP_05 = ../FIFO
PATH_APP_06 = ../LIST
PATH_APP_07 = ../sdkupdata
PATH_APP_08 = ../sysprintf
PATH_APP_09 = ../sysprintf/xprintf
PATH_APP_10 = 
PATH_APP_11 = 
PATH_APP_12 = 
PATH_APP_13 = 
PATH_APP_14 = 
PATH_APP_15 = 
PATH_APP_16 = 
PATH_APP_17 = 
PATH_APP_18 = 

INCLUDE_01 = ../include
INCLUDE_02 = ../../CanFestival/include
INCLUDE_03 = ../../CanFestival/include/$(TARGET) 
INCLUDE_04 = ../../CanFestival/include/timers_unix
INCLUDE_05 = ../../CanFestival/src/mkf
INCLUDE_06 = ../../CanFestival/src
INCLUDE_07 = ../../WriteLog
INCLUDE_08 = 
INCLUDE_09 = 

#PREFIX = /home/jjj/Desktop/CanFestival-3-de1fc3261f21/myinstall

#CAN_DRIVER = can_socket
INCLUDES = -I$(INCLUDE_01) -I$(INCLUDE_02) -I$(INCLUDE_03) -I$(INCLUDE_04) -I$(INCLUDE_05) -I$(INCLUDE_06) -I$(INCLUDE_07)

OBJS = $(APPLICATION_01).o $(APPLICATION_02).o $(APPLICATION_03).o $(APPLICATION_04).o $(APPLICATION_05).o $(APPLICATION_06).o\
		$(APPLICATION_07).o $(APPLICATION_08).o $(APPLICATION_09).o $(APPLICATION_10).o $(APPLICATION_11).o $(APPLICATION_12).o\
		$(APPLICATION_13).o 

#SRC_HFILES = ../../include/$(TIMERS_DRIVER)/timerscfg.h

#TARGET_HFILES = $(DESTDIR)$(PREFIX)/include/canfestival/timerscfg.h
vpath %.c $(PATH_APP_01):$(PATH_APP_02):$(PATH_APP_03):$(PATH_APP_04):$(PATH_APP_05):$(PATH_APP_06):$(PATH_APP_07):$(PATH_APP_08):$(PATH_APP_09)


all: apploction

apploction: libapplication.a

libapplication.a: $(OBJS)
	@echo " "
	@echo "*********************************************"
	@echo "**Building [libapplication.a]"
	@echo "*********************************************"
	$(BINUTILS_PREFIX)ar rc $@ $(OBJS)
	$(BINUTILS_PREFIX)ranlib $@


%.o: %.c
	@echo " "
	@echo "*********************************************"
	@echo "**Compiling $^ -> $@"
	@echo "*********************************************"
	$(CC) $(C99) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ -c $^

install:
	mkdir -p $(DESTDIR)$(PREFIX)/lib_static
	cp libapplication.a $(DESTDIR)$(PREFIX)/lib_static

uninstall:
	rm -f $(TARGET_HFILES)

clean:
#	rm -f $(OBJS)

mrproper: clean


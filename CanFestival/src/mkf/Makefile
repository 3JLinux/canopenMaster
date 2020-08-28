#! gmake

#
# Copyright (C) 2006 Laurent Bessard
# 
# This file is part of canfestival, a library implementing the canopen
# stack
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# 

CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
LD = arm-linux-gnueabihf-g++
OPT_CFLAGS = -O2
CFLAGS = $(OPT_CFLAGS)
PROG_CFLAGS =  -DDEBUG_WAR_CONSOLE_ON -DDEBUG_ERR_CONSOLE_ON -DDEBUG_MSG_CONSOLE_ON -g
#EXE_CFLAGS =  -lpthread -lrt -ldl
OS_NAME = unix
ARCH_NAME = arm
PREFIX = /home/yzh/XAG/CanFestival/myinstall
PREFIXS = /home/yzh/XAG/can_open
TARGET = unix
UNIX_PATH = $(TARGET)/mkf
CAN_DRIVER = can_socket
CAN_PATH = $(CAN_DRIVER)/mkf
LIB_PATH = lib/mkf
TIMERS_DRIVER = timers_unix
TIMERS_PATH = $(TIMERS_DRIVER)/mkf
#CANOPEN_APP = "canopen_app"
#CANOPEN_APP = canopen_app.o
INCLUDES = -I./ -I../../include -I../../include/$(TARGET) -I../../include/$(CAN_DRIVER) -I../../include/$(TIMERS_DRIVER)

MASTER_OBJS = TestMaster.o canopen_app.o

#OBJS = $(MASTER_OBJS) ../../$(LIB_PATH)/libcanfestival.a ../../driver/$(UNIX_PATH)/libcanfestival_$(TARGET).a

vpath %.c ..
vpath %.od ..

#ifeq ($(TARGET),win32)
#	CANOPEN_APP = "canopen_app.exe"
#endif

#ifeq ($(TIMERS_DRIVER),timers_win32)
#	EXE_CFLAGS =
#endif

#all: $(CANOPEN_APP) 
# 这是要先编译一下库么？先跳入unix 中make一下
#../../driver/$(UNIX_PATH)/libcanfestival_$(TARGET).a:
#	$(MAKE) -C ../../driver/$(UNIX_PATH) libcanfestival_$(TARGET).a 	


#$(CANOPEN_APP): $(OBJS)
#	$(LD) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ $(OBJS) $(EXE_CFLAGS)
all: $(MASTER_OBJS)
	
TestMaster.c: TestMaster.od 
	$(MAKE) -C ../../objdictgen gnosis
	python ../../objdictgen/objdictgen.py TestMaster.od TestMaster.c

%.o: %.c
	$(CC) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ -c $<

clean:
	rm -f $(MASTER_OBJS)
#	rm -f $(CANOPEN_APP)

mrproper: clean
	rm -f TestMaster.c
	
#install: canopen_app
#	mkdir -p $(DESTDIR)$(PREFIX)/bin/
#	cp $< $(DESTDIR)$(PREFIX)/bin/
#	cp $< $(DESTDIR)$(PREFIXS)
#uninstall:
#	rm -f $(DESTDIR)$(PREFIX)/bin/canopen_app



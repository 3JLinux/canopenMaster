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
OPT_CFLAGS = -O2
CFLAGS = $(OPT_CFLAGS)
PROG_CFLAGS =  -DDEBUG_WAR_CONSOLE_ON -DDEBUG_ERR_CONSOLE_ON -DDEBUG_MSG_CONSOLE_ON -g
OS_NAME = unix
ARCH_NAME = arm
PREFIX = /home/jjj/Desktop/CanFestival-3-de1fc3261f21/myinstall
TARGET = unix
CAN_DRIVER = can_socket
TIMERS_DRIVER = timers_unix

INCLUDES = -I../../include -I../../include/$(TARGET) -I../../include/$(CAN_DRIVER) -I../../include/$(TIMERS_DRIVER)

OBJS = $(TIMERS_DRIVER).o

SRC_HFILES = ../../include/$(TIMERS_DRIVER)/timerscfg.h

TARGET_HFILES = $(DESTDIR)$(PREFIX)/include/canfestival/timerscfg.h

all: driver

driver: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ -c $<

install:
	mkdir -p $(DESTDIR)$(PREFIX)/include/canfestival
	cp $(SRC_HFILES) $(DESTDIR)$(PREFIX)/include/canfestival

uninstall:
	rm -f $(TARGET_HFILES)

clean:
	rm -f $(OBJS)

mrproper: clean



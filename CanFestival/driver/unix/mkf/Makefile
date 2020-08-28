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

#ifneq ($(KERNELRELEASE),)
# Kbuild part of Makefile
# 设置变量
obj-m := libcanfestival_$(TARGET).o
libcanfestival_$(TARGET)-objs := $(OBJS)

#else
# Normal Makefile
CC = arm-linux-gnueabihf-gcc
OPT_CFLAGS = -O2
PROG_CFLAGS =  -DDEBUG_WAR_CONSOLE_ON -DDEBUG_ERR_CONSOLE_ON -DDEBUG_MSG_CONSOLE_ON -g
OS_NAME = unix
ARCH_NAME = arm
PREFIX = /home/yzh/XAG/CanFestival/myinstall
TARGET = unix
CAN_DRIVER = can_socket
CAN_PATH = $(CAN_DRIVER)/mkf
TIMERS_DRIVER = timers_unix
TIMERS_PATH = $(TIMERS_DRIVER)/mkf
ENABLE_DLL_DRIVERS = 1
BINUTILS_PREFIX = arm-linux-gnueabihf-

INCLUDES = -I../../../include -I../../../include/$(TARGET) -I../../../include/$(TIMERS_DRIVER) -I../../../../application/include

OBJS = $(TARGET).o

vpath %.c ../

# add timers driver if any
# 判断是不是有time驱动如果没有则再OBJS中添加time的.o文件
ifneq ($(TIMERS_DRIVER),timers_)
OBJS += ../../$(TIMERS_PATH)/$(TIMERS_DRIVER).o
endif
# 创建变量路径未
SRC_HFILES = ../../../include/$(TARGET)/applicfg.h ../../../include/$(TARGET)/canfestival.h

TARGET_HFILES = $(DESTDIR)$(PREFIX)/include/$(TARGET)/applicfg.h $(DESTDIR)$(PREFIX)/include/$(TARGET)/canfestival.h

all: driver

#ifeq ($(TIMERS_DRIVER), timers_kernel)
#USE_KERNEL_TIMER = true
#endif
#ifeq ($(TIMERS_DRIVER), timers_kernel_xeno)
#USE_KERNEL_TIMER = true
#endif

#ifeq ($(USE_KERNEL_TIMER), true)
#OBJS := $(shell echo $(OBJS) | sed "s:$(TARGET)_::g")
#EXTRA_CFLAGS := $(shell echo $(INCLUDES) | sed "s:-I:-I$(src)/:g") # 没看懂 这冒号是什么鬼啊 冒号可能跟/号有相同功能
#EXTRA_CFLAGS += $(PROG_CFLAGS)
#KERNELDIR := /home/jjj/linux/IMX6ULL/linux/alientek_linux
#export TARGET	# 传递变量到下一级
#export OBJS
#export EXTRA_CFLAGS

#driver: libcanfestival_$(TARGET)

#libcanfestival_$(TARGET):
#	@echo Building [libcanfestival_$(TARGET).o]
#	先跳到KERNELDIR目录下执行makefile 之后再跳回当前目录继续执行makefile
#	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules

#clean:
#	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean
#	rm -f Module.symvers

#install:


#else
CFLAGS = $(OPT_CFLAGS)	# 关键字 优化编译

driver: libcanfestival_$(TARGET).a

	$(MAKE) -C ../../$(CAN_PATH)
	$(MAKE) -C ../../$(CAN_PATH)
# 编译
%.o: %.c
	
	$(CC) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ -c $<

../../$(TIMERS_PATH)/$(TIMERS_DRIVER).o:
	$(MAKE) -C ../../$(TIMERS_PATH)

../../$(CAN_PATH)/$(CAN_DRIVER).o:
	$(MAKE) -C ../../$(CAN_PATH)

ifneq ($(ENABLE_DLL_DRIVERS),1)
OBJS += ../../$(CAN_PATH)/$(CAN_DRIVER).o # can_socket.o timers_driver.o unix.o
endif
libcanfestival_$(TARGET).a: $(OBJS)	# 定义库的依赖
	@echo Building [libcanfestival_$(TARGET).a]
	$(BINUTILS_PREFIX)ar rc $@ $(OBJS)	
	$(BINUTILS_PREFIX)ranlib $@

install: libcanfestival_$(TARGET).a
	mkdir -p $(DESTDIR)$(PREFIX)/lib/
	mkdir -p $(DESTDIR)$(PREFIX)/include/canfestival
	cp libcanfestival_$(TARGET).a $(DESTDIR)$(PREFIX)/lib/
	cp $(SRC_HFILES) $(DESTDIR)$(PREFIX)/include/canfestival

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libcanfestival_$(TARGET).a
	rm -f $(TARGET_HFILES)

clean:
	rm -f $(OBJS)
	rm -f libcanfestival_$(TARGET).a

#endif
mrproper: clean

#endif

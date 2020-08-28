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

ifneq ($(KERNELRELEASE),)
# Kbuild part of Makefile
obj-m := canfestival.o
canfestival-objs := $(OBJS)

else
# Normal Makefile
CC = arm-linux-gnueabihf-gcc
PROG_CFLAGS =  -DDEBUG_WAR_CONSOLE_ON -DDEBUG_ERR_CONSOLE_ON -DDEBUG_MSG_CONSOLE_ON -g
OS_NAME = unix
ARCH_NAME = arm
PREFIX = ../../../install
BINUTILS_PREFIX = arm-linux-gnueabihf-
TARGET = unix
CAN_DRIVER = can_socket
CAN_PATH = $(CAN_DRIVER)/mkf
TIMERS_DRIVER = timers_unix
TIMERS_PATH = $(TIMERS_DRIVER)/mkf
ENABLE_LSS = 0

INCLUDES = -I../../include -I../../include/$(TARGET) -I../../include/$(TIMERS_DRIVER) #-I../drivers/$(TARGET)

OBJS = $(TARGET)_objacces.o $(TARGET)_lifegrd.o $(TARGET)_sdo.o\
	    $(TARGET)_pdo.o $(TARGET)_sync.o $(TARGET)_nmtSlave.o $(TARGET)_nmtMaster.o $(TARGET)_states.o $(TARGET)_timer.o $(TARGET)_dcf.o $(TARGET)_emcy.o

vpath %.c ..

ifeq ($(ENABLE_LSS),1)
OBJS += $(TARGET)_lss.o
endif

# # # # Target specific paramters # # # #

#ifeq ($(TARGET),hcs12)		don't use
#OPT_CFLAGS = -Os
#PROGDEFINES = -mnoshort -Wall -Wmissing-prototypes -fno-strict-aliasing
#endif

ifeq ($(TARGET),unix)
OPT_CFLAGS = -O2
endif

# # # # Options # # # #

all: canfestival

#ifeq ($(TIMERS_DRIVER), timers_kernel)
#USE_KERNEL_TIMER = true
#endif
#ifeq ($(TIMERS_DRIVER), timers_kernel_xeno)
#USE_KERNEL_TIMER = true
#endif

#ifeq ($(USE_KERNEL_TIMER), true)
#OBJS := $(shell echo $(OBJS) | sed "s:$(TARGET)_::g")
#OBJS += symbols.o
#OBJS += ../drivers/unix/libcanfestival_$(TARGET).o
#EXTRA_CFLAGS := $(shell echo $(INCLUDES) | sed "s:-I:-I$(src)/:g")
#EXTRA_CFLAGS += $(PROG_CFLAGS)
#KERNELDIR := /home/jjj/linux/IMX6ULL/linux/alientek_linux
#export OBJS
#export EXTRA_CFLAGS

#canfestival:
#	@echo " "
#	@echo "*********************************************"
#	@echo "**Building [canfestival.o]"
#	@echo "*********************************************"
#	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules
#
#clean:
#	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean
#	rm -f Module.symvers

#install:
#	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules_install
#	mkdir -p $(DESTDIR)$(PREFIX)/include/canfestival
#	cp ../include/*.h $(DESTDIR)$(PREFIX)/include/canfestival

#uninstall:
#	rm -rf $(DESTDIR)$(PREFIX)/include/canfestival

#else
CFLAGS = $(OPT_CFLAGS)

ifeq ($(TARGET),none)
canfestival: libcanfestival.o
else
canfestival: libcanfestival.a
endif

libcanfestival.a: $(OBJS)
	@echo " "
	@echo "*********************************************"
	@echo "**Building [libcanfestival.a]"
	@echo "*********************************************"
	$(BINUTILS_PREFIX)ar rc $@ $(OBJS)
	$(BINUTILS_PREFIX)ranlib $@

libcanfestival.o: $(OBJS)
	@echo " "
	@echo "*********************************************"
	@echo "**Prelink [libcanfestival.o]"
	@echo "*********************************************"
	$(BINUTILS_PREFIX)ld -r $(OBJS) -o $@

$(TARGET)_%.o: %.c
	@echo " "
	@echo "*********************************************"
	@echo "**Compiling $< -> $@"
	@echo "*********************************************"
	$(CC) $(CFLAGS) $(PROG_CFLAGS) ${PROGDEFINES} $(INCLUDES) -o $@ -c $<

install: libcanfestival.a
	mkdir -p $(DESTDIR)$(PREFIX)/lib_static
#	mkdir -p $(DESTDIR)$(PREFIX)/include/canfestival
	cp libcanfestival.a $(DESTDIR)$(PREFIX)/lib_static
#	cp ../include/*.h $(DESTDIR)$(PREFIX)/include/canfestival

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib_static/libcanfestival.a
#	rm -rf $(DESTDIR)$(PREFIX)/include/canfestival

clean:
	rm -f $(OBJS) libcanfestival.a libcanfestival.o

#endif
mrproper: clean

endif

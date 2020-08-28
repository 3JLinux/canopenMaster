#! gmake

CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
LD = arm-linux-gnueabihf-g++
EXE_CFLAGS = 
INCLUDE = -I../include

DELAY_OBJS = delay.o

all: $(DELAY_OBJS)

$(DELAY_OBJS):*.c
	$(CC) -c $^ $(EXE_CFLAGS) $(INCLUDE)

clean:
	rm -f $(DELAY_OBJS)


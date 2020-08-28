#! gmake

CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
LD = arm-linux-gnueabihf-g++
EXE_CFLAGS = 
INCLUDE = -I../include

OBJS = endnodemem.o

all: $(OBJS)

$(OBJS): *.c
	$(CC) $(INCLUDE) -c $^ $(EXE_CFLAGS)

clean:
	rm -f $(OBJS)


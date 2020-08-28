#! gmake

CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
LD = arm-linux-gnueabihf-g++
EXE_CFLAGS = 
INCLUDE = -I../inlcude

OBJS = list.o


all: $(OBJS)

$(OBJS): *.c
	$(CC) -c $^ $(EXE_CFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJS)


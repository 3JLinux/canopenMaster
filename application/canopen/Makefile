#! gmake

CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
LD = arm-linux-gnueabihf-g++
EXE_CFLAGS = -lpthread -lrt -ldl 
TARGET = unix
CAN_DRIVER = can_socket
TIMERS_DRIVER = timers_unix
INCLUDE = -I../include -I../../CanFestival/examples/personal_example -I../../CanFestival/include -I../../CanFestival/include/$(TARGET) -I../../CanFestival/include/$(CAN_DRIVER) -I../../CanFestival/include/$(TIMERS_DRIVER) 

MASTER_OBJS = ../../CanFestival/src/libcanfestival.a ../../CanFestival/drivers/$(TARGET)/libcanfestival_$(TARGET).a

OBJS = canopenAnalysis.o battery_app.o

all: $(OBJS)

$(OBJS): *.c
	$(CC) $(INCLUDE) -c $^ $(EXE_CFLAGS)
	
clean:
	rm -f $(OBJS)


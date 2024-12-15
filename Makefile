CC = gcc

CFLAGS = -pthread

TARGET = bus_simulation

SRCS = main.c queues.c

OBJS = main.o queues.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


main.o: main.c queues.h
	$(CC) $(CFLAGS) -c main.c


queues.o: queues.c queues.h
	$(CC) $(CFLAGS) -c queues.c


clean:
	rm -f $(OBJS) $(TARGET)


rebuild: clean all
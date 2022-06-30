.SUFFIXES : .c .o

OBJECTS = main.o read.o write.o
TARGET = test

CC = gcc
CFLAGS = -g

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

%.o : io.h %.c
	$(CC) -c $^

clean :
	rm -rf $(OBJECTS) $(TARGET) text.txt

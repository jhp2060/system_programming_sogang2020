CC = gcc
CFLAGS = -W -Wall
TARGET = 20140825.out
OBJECTS = 20140825.o shell_commands.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm *.o
	rm *.out

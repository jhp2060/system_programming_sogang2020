CC = gcc
CFLAGS = -W -Wall
TARGET = 20140825.out
OBJECTS = 20140825.o helpers.o shell_commands.o 

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	rm *.o

clean :
	rm *.out

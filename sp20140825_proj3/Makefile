CC = gcc
CFLAGS = -W -Wall
TARGET = 20140825.out
OBJECTS = 20140825.o helpers.o shell_commands.o memory_commands.o opcode_commands.o assembler_commands.o loader_commands.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm *.o
	rm *.out

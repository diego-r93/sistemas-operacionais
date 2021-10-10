# the compiler
CC = gcc
LD = gcc

# compiler flags:
# don't work with std=c99 or std=c11 
CFLAGS = -Wall
LFLAGS = -Wall

# the build target executable:
TARGET = Trabalho1

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	rm -f *.o $(TARGET)	
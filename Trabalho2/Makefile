# the compiler
CC = gcc
LD = gcc

# compiler flags:
CFLAGS = -Wall -Werror -D_POSIX_THREAD_SEMANTICS
LFLAGS = -pthread

# the build target executable:
TARGET = Trabalho2

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c -lpthread

clean:
	rm -f *.o $(TARGET)	
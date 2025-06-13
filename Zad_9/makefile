CC = gcc
CFLAGS = -Wall -pedantic -std=c99
TARGET = a
SRC = mutex_wykluczenie.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

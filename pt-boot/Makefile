CC ?= gcc
CFLAGS ?= -Wall -O2
TARGET = pt-boot

all: $(TARGET)

$(TARGET): pt-boot.c
	$(CC) $(CFLAGS) -o $(TARGET) pt-boot.c

clean:
	rm -f $(TARGET)

.PHONY: all clean


CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O3 -march=native -flto
LDFLAGS = -lm

TARGET = sieve
SOURCES = main.c sieve.c
HEADERS = sieve.h
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

# Convenience targets for testing
test-small: $(TARGET)
	./$(TARGET) 100

test-medium: $(TARGET)
	./$(TARGET) 1000000

test-large: $(TARGET)
	./$(TARGET) 10000000

test-output: $(TARGET)
	./$(TARGET) 1000 primes_1000.txt
	@echo "First 10 primes in output file:"
	@head -10 primes_1000.txt


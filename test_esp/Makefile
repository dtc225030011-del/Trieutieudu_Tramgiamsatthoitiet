CC := gcc
CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -Iunity -Iinclude
LDLIBS := -lm

SOURCES := unity/unity.c \
           src/esp_sensors.c \
           src/esp_control.c \
           src/esp_connectivity.c \
           test/test_bmp280.c \
           test/test_tsl2561.c \
           test/test_control.c \
           test/test_connectivity.c \
           test/test_main.c

TARGET := run_esp_tests

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(LDLIBS) -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o test_results.log

CHIP=atmega8
F_CPU=1000000

CC=avr-gcc
CFLAGS=-mmcu=$(CHIP) -g -DF_CPU=$(F_CPU) -Wall -Os -Werror -Wextra -std=c99
LDFLAGS=-mmcu=$(CHIP)

#CFLAGS=-g -O2 -Wall -Wextra -rdynamic -DNDEBUG $(OPTFLAGS)
#LIBS=-ldl $(OPTLIBS)

SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))
TESTS_OBJECTS=$(patsubst tests/%_tests.c,%.o,$(TEST_SRC))

TARGET=main

# The Target Build
all: hex

$(TARGET): $(OBJECTS)

hex: $(TARGET)
	avr-objcopy -j .text -j .data -O ihex $(TARGET) $(TARGET).flash.hex

# The Unit Tests
.PHONY: tests
tests: CC = cc
tests: CFLAGS=-g -O2 -Wall -Wextra -I.
tests: LDFLAGS=
tests: $(TESTS)
	sh ./tests/runtests.sh

$(TESTS): $(TESTS_OBJECTS)

$(TESTS_OBJECTS): .FORCE

valgrind:
	VALGRIND="valgrind --error-exitcode=1 --leak-check=full --log-file=/tmp/valgrind-%p.log" $(MAKE) tests

# The Cleaner
clean:
	rm -rf $(OBJECTS) $(TESTS) $(TARGET) $(TARGET).lst $(TARGET).flash.hex
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

upload: all
	avrdude -c ft232c -p m8 -U flash:w:$(TARGET).flash.hex:i

# The Checker
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions.
	@egrep $(BADFUNCS) $(SOURCES) || true

.FORCE:

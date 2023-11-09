ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
  else # in a bash-like shell, like msys
	CLEANUP = rm -f
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
	TARGET_EXTENSION=.out
endif

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif

UNITY_ROOT=Unity
UNITYFLAGS += -DUNITY_OUTPUT_COLOR -DCB_DEBUG=1

CFLAGS=-std=c99 -Wall -Wextra -Wpointer-arith -Wcast-align -Wwrite-strings \
		-Wswitch-default -Wunreachable-code -Winit-self -Wmissing-field-initializers \
		-Wno-unknown-pragmas -Wstrict-prototypes -Wundef -Wold-style-definition

TARGET_BASE=test_cbuffer
TARGET = $(TARGET_BASE)$(TARGET_EXTENSION)
SRC_FILES=$(UNITY_ROOT)/src/unity.c $(wildcard ../src/*.c) $(wildcard ./*.c) 
INC_DIRS=-I../src -I$(UNITY_ROOT)/src
SYMBOLS=

all: clean default

default: $(SRC_FILES)
	@ $(C_COMPILER) $(CFLAGS) $(UNITYFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES) -o $(TARGET)
	@ - ./$(TARGET)

clean:
	@ $(CLEANUP) $(TARGET)

ci: CFLAGS += -Werror
ci: default

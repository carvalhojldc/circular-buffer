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

CFLAGS=

TARGET_BASE=static-inertion
TARGET = $(TARGET_BASE)$(TARGET_EXTENSION)
SRC_FILES=$(wildcard ../../src/*.c) $(wildcard ./*.c) 
INC_DIRS=-I../../src
SYMBOLS=

all: clean default

default: $(SRC_FILES)
	@ $(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES) -o $(TARGET)
	@ - ./$(TARGET)

clean:
	@ $(CLEANUP) $(TARGET)

ci: CFLAGS += -Werror
ci: default

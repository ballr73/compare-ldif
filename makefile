CC = g++
CFLAGS = -std=c++17 -Wall

BIN = bin
TARGET = cldif
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

all: clean build
build: clean
	echo $@
	mkdir $(BIN)
	$(CC) $(SOURCES) -o $(BIN)/$(TARGET)
clean:
	echo $@
	rm -rf $(BIN)
# the compiler to use, gcc for c and g++ for c++
CC = g++

# vars:
RM = rm -rf
BUILD_DIR=_build

# compiler flags:
# -g     -for adding debugging information to the executable file
# -wall  -for turning on most compiler warnings
CFLAGS = -g -Wall

# the target to build
TARGET = example

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	[ -d $(BUILD_DIR) ] || mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(TARGET).cpp

clean:
	$(RM) _build

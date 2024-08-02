TARGET=rasterizer
SRC_DIRS=src
BUILD_DIR=./build
INCLUDE_DIRS=C:/msys64/mingw64/include
LIB_DIRS=C:/msys64/mingw64/lib

CC=gcc
OPT=
# TODO: debug vs release builds to remove -g
CFLAGS=-Wall -Wextra -Werror -std=c99 -pedantic -g $(foreach X,$(INCLUDE_DIRS),-I$(X)) $(OPT)

CFILES=$(foreach X,$(SRC_DIRS),$(wildcard $(X)/*.c))
#OBJECTS=$(patsubst %.c,%.o,$(CFILES))
OBJECTS=$(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(CFILES)))

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(foreach X,$(LIB_DIRS),-L$(X)) -w -mconsole -o $@ $^ -lmingw32 -lSDL2 -LSDL2main 

$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(TARGET) $(BUILD_DIR)

run: all
	./$(TARGET)

.PHONY: all clean run

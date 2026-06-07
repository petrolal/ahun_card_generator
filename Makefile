CC = gcc
CFLAGS = -Wall -Wextra -I./include `pkg-config --cflags MagickWand libxml-2.0`
LDFLAGS = `pkg-config --libs MagickWand libxml-2.0`
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
TARGET = build/ahun_card_generator

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

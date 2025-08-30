BIN     := mini-db

CC      := gcc
CFLAGS  := -g -Wall -Wextra -Iinclude

SRC_DIR := src
#INC_DIR := include 

SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(SRCS:.c=.o)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN)

.PHONY: all clean

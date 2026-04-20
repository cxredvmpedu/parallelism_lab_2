CC       := clang
STD      := -std=c23
INCLUDES := -Iinclude

SRC_DIR  := src
SRCS     := $(wildcard $(SRC_DIR)/*.c)

DEBUG_DIR     := build/debug
DEBUG_TARGET  := $(DEBUG_DIR)/lab_2
DEBUG_OBJS    := $(SRCS:$(SRC_DIR)/%.c=$(DEBUG_DIR)/%.o)

DEBUG_CFLAGS  := $(STD) $(INCLUDES) -pthread -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined,leak $(EXTRA_CFLAGS)
DEBUG_LDFLAGS := -pthread -fsanitize=address,undefined,leak

RELEASE_DIR     := build/release
RELEASE_TARGET  := $(RELEASE_DIR)/lab_2
RELEASE_OBJS    := $(SRCS:$(SRC_DIR)/%.c=$(RELEASE_DIR)/%.o)

RELEASE_CFLAGS  := $(STD) $(INCLUDES) -pthread -O3 $(EXTRA_CFLAGS)
RELEASE_LDFLAGS := -pthread

.PHONY: all debug release clean

all: debug

debug: $(DEBUG_TARGET)

release: $(RELEASE_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(DEBUG_OBJS) $(DEBUG_LDFLAGS) -o $(DEBUG_TARGET)

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

$(RELEASE_TARGET): $(RELEASE_OBJS)
	@mkdir -p $(RELEASE_DIR)
	$(CC) $(RELEASE_OBJS) $(RELEASE_LDFLAGS) -o $(RELEASE_TARGET)

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(RELEASE_DIR)
	$(CC) $(RELEASE_CFLAGS) -c $< -o $@

clean:
	rm -rf build

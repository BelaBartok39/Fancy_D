CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lm -lcjson
INCLUDES = $(shell pkg-config --cflags cjson) -Isrc

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(wildcard $(SRC_DIR)/*.h)
TARGET = $(BIN_DIR)/fancyC

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Building $(TARGET)"
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

install: $(TARGET)
	@echo "Installing $(TARGET)"
	mkdir -p $(HOME)/bin
	cp $(TARGET) $(HOME)/bin/fancyC
	chmod +x $(HOME)/bin/fancyC
	@echo "Installation complete. Make sure $(HOME)/bin is in your PATH."

uninstall:
	@echo "Uninstalling fancyC"
	rm -f $(HOME)/bin/fancyC
	@echo "Uninstallation complete."

clean:
	@echo "Cleaning up"
	rm -rf $(OBJ_DIR) $(BIN_DIR)

debug:
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(OBJS)"
	@echo "Dependencies: $(DEPS)"
	@echo "cJSON flags: $(INCLUDES)"
	@echo "Libraries: $(LIBS)"
	@ls -l $(SRC_DIR)

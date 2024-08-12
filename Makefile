CC = gcc
CFLAGS = -Wall -Wextra -g -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200809L
LIBS = -lcjson
INCLUDES = $(shell pkg-config --cflags cjson 2>/dev/null || echo "") -Isrc
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(wildcard $(SRC_DIR)/*.h)
TARGET = $(BIN_DIR)/fancyD

# Test-specific variables
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)
MAIN_OBJ = $(OBJ_DIR)/main.o
TEST_TARGET = $(BIN_DIR)/run_tests
TEST_LIBS = $(LIBS) -lcjson -lcheck -lsubunit -lpthread -lrt -lm

.PHONY: all clean install uninstall test

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
	cp $(TARGET) $(HOME)/bin/fancyD
	chmod +x $(HOME)/bin/fancyD
	@echo "Installation complete. Make sure $(HOME)/bin is in your PATH."

uninstall:
	@echo "Uninstalling fancyD"
	rm -f $(HOME)/bin/fancyD
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

# Test targets
test: $(TEST_TARGET)
	@echo "Running tests"
	./$(TEST_TARGET)

$(TEST_TARGET): $(filter-out $(MAIN_OBJ), $(OBJS)) $(TEST_OBJS) | $(BIN_DIR)
	@echo "Building test executable"
	$(CC) $(CFLAGS) -o $@ $^ $(TEST_LIBS)

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling test file $<"
	$(CC) $(CFLAGS) $(INCLUDES) -I$(SRC_DIR) -c $< -o $@

CC = gcc
CFLAGS = -Wall -Wextra -g -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200809L
LIBS = -lcjson
INCLUDES = $(shell pkg-config --cflags cjson 2>/dev/null || echo "") -Isrc
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests
INSTALL_DIR = /usr/local/bin
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

# Color definitions
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

.PHONY: all clean install uninstall test requirements

all: requirements $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "$(BLUE)Building $(TARGET)$(NC)"
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "$(GREEN)Build complete: $(TARGET)$(NC)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) | $(OBJ_DIR)
	@echo "$(BLUE)Compiling $<$(NC)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

install: $(TARGET)
	@echo "$(YELLOW)Note: Installing to $(INSTALL_DIR) requires sudo privileges$(NC)"
	@echo "$(BLUE)Installing $(TARGET)$(NC)"
	@sudo cp $(TARGET) $(INSTALL_DIR)/fancyD
	@sudo chmod +x $(INSTALL_DIR)/fancyD
	@echo "$(GREEN)Installation complete. $(INSTALL_DIR) should already be in your PATH.$(NC)"

uninstall:
	@echo "$(YELLOW)Note: Uninstalling from $(INSTALL_DIR) requires sudo privileges$(NC)"
	@echo "$(BLUE)Uninstalling fancyD$(NC)"
	@sudo rm -f $(INSTALL_DIR)/fancyD
	@echo "$(GREEN)Uninstallation complete.$(NC)"

clean:
	@echo "$(YELLOW)Cleaning up$(NC)"
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "$(GREEN)Clean complete$(NC)"

debug:
	@echo "$(BLUE)Sources: $(SRCS)$(NC)"
	@echo "$(BLUE)Objects: $(OBJS)$(NC)"
	@echo "$(BLUE)Dependencies: $(DEPS)$(NC)"
	@echo "$(BLUE)cJSON flags: $(INCLUDES)$(NC)"
	@echo "$(BLUE)Libraries: $(LIBS)$(NC)"
	@ls -l $(SRC_DIR)

# Test targets
test: requirements $(TEST_TARGET)
	@echo "$(BLUE)Running tests$(NC)"
	@./$(TEST_TARGET)

$(TEST_TARGET): $(filter-out $(MAIN_OBJ), $(OBJS)) $(TEST_OBJS) | $(BIN_DIR)
	@echo "$(BLUE)Building test executable$(NC)"
	@$(CC) $(CFLAGS) -o $@ $^ $(TEST_LIBS)
	@echo "$(GREEN)Test executable built: $(TEST_TARGET)$(NC)"

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	@echo "$(BLUE)Compiling test file $<$(NC)"
	@$(CC) $(CFLAGS) $(INCLUDES) -I$(SRC_DIR) -c $< -o $@

requirements:
	@echo "$(BLUE)Checking requirements...$(NC)"
	@if ! dpkg -s libcjson-dev build-essential check > /dev/null 2>&1; then \
		echo "$(YELLOW)Some requirements are missing.$(NC)"; \
		read -p "$(YELLOW)Do you want to install them? (y/n) $(NC)" answer; \
		if [ "$$answer" = "y" ]; then \
			echo "$(BLUE)Installing requirements...$(NC)"; \
			sudo apt-get update && sudo apt-get install -y libcjson-dev build-essential check; \
			echo "$(GREEN)Requirements installed successfully!$(NC)"; \
		else \
			echo "$(RED)Requirements not installed. Exiting.$(NC)"; \
			exit 1; \
		fi; \
	else \
		echo "$(GREEN)All requirements are satisfied.$(NC)"; \
	fi

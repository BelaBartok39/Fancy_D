CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lm -lcjson
INCLUDES = $(shell pkg-config --cflags cjson)

SRCS = fancy.c
OBJS = $(SRCS:.c=.o)
TARGET = fancyC

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Building $(TARGET)"
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

install: $(TARGET)
	@echo "Installing $(TARGET)"
	mkdir -p $(HOME)/bin
	cp $(TARGET) $(HOME)/bin/$(TARGET)
	chmod +x $(HOME)/bin/$(TARGET)
	@echo "Installation complete. Make sure $(HOME)/bin is in your PATH."

uninstall:
	@echo "Uninstalling $(TARGET)"
	rm -f $(HOME)/bin/$(TARGET)
	@echo "Uninstallation complete."

clean:
	@echo "Cleaning up"
	rm -f $(OBJS) $(TARGET)

debug:
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(OBJS)"
	@echo "cJSON flags: $(INCLUDES)"
	@echo "Libraries: $(LIBS)"
	@ls -l

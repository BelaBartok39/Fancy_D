CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lm

SRCS = fancy.c cJSON.c
OBJS = $(SRCS:.c=.o)
TARGET = fancy

all: $(TARGET)

$(TARGET): $(OBJS)
    $(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
    $(CC) $(CFLAGS) -c $

clean:
    rm -f $(OBJS) $(TARGET)

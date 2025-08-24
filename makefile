CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcrypto

TARGET = linkshare

SRCS = main.c sqlite3.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

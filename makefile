CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lcrypto -g

TARGET = linkshare

SRCS = src/*.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

src/main.o: pages/*.html

src/db.o: sql/*.sql

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

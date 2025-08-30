CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lcrypto -g

TARGET = linkshare

SRCS = src/main.c src/sqlite3.c src/db.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

main.o: pages/index.html pages/view.html

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

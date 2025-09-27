CC = gcc
CFLAGS = -Wall -Wextra -O2 -g 
LDFLAGS = -lcrypto -lcurl -g

TARGET = linkshare

SRCS = src/db.c src/main.c src/pg.c src/sqlite3.c src/md.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

src/pg.o: pages/*.html
src/pg.o: pages/*.xml
src/pg.o: pages/*.css

src/db.o: sql/*.sql

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

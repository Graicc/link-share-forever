#include "pg.h"

#include "db.h"
#include <string.h>
#include <unistd.h>

#define write_str(buffer, string) write(buffer, string, strlen(string))

const char HEADER[] = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html; charset=UTF-8\r\n\r\n";

const char S_LAYOUT_PAGE[] = {
#embed "../pages/layout.html"
    , '\0'};
splitString LAYOUT_PAGE = {};

const char S_INDEX_PAGE[] = {
#embed "../pages/index.html"
};

const char *TARGET = "{{}}";

int pg_init() {
  pg_splitString(S_LAYOUT_PAGE, &LAYOUT_PAGE);

  return 0;
}

int pg_splitString(const char *str, splitString *out) {
  const char *splitPoint = strstr(str, TARGET);
  if (splitPoint == NULL) {
    return 1;
  }

  out->before = str;
  out->beforeLen = splitPoint - str;
  out->after = splitPoint + 4;
  out->afterLen = strlen(str) - out->beforeLen - 4;

  return 0;
}

int pg_index(int stream_fd) {
  write(stream_fd, HEADER, sizeof(HEADER));
  write(stream_fd, LAYOUT_PAGE.before, LAYOUT_PAGE.beforeLen);

  write(stream_fd, S_INDEX_PAGE, sizeof(S_INDEX_PAGE));

  write(stream_fd, LAYOUT_PAGE.after, LAYOUT_PAGE.afterLen);

  return 0;
}

int pg_view(int stream_fd, const char *name, sqlite3 *db) {
  write(stream_fd, HEADER, sizeof(HEADER));
  write(stream_fd, LAYOUT_PAGE.before, LAYOUT_PAGE.beforeLen);

  db_getFeed(db, name);

  db_link link;
  while (db_stepGetFeed(db, &link) == 0) {
    write_str(stream_fd, "<a href=\"");
    write_str(stream_fd, link.url);
    write_str(stream_fd, "\"> <h2>");
    write_str(stream_fd, link.title);
    write_str(stream_fd, "</h2></a> <p>");
    write_str(stream_fd, link.desc);
    write_str(stream_fd, "</p>");
  }

  write(stream_fd, LAYOUT_PAGE.after, LAYOUT_PAGE.afterLen);

  return 0;
}
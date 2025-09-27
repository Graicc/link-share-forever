#include "pg.h"

#include "db.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define write_str(buffer, string) write(buffer, string, strlen(string))

const char HTML_HEADER[] = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n\r\n";
const char RSS_HEADER[] = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/rss+xml\r\n\r\n";

const char S_LAYOUT_PAGE[] = {
#embed "../pages/layout.html"
    , '\0'};
splitString LAYOUT_PAGE = {};

const char S_RSS_OUTLINE[] = {
#embed "../pages/rss_outline.xml"
};
splitString RSS_OUTLINE = {};

const char S_INDEX_PAGE[] = {
#embed "../pages/index.html"
};

const char *TARGET = "{{}}";

int pg_init() {
  pg_splitString(S_LAYOUT_PAGE, &LAYOUT_PAGE);
  pg_splitString(S_RSS_OUTLINE, &RSS_OUTLINE);

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

int pg_pageIndex(int stream_fd) {
  write(stream_fd, HTML_HEADER, sizeof(HTML_HEADER) - 1);
  write(stream_fd, LAYOUT_PAGE.before, LAYOUT_PAGE.beforeLen);

  write(stream_fd, S_INDEX_PAGE, sizeof(S_INDEX_PAGE) - 1);

  write(stream_fd, LAYOUT_PAGE.after, LAYOUT_PAGE.afterLen);

  return 0;
}

int pg_pageView(int stream_fd, const char *name, sqlite3 *db) {
  write(stream_fd, HTML_HEADER, sizeof(HTML_HEADER) - 1);
  write(stream_fd, LAYOUT_PAGE.before, LAYOUT_PAGE.beforeLen);

  db_getFeed(db, name);

  write_str(stream_fd, "<h1>");
  write_str(stream_fd, name);
  write_str(stream_fd, "'s Links </h1>");

  db_link link;
  while (db_stepGetFeed(db, &link) == 0) {
    write_str(stream_fd, "<hr /><a href=\"");
    write_str(stream_fd, link.url);
    write_str(stream_fd, "\"> <h2>");
    write_str(stream_fd, link.title);
    write_str(stream_fd, "</h2></a>");
    if (link.desc != NULL) {
      write_str(stream_fd, "<p>");
      write_str(stream_fd, link.desc);
      write_str(stream_fd, "</p>");
    }
  }

  write(stream_fd, LAYOUT_PAGE.after, LAYOUT_PAGE.afterLen);

  return 0;
}

int pg_rssView(int stream_fd, const char *name, sqlite3 *db) {
  write(stream_fd, RSS_HEADER, sizeof(RSS_HEADER) - 1);
  write(stream_fd, RSS_OUTLINE.before, RSS_OUTLINE.beforeLen);
  printf("%s", RSS_OUTLINE.before);

  db_getFeed(db, name);

  write_str(stream_fd, "<title>");
  write_str(stream_fd, name);
  write_str(stream_fd,
            "'s Links </title> <link>https://share-links.graic.net/view/");
  write_str(stream_fd, name);
  write_str(stream_fd, "</link> <description>undefined</description>");

  db_link link;
  while (db_stepGetFeed(db, &link) == 0) {
    write_str(stream_fd, "<item><title><![CDATA[");
    write_str(stream_fd, link.title);
    write_str(stream_fd, "]]></title> <guid>");
    write_str(stream_fd, link.url);
    write_str(stream_fd, "</guid> <pubDate>");

    {
      struct tm *tm_info;

      tm_info = gmtime(&link.date);
      if (tm_info == NULL) {
        fprintf(stderr, "Error parsing Unix timestamp\n");
        return 1;
      }

      char rfc822_timestamp[30];
      strftime(rfc822_timestamp, sizeof(rfc822_timestamp),
               "%a, %d %b %Y %H:%M:%S GMT", tm_info);

      write_str(stream_fd, rfc822_timestamp);
    }
    write_str(stream_fd, "</pubDate>");

    if (link.desc != NULL) {
      write_str(stream_fd, "<description><![CDATA[");
      write_str(stream_fd, link.desc);
      write_str(stream_fd, "]]></description>");
    }

    write_str(stream_fd, "</item>");
  }

  write(stream_fd, RSS_OUTLINE.after, RSS_OUTLINE.afterLen);

  return 0;
}

int pg_parseForm(const char *response, const char *key1, const char **outVal1,
                 const char *key2, const char **outVal2, const char *key3,
                 const char **outVal3) {
  const char *doubleLineBreak = "\r\n\r\n";

  const char *doubleLineBreakInBuffer = strstr(response, doubleLineBreak);

  if (doubleLineBreakInBuffer == NULL) {
    fprintf(stderr, "Post request with no body\n");
    return 1;
  }

  const char *body = doubleLineBreakInBuffer + strlen(doubleLineBreak);
  return pg_parseFormBody(body, key1, outVal1, key2, outVal2, key3, outVal3);
}

int pg_parseFormBody(const char *body, const char *key1, const char **outVal1,
                     const char *key2, const char **outVal2, const char *key3,
                     const char **outVal3) {
  // This could be a varadic function, but that would be complexity that isn't
  // neccessary, since we know we won't ever need more than 3 args

  *outVal1 = NULL;
  *outVal2 = NULL;
  *outVal3 = NULL;

  const char *head = body;
  while (1) {
    const char *equals = strchr(head, '=');
    char *ampersand = strchrnul(head, '&');
    if (equals == NULL) {
      break;
    }

    const char *key = head;
    size_t keyLen = equals - head;

    const char *value = equals + 1;
    // size_t valueLen = ampersand - value;

    if (key1 != NULL && strncmp(key, key1, keyLen) == 0) {
      *outVal1 = value;
    } else if (key2 != NULL && strncmp(key, key2, keyLen) == 0) {
      *outVal2 = value;
    } else if (key3 != NULL && strncmp(key, key3, keyLen) == 0) {
      *outVal3 = value;
    }

    if (ampersand[0] == '\0') {
      // We have reached the end of the string
      break;
    }

    ampersand[0] = '\0'; // Make the value null terminated

    head = ampersand + 1;
  }

  // Make sure all asked for keys are present
  if ((key1 != NULL && *outVal1 == NULL) ||
      (key2 != NULL && *outVal2 == NULL) ||
      (key3 != NULL && *outVal3 == NULL)) {
    return 1;
  }

  return 0;
}
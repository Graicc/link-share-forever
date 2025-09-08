#include "sqlite3.h"
#include <stddef.h>

typedef struct splitString {
  const char *before;
  size_t beforeLen;
  const char *after;
  size_t afterLen;
} splitString;

int pg_init();

int pg_splitString(const char *str, splitString *out);

int pg_pageIndex(int stream_fd);

int pg_pageView(int stream_fd, const char *name, sqlite3 *db);

int pg_parseForm(const char *response, const char *key1, const char **outVal1,
                 const char *key2, const char **outVal2, const char *key3,
                 const char **outVal3);

int pg_parseFormBody(const char *body, const char *key1, const char **outVal1,
                     const char *key2, const char **outVal2, const char *key3,
                     const char **outVal3);
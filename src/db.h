#include "sqlite3.h"

typedef struct link {
  const char *title;
  const char *url;
  const char *desc;
  const char *image;
} link;

int db_setup(sqlite3 *db);

int db_addFeed(sqlite3 *db, char *name, char *plaintextPassword);

int db_addLink(sqlite3 *db, char *name, char *plaintextPassword,
               const link *info);
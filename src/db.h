#include "sqlite3.h"

typedef struct db_link {
  const char *title;
  const char *url;
  const char *desc;
  const char *image;
} db_link;

int db_init(sqlite3 *db);

int db_addFeed(sqlite3 *db, char *name, char *plaintextPassword);

int db_addLink(sqlite3 *db, char *name, char *plaintextPassword,
               const db_link *info);

int db_getFeed(sqlite3 *db, const char *name);
int db_stepGetFeed(sqlite3 *db, db_link *link);
#include "sqlite3.h"

int db_setup(sqlite3 *db);

int db_addFeed(sqlite3 *db, char *name, char *plaintextPassword);
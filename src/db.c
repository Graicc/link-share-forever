#include "db.h"

#include "openssl/sha.h"
#include <stdio.h>
#include <string.h>

const char S_INIT[] = {
#embed "../sql/init.sql"
};

const char S_ADD_FEED[] = {
#embed "../sql/add_feed.sql"
};

const char S_GET_FEED[] = {
#embed "../sql/get_feed.sql"
};

const char S_ADD_LINK[] = {
#embed "../sql/add_link.sql"
};

sqlite3_stmt *addFeedStmt = NULL;
sqlite3_stmt *getFeedStmt = NULL;

sqlite3_stmt *addLinkStmt = NULL;

#pragma GCC diagnostic ignored "-Wunused-parameter"
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

int db_init(sqlite3 *db) {
  char *zErrMsg = 0;
  int res = sqlite3_exec(db, S_INIT, callback, 0, &zErrMsg);
  if (res != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sqlite3_prepare_v2(db, S_ADD_FEED, sizeof(S_ADD_FEED), &addFeedStmt, NULL);
  sqlite3_prepare_v2(db, S_GET_FEED, sizeof(S_GET_FEED), &getFeedStmt, NULL);
  sqlite3_prepare_v2(db, S_ADD_LINK, sizeof(S_ADD_LINK), &addLinkStmt, NULL);

  // TODO: remove temporary data

  db_addFeed(db, "Graic", "hunter2");
  db_addFeed(db, "Graic2", "hunter2");

  // db_link post = {
  //     .title = "Graic Blog",
  //     .url = "https://graic.net",
  //     .desc = "Description for graic.net",
  //     .image = NULL,
  // };
  // db_addLink(db, "Graic", "hunter2", &post);
  // db_link post2 = {
  //     .title = "Callan",
  //     .url = "https://callan101.com",
  //     .desc = "Description",
  //     .image = NULL,
  // };
  // db_addLink(db, "Graic", "hunter2", &post2);
  // db_addLink(db, "Graic2", "hunter2", &post);

  return res;
}

void db_hash(const char *plaintextPassword,
             unsigned char hash[SHA256_DIGEST_LENGTH + 1]) {
  // yeah yeah this should be random for each feed so that you can't precompute
  // a lookup table
  const char *SALT = "linksharesalt";

  char saltedPassword[256];
  snprintf(saltedPassword, sizeof(saltedPassword), "%s%s", plaintextPassword,
           SALT);

  SHA256((unsigned char *)saltedPassword, strlen(saltedPassword), hash);
  hash[SHA256_DIGEST_LENGTH] = 0;
}

int db_addFeed(sqlite3 *db, const char *name, const char *plaintextPassword) {
  unsigned char hash[SHA256_DIGEST_LENGTH + 1];
  db_hash(plaintextPassword, hash);

  sqlite3_reset(addFeedStmt);
  // SQLITE_STATIC = We ensure the pointer will live until statement is
  // finalized SQLITE_TRANSIENT = SQLite should make a copy of this data
  sqlite3_bind_text(addFeedStmt, 1, name, strlen(name), SQLITE_TRANSIENT);
  sqlite3_bind_text(addFeedStmt, 2, (const char *)hash, SHA256_DIGEST_LENGTH,
                    SQLITE_TRANSIENT);

  int res = sqlite3_step(addFeedStmt);
  if (res != SQLITE_DONE) {
    fprintf(stderr, "Can't step: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  return 0;
}

int db_addLink(sqlite3 *db, const char *name, const char *plaintextPassword,
               const db_link *info) {
  if (name == NULL || plaintextPassword == NULL || info->title == NULL ||
      info->url == NULL) {
    fprintf(stderr, "Can't add null link");
    return 1;
  }

  unsigned char hash[SHA256_DIGEST_LENGTH + 1];
  db_hash(plaintextPassword, hash);

  sqlite3_reset(addLinkStmt);

  sqlite3_bind_text(addLinkStmt, 1, info->title, strlen(info->title),
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(addLinkStmt, 2, info->url, strlen(info->url),
                    SQLITE_TRANSIENT);
  if (info->desc == NULL) {
    sqlite3_bind_null(addLinkStmt, 3);
  } else {
    sqlite3_bind_text(addLinkStmt, 3, info->desc, strlen(info->desc),
                      SQLITE_TRANSIENT);
  }
  if (info->image == NULL) {
    sqlite3_bind_null(addLinkStmt, 4);
  } else {
    sqlite3_bind_text(addLinkStmt, 4, info->image, strlen(info->image),
                      SQLITE_TRANSIENT);
  }

  sqlite3_bind_text(addLinkStmt, 5, name, strlen(name), SQLITE_TRANSIENT);
  sqlite3_bind_text(addLinkStmt, 6, (const char *)hash, SHA256_DIGEST_LENGTH,
                    SQLITE_TRANSIENT);

  int res = sqlite3_step(addLinkStmt);
  if (res != SQLITE_DONE) {
    fprintf(stderr, "Can't step: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  return 0;
}

int db_getFeed(sqlite3 *db, const char *name) {
  sqlite3_reset(getFeedStmt);

  sqlite3_bind_text(getFeedStmt, 1, name, strlen(name), SQLITE_TRANSIENT);

  return 0;
}

int db_stepGetFeed(sqlite3 *db, db_link *link) {
  int res = sqlite3_step(getFeedStmt);
  if (res == SQLITE_DONE) {
    return 1;
  } else if (res == SQLITE_ROW) {
    link->title = (const char *)sqlite3_column_text(getFeedStmt, 0);
    link->url = (const char *)sqlite3_column_text(getFeedStmt, 1);
    link->desc = (const char *)sqlite3_column_text(getFeedStmt, 2);
    link->date = sqlite3_column_int64(getFeedStmt, 3);
    link->image = (const char *)sqlite3_column_text(getFeedStmt, 4);
  } else {
    fprintf(stderr, "Can't step: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  return 0;
}
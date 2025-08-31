#include "db.h"

#include <stdio.h>
#include <string.h>
#include "openssl/sha.h"

const char S_INIT[] = {
    #embed "../sql/init.sql"
};

const char S_ADD_FEED[] = {
    #embed "../sql/add_feed.sql"
};

const char S_ADD_LINK[] = {
    #embed "../sql/add_link.sql"
};

sqlite3_stmt *addFeedStmt = NULL;
sqlite3_stmt *addLinkStmt = NULL;

#pragma GCC diagnostic ignored "-Wunused-parameter"
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int db_setup(sqlite3 *db)
{
    char *zErrMsg = 0;
    int res = sqlite3_exec(db, S_INIT, callback, 0, &zErrMsg);
    if (res != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    sqlite3_prepare_v2(db, S_ADD_FEED, sizeof(S_ADD_FEED), &addFeedStmt, NULL);
    sqlite3_prepare_v2(db, S_ADD_LINK, sizeof(S_ADD_LINK), &addLinkStmt, NULL);

    db_addFeed(db, "graic", "hunter2");
    db_addFeed(db, "graic2", "hunter2");

    link post = {
        .title = "TITLE",
        .url = "URL",
        .desc = "DESC",
        .image = NULL,
    };
    db_addLink(db, "graic", "hunter2", &post);
    db_addLink(db, "graic2", "hunter2", &post);

    return res;
}

void db_hash(const char *plaintextPassword, unsigned char hash[SHA256_DIGEST_LENGTH + 1]) {
    // yeah yeah this should be random for each feed so that you can't precompute a lookup table
    const char *SALT = "linksharesalt";

    char saltedPassword[256];
    snprintf(saltedPassword, sizeof(saltedPassword), "%s%s", plaintextPassword, SALT);

    SHA256((unsigned char *)saltedPassword, strlen(saltedPassword), hash);
    hash[SHA256_DIGEST_LENGTH] = 0;
}

int db_addFeed(sqlite3 *db, char *name, char *plaintextPassword)
{
    unsigned char hash[SHA256_DIGEST_LENGTH + 1];
    db_hash(plaintextPassword, hash);

    sqlite3_reset(addFeedStmt);
    // SQLITE_STATIC = We ensure the pointer will live until statement is finalized
    // SQLITE_TRANSIENT = SQLite should make a copy of this data
    sqlite3_bind_text(addFeedStmt, 1, name, strlen(name), SQLITE_TRANSIENT);
    sqlite3_bind_text(addFeedStmt, 2, (const char *)hash, SHA256_DIGEST_LENGTH, SQLITE_TRANSIENT);

    int res = sqlite3_step(addFeedStmt);
    if (res != SQLITE_DONE) {
        fprintf(stderr, "Can't step: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    return 0;
}

int db_addLink(sqlite3 *db, char *name, char *plaintextPassword, const link *info) {
    unsigned char hash[SHA256_DIGEST_LENGTH + 1];
    db_hash(plaintextPassword, hash);

    sqlite3_reset(addLinkStmt);

    sqlite3_bind_text(addLinkStmt, 1, info->title, strlen(info->title), SQLITE_TRANSIENT);
    sqlite3_bind_text(addLinkStmt, 2, info->url, strlen(info->url), SQLITE_TRANSIENT);
    sqlite3_bind_text(addLinkStmt, 3, info->desc, strlen(info->desc), SQLITE_TRANSIENT);
    if (info->image == NULL) {
        sqlite3_bind_null(addLinkStmt, 4);
    } else {
        sqlite3_bind_text(addLinkStmt, 4, info->image, strlen(info->image), SQLITE_TRANSIENT);
    }

    sqlite3_bind_text(addLinkStmt, 5, name, strlen(name), SQLITE_TRANSIENT);
    sqlite3_bind_text(addLinkStmt, 6, (const char *)hash, SHA256_DIGEST_LENGTH, SQLITE_TRANSIENT);

    int res = sqlite3_step(addLinkStmt);
    if (res != SQLITE_DONE) {
        fprintf(stderr, "Can't step: %s\n", sqlite3_errmsg(db));
        printf("\n!!%d", res);
        return 1;
    }

    return 0;
}
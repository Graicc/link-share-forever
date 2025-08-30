#include "db.h"

#include <stdio.h>
#include <string.h>
#include "openssl/sha.h"

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
    char *createFeedTable = "CREATE TABLE IF NOT EXISTS users (name TEXT UNIQUE PRIMARY KEY, password CHAR(32), lastUpdate DATETIME);";

    char *zErrMsg = 0;
    int res = sqlite3_exec(db, createFeedTable, callback, 0, &zErrMsg);
    if (res != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return res;
}

int db_addFeed(sqlite3 *db, char *name, char *plaintextPassword)
{
    const char *SALT = "linksharesalt";

    char saltedPassword[256];
    snprintf(saltedPassword, sizeof(saltedPassword), "%s%s", plaintextPassword, SALT);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)saltedPassword, strlen(saltedPassword), hash);

    // TODO: Implement

    return 0;
}
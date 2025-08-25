#include "sqlite3.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
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

int db_addUser(sqlite3 *db, char *username, char *plaintextPassword)
{
    const char *SALT = "linksharesalt";

    char saltedPassword[256];
    snprintf(saltedPassword, sizeof(saltedPassword), "%s%s", plaintextPassword, SALT);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)saltedPassword, strlen(saltedPassword), hash);

    // TODO: Implement

    return 0;
}

int main()
{
    sqlite3 *db;
    if (sqlite3_open("database.db", &db))
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    db_setup(db);

    // db_addUser(db, "graic", "password");
    // db_addUser(db, "graic2", "password2");

    sqlite3_close(db);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Failed to initialize socket\n");
        exit(1);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
    {
        perror("Failed to set sock opt\n");
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(3000);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Failed to bind socket\n");
        exit(1);
    }

    int backlog_size = 10;
    if (listen(server_fd, backlog_size) < 0)
    {
        perror("Failed to listen socket\n");
        exit(1);
    }

    while (true)
    {
        printf("Waiting for connection\n");
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0)
        {
            perror("Failed to accept connection\n");
            continue;
        }

        const int BUFFER_SIZE = 16000;
        char buffer[BUFFER_SIZE];

        ssize_t bytes_read = read(client_fd, &buffer, sizeof(buffer) - 1);
        if (bytes_read < 0)
        {
            perror("Failed to read connection\n");
            close(client_fd);
            continue;
        }

        buffer[bytes_read] = 0; // Null terminate the string

        printf(buffer);

        const char *index = "GET / HTTP/1.1";
        if (strncmp(index, buffer, sizeof(index) - 1) == 0)
        {
            printf("strcmp\n");
            const char *header = "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                ;
            write(client_fd, header, strlen(header));
            const char message[] = {
                #embed "index.html"
            };

            write(client_fd, message, strlen(message));

        }

        close(client_fd);
    }
}
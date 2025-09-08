#include "sqlite3.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "db.h"
#include "md.h"
#include "pg.h"

int main() {
  md_init();

  sqlite3 *db;
  if (sqlite3_open("database.db", &db)) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }

  db_init(db);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Failed to initialize socket\n");
    exit(1);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1},
                 sizeof(int)) < 0) {
    perror("Failed to set sock opt\n");
    exit(1);
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(3000);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Failed to bind socket\n");
    exit(1);
  }

  int backlog_size = 10;
  if (listen(server_fd, backlog_size) < 0) {
    perror("Failed to listen socket\n");
    exit(1);
  }

  while (1) {
    printf("Waiting for connection\n");
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
      perror("Failed to accept connection\n");
      continue;
    }

    const int BUFFER_SIZE = 16000;
    char buffer[BUFFER_SIZE];

    ssize_t bytes_read = read(client_fd, &buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
      perror("Failed to read connection\n");
      close(client_fd);
      continue;
    }

    buffer[bytes_read] = 0; // Null terminate the string

    // printf(buffer);

    const char *index = "GET / HTTP/1.1\r\n";

    const char *view = "GET /view/";
    const char *suffix = " HTTP/1.1\r\n";

    const char *indexPost = "POST / HTTP/1.1\r\n";

    if (strncmp(buffer, index, strlen(index)) == 0) {
      pg_pageIndex(client_fd);
    } else if (strncmp(buffer, indexPost, strlen(indexPost)) == 0) {
      // POST /
      // Used for submitting a new link

      const char *feed_name = NULL;
      const char *password = NULL;
      const char *feed_url = NULL;

      if (pg_parseForm(buffer, "feed_name", &feed_name, "password", &password,
                       "feed_url", &feed_url) != 0) {
        fprintf(stderr, "Not all arguments provided: %d %d %d\n",
                feed_name == NULL, password == NULL, feed_url == NULL);
        continue;
      }

      printf("\n\nArguments: %s, %s, %s\n\n", feed_name, password, feed_url);

      char *url = md_decodeURL(feed_url);
      db_link info = {.url = url};
      if (md_getMetadata(&info) != 0) {
        fprintf(stderr, "Failure to get metadata");
        continue;
      }

      if (db_addLink(db, feed_name, password, &info) != 0) {
        fprintf(stderr, "Failure to add link");
        continue;
      }

      free(info.title);
      free(info.url);
      free(info.desc);
      free(info.image);

      pg_pageIndex(client_fd);
    } else if (strncmp(buffer, view, strlen(view)) == 0) {
      // /view/*
      const char *space = strchr(buffer + sizeof(view) - 1, ' ');
      if (space == NULL) {
        fprintf(stderr, "Space not found\n");
        continue;
      }

      // Validate that the request ends with ' HTTP/1.1'
      if (strncmp(space, suffix, strlen(suffix)) != 0) {
        fprintf(stderr, "Wrong suffix\n");
        continue;
      }

      char *subpage = buffer + strlen(view);
      size_t subpageLen = (size_t)(space - subpage);
      subpage[subpageLen] = '\0';

      pg_pageView(client_fd, subpage, db);
    }

    close(client_fd);
  }

  sqlite3_close(db);

  return 0;
}
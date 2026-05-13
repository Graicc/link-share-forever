#include "md.h"

#include <ctype.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CURL *curl;

typedef struct md_curlResponse {
  char *data;
  size_t size;
} md_curlResponse;

size_t md_write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
  md_curlResponse *response = userdata;

  size_t dataSize = size * nmemb;

  response->data = realloc(response->data, response->size + dataSize + 1);
  if (response->data == NULL) {
    fprintf(stderr, "Ran out of memory\n");
    return 0;
  }

  memcpy(response->data + response->size, ptr, dataSize);

  response->size += dataSize;
  response->data[response->size] = '\0';

  return dataSize;
}

// Caller must free this pointer. Returns null if not found or not valid
char *md_getInside(const char *data, const char *left, const char *right) {
  const char *leftPos = strstr(data, left);
  if (leftPos == NULL) {
    return NULL;
  }

  const char *start = leftPos + strlen(left);

  const char *rightPos = strstr(start, right);
  if (rightPos == NULL) {
    return NULL;
  }

  const char *end = rightPos - sizeof(char);

  // Trim whitespace
  while (isspace(*start)) {
    start += sizeof(char);
  }
  while (isspace(*end)) {
    end -= sizeof(char);
  }

  if (start >= end) {
    return NULL;
  }

  size_t len = end - start + 1;

  char *out = malloc(len + 1);
  memcpy(out, start, len);
  out[len] = '\0';
  return out;
}

int md_getMetadataFromResponse(const md_curlResponse *response,
                               db_link *outMetadata) {
  outMetadata->title = NULL;
  outMetadata->desc = NULL;
  outMetadata->image = NULL;

  outMetadata->title = md_getInside(
      response->data, "<meta property=\"og:title\" content=\"", "\"");
  if (outMetadata->title == NULL) {
    // Substack is weird
    outMetadata->title = md_getInside(
        response->data, "<meta data-rh=\"true\" property=\"og:title\" content=\"", "\"");
  }
  if (outMetadata->title == NULL) {
    outMetadata->title = md_getInside(
        response->data, "<meta name=\"twitter:title\" content=\"", "\"");
  }
  if (outMetadata->title == NULL) {
    outMetadata->title = md_getInside(response->data, "<title>", "</title>");
  }

  if (outMetadata->title == NULL) {
    fprintf(stderr, "No title found\n");
    return 1;
  }

  outMetadata->desc = md_getInside(
      response->data, "<meta property=\"og:description\" content=\"", "\"");
  if (outMetadata->desc == NULL) {
    // Substack is weird
    outMetadata->desc = md_getInside(
        response->data, "<meta data-rh=\"true\" property=\"og:description\" content=\"", "\"");
  }
  if (outMetadata->desc == NULL) {
    outMetadata->desc = md_getInside(
        response->data, "<meta name=\"twitter:description\" content=\"", "\"");
  }
  if (outMetadata->desc == NULL) {
    outMetadata->desc = md_getInside(
        response->data, "<meta name=\"description\" content=\"", "\"");
  }

  outMetadata->image = md_getInside(
      response->data, "<meta property=\"og:image\" content=\"", "\"");
  if (outMetadata->image == NULL) {
    // Substack is weird
    outMetadata->image = md_getInside(
        response->data, "<meta data-rh=\"true\" property=\"og:image\" content=\"", "\"");
  }
  if (outMetadata->image == NULL) {
    outMetadata->image = md_getInside(
        response->data, "<meta name=\"twitter:image\" content=\"", "\"");
  }

  printf("TITLE: [%s]\n", outMetadata->title);
  if (outMetadata->desc != NULL) {
    printf("DESCRIPTION: [%s]\n", outMetadata->desc);
  }
  if (outMetadata->image != NULL) {
    printf("IMAGE: [%s]\n", outMetadata->image);
  }
  return 0;
}

int md_init() {
  curl = curl_easy_init();
  return 0;
}

char *md_decodeURL(const char *url) {
  size_t len = strlen(url);

  size_t new_len = len + 1;

  // Handle urls not prefixed with http(s)://
  bool needsHTTP = false;
  const char *httpPrefix = "http://";
  if (strncmp("http", url, strlen("http")) != 0) {
    new_len += strlen(httpPrefix);
    needsHTTP = true;
  }

  char *out = malloc(new_len);
  if (!out) {
    fprintf(stderr, "Ran out of memory\n");
    return NULL;
  }

  char *curr = out;
  if (needsHTTP) {
    strcpy(out, httpPrefix);
    curr += strlen(httpPrefix);
  }

  for (size_t i = 0; i < len; i++) {
    char u = url[i];
    if (u == '%' && i < len - 2) {
      if (isxdigit(url[i + 1]) && isxdigit(url[i + 2])) {
        int value;
        sscanf(&url[i + 1], "%2x", &value);
        *curr = (char)value;
        curr++;
        i += 2;
      }
    } else if (u == '+') {
      *curr = ' ';
      curr++;
    } else {
      *curr = u;
      curr++;
    }
  }
  *curr = '\0';
  return out;
}

int md_getMetadata(db_link *outMetadata) {
  if (!curl) {
    fprintf(stderr, "Curl does not exist\n");
    return 1;
  }

  md_curlResponse response = {0};

  curl_easy_setopt(curl, CURLOPT_URL, outMetadata->url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, md_write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    free(response.data);
    return 1;
  }

  // We only want to run this at the end of the program
  // curl_easy_cleanup(curl);

  printf("Data for URL: %s\n", outMetadata->url);
  int parseRes = md_getMetadataFromResponse(&response, outMetadata);

  free(response.data);
  return parseRes;
}

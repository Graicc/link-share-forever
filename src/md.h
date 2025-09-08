#include "db.h"

int md_init();

// Caller must free this value
char *md_decodeURL(const char *url);

// Populates metadata. URL must already be set. Caller must free pointers in
// outmetadata
int md_getMetadata(db_link *outMetadata);
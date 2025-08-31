# Logs

## 2025-08-30
- Got main db mutation functions working: creating feeds and inserting into feeds
- Finally refactored project to sort things into directories (src, sql, pages)
- Updated the makefile to refresh on .html and .sql changes
  - Still "need" to make it recompile on header changes

## 2025-08-28
- Got forms working with parsing the resulting arguments
- sizeof(char *) is 8 not the length of the string (obviously)
- I miss wide pointers

## 2025-08-24
- Got minimal HTTP server working
- Returns index.html using #embed to statically link it
- Seems that index.html doesn't have to be CRLF encoded, at least in firefox
- Make sure to keep makefile up to date with html dependencies

## 2025-08-23
- Learned enough sql to get by (see sql.md)
- Need to setup prepared statements with these queries
- #embed exists now!!

## 2025-08-19
- Got sqlite setup
- Got make file setup to build (ilovecilovec)
- I need to learn more sql to be effective

# Logs

## 2025-09-07
Got end to end flow working! You can now add links!
- Moved form parsing to pg.c
- URL decode for form param
- Get metadata for url. Parses title, description, image
- Form on index.html adds it to the database
Still need to add the ability to create accounts, rss view (needs dates), and rate limiting. But thats all that's left for v0.1! *should* be able to get v0.1 out this week.

## 2025-09-01
- Database get feed working (mostly, no dates yet)
- Added barebones /view/[name] page to show data
  - techincally RSS feed is higher priority but this is easier to test
- Starting porting the pages to pg.c
  - Ported index over using pg_splitString to have a layout page
- Largest unkown now is getting all of the info from just the link

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

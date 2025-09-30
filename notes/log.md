# Logs

## 2025-09-29
Shipped!

Added the ability to make a new feed, and put it on an oracle free tier. Need to set it up to properly setup after a restart (right now it's in a tmux session). Ended up installing nix on it and building from source, it was the easiest. In the past I've tried to copy build artifacts, but that's always more hassle than its worth. Updated the DNS records so it should point there in the morning. Final thing before I can call it done is to fix the metadata of youtube urls, since youtube returns generic metadata if you query it from a server.

Glad to have it up, now I *just* need to write the blog post.

## 2025-09-28
Lots of usability improvemnts today.

Switched over to bcrypt. It was easier than I expected. Tried vibe coding it. The last (and first) time I tried vibe coding on this project the first thing it did was add a SQL injection.
Unsuprisingly, it did not do well. It did correctly point me at `crypt` being the function I needed, but the hash specifier for bcrypt was malformed, the salt was `linksharesaltstring22chars!` (notably 27 characters), the output buffer was 64 chars instead of 60, it didn't use constants anywhere, the dependence it added to shell.nix didn't exist, and also it always returned NULL so auth would always succeed. Actually writing the code like its 2019 worked (duh).

The man pages for `crypt` say that "salt is a two-character string". This is a lie. It is actually a string in the Modular Crypt Format! RTFM (Recognize The Faulty Manual)!

Added error reporting to the link add form. Did some sql crimes in `can_post.sql` to get the type of error directly. Once again tried to have an LLM generate it, once again had to rewrite it from scratch because it didn't understand that the query would be empty if there was no user with the id (yes I reproompted it with this information)

I feel like there should've been a more native way than returning a new page with an error message. This is probably a usecase where HTMX would be usefull, but HTMX won't be a thing people use when I die. Also, it'd require adding JS to the project. no more scope, we have enough

## 2025-09-27
Almost past the finish line. Added rate limiting on making new posts, now you can only do it every 6 hours.
Still need to add the ability to make a new feed, but after that its just getting it deployed.
I think I'll probably use oracle free tier compute + nginx + systemd + cloudflare (for https) since I've already used it all. Maybe a ./config folder with config files and shell scripts to copy them. Should be able to get away with scp-ing the binary over, its only 5Mb.

## 2025-09-26
Oopsie didn't work on this for a while. Did a bunch of style work for the website, it maches the look of the old one now. Major blockers rn are adding the 6hr timeout and a new post page. Also should probably add some 404 pages when the db returns nothing. Tried setting up entr for hot reloading to no avail.

## 2025-09-11
RSS! Forogt how much of RSS spec is weird (need to have xmlns:cd="http://purl.org/dc/elements/1/1" or it doesn't work)
but it passes [https://validator.w3.org/feed/check.cgi](W3 Feed Validator) so good enough for me.
/feed/[name] responds with rss feed like it does on the live version.

I dislike that RSS uses [RFC822](https://www.ietf.org/rfc/rfc822.txt) dates. ISO8601 🔛🔝

Other changes:
- Wasn't actually initializing outlines, oopsie!
- Date support

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

INSERT OR IGNORE INTO feeds (name, password, last_updated)
VALUES (@name, @password, julianday('now'));
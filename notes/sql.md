DROP TABLE IF EXISTS feeds;

CREATE TABLE IF NOT EXISTS feeds (
  id INTEGER PRIMARY KEY,
  name varchar(255) UNIQUE NOT NULL,
  password varchar(32) NOT NULL,
  last_updated real NOT NULL
);

INSERT INTO feeds (name, password, last_updated)
VALUES ('graic', 'password', julianday('now'));

INSERT INTO feeds (name, password, last_updated)
VALUES ('other', 'password', julianday('2020-01-01T10:30:00'));

--SELECT * from feeds;

--SELECT name, date(last_updated), time(last_updated) from feeds

DROP TABLE IF EXISTS links;

CREATE TABLE IF NOT EXISTS links (
  id INTEGER PRIMARY KEY,
  title text NOT NULL,
  url text NOT NULL,
  description text NOT NULL,
  date real NOT NULL,
  image text,

  owner_id INTEGER NOT NULL,
  FOREIGN KEY (owner_id)
    REFERENCES feeds (id)
);

-- Index to make it zoomy
CREATE INDEX IF NOT EXISTS idx_links_owner_date
  ON links(owner_id, date DESC);

INSERT INTO links (title, url, description, date, image, owner_id)
SELECT 'Title', 'URL', 'Desc', julianday('2020-01-01T10:30:00'), NULL, feeds.id
FROM feeds
WHERE feeds.name == 'graic' AND feeds.password == 'password';

INSERT INTO links(title, url, description, date, image, owner_id)
SELECT 'Title2', 'URL2', 'Desc2', julianday('now'), NULL, f.id
FROM feeds f
WHERE f.name == 'other' AND f.password == 'password';

-- SELECT * from links;

SELECT title, description, date
FROM links
WHERE owner_id == (
  SELECT id FROM feeds where name == 'other'
)
ORDER BY date DESC
LIMIT 100


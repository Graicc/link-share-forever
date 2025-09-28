-- DROP TABLE IF EXISTS feeds;

CREATE TABLE IF NOT EXISTS feeds (
  id INTEGER PRIMARY KEY,
  name varchar(255) UNIQUE NOT NULL,
  password varchar(32) NOT NULL
);

-- DROP TABLE IF EXISTS links;

CREATE TABLE IF NOT EXISTS links (
  id INTEGER PRIMARY KEY,
  title text NOT NULL,
  url text NOT NULL,
  description text,
  date real NOT NULL,
  image text,

  owner_id INTEGER NOT NULL,
  FOREIGN KEY (owner_id)
    REFERENCES feeds (id)
);

-- Index to make it zoomy
CREATE INDEX IF NOT EXISTS idx_links_owner_date
  ON links(owner_id, date DESC);


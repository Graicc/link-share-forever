SELECT title, url, description, unixepoch(date), image
FROM links
WHERE owner_id == (
  SELECT id FROM feeds where name == @name
)
ORDER BY date DESC
LIMIT 100

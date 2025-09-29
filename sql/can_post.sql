SELECT COALESCE (
  (
    SELECT CASE
      WHEN feed_password != @password THEN 2 -- Wrong password
      WHEN EXISTS (
        SELECT 1 FROM links
        WHERE owner_id = feed_id AND julianday('now') - date <= 0.25
      ) THEN 3 -- Posted too recent
      ELSE 0 -- Good to go
    END
    FROM (
      SELECT id AS feed_id, password AS feed_password
      FROM feeds
      WHERE name = @name
      LIMIT 1
    )
  ),
  1 -- No feed
);
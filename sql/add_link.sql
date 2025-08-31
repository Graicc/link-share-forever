INSERT INTO links (title, url, description, date, image, owner_id)
SELECT @title, @url, @desc, julianday('now'), @image, feeds.id
-- SELECT @title, @url, @desc, julianday('now'), @image, 1
FROM feeds
WHERE feeds.name == @name AND feeds.password == @password;
-- FROM feeds
-- WHERE feeds.name == "graic"

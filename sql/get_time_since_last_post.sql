SELECT COALESCE(
	(
		SELECT julianday('now') - date
		FROM links
		WHERE owner_id = (
			SELECT id FROM feeds WHERE name = @name
		)
		ORDER BY date DESC
		LIMIT 1
	),
	999	
)

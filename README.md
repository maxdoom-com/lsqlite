Lsqlite is a simple sqlite module i wrote back in 2010. It is still fully functional with lua 5.3 as of 2016.

Lsqlite gives you 3 functions only:
```lua
	local db = lsqlite.open(database)
	results, err = db:exec(statments)
	db:close()
```


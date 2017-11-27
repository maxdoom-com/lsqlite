LSQlite
=======

Lsqlite is a simple sqlite module i wrote back in 2010. It is still fully functional with lua 5.3 as of end 2017.

Lsqlite gives you 3 functions only:

```lua
    local db = lsqlite.open(database)
    results, err = db:exec(statments)
    db:close()
```


SYNOPSIS
--------

```lua
    require("lsqlite")

    local db = lsqlite.open(database)
    db:exec(statements)
    db:close(db)
```


SUMMARY
-------

The lsqlite module is a minimalistic sqlite3 - lua module.  You can open a database, perform any sqlite statements on it and close it.  If you don't close the database, the garbage collector will do this for you.


DESCRIPTION
-----------

- lsqlite.open(database) returns an lsqlite object or nil
- db:exec(statements) executes one or more sqlite statments in one string and returns the output and an error description, if any
- db:close() closes an open database


EXAMPLE
-------

```lua
    local db = lsqlite.open("database.sqlite")
    local results, error = db:exec("select * from my_table;")
    for i=1,#results do
        for k,v in pairs(results[i]) do
            print(k,v)
        end
    end
    db:close(db)
```


SPECIAL FILES
-------------

To open an in memory database try

```lua
    local db = lsqlite.open(":memory:")
```


COPYRIGHT
---------

Copyright © 2010-2017 by Nico Hoffmann. License is MIT. For the complete text see the supplied documentation.

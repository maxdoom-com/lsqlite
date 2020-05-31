LSQlite
=======

Lsqlite is a simple sqlite module i wrote back in 2010. It is still fully
functional with lua 5.3 as of end 2017.

Update: It's 2020 and I had to change two lines.


SYNOPSIS
--------

```lua
lsqlite = require("lsqlite")

local db = lsqlite.open(database)
db:exec(statements)
db:close(db)
```


SUMMARY
-------

The lsqlite module is a minimalistic sqlite3 - lua module. 
You can open a database, perform any sqlite statements on it and close it.
If you don't close the database, the garbage collector will do this for you.


DESCRIPTION
-----------

- lsqlite.open(database) returns an lsqlite object or nil
- db:exec(statements) executes one or more sqlite statments in one string and returns the output and an error description, if any
- db:close() closes an open database


BUILDING
--------

- building the library requires `gcc` with version < `9`.


EXAMPLE
-------

```lua
lsqlite = require("lsqlite")

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
lsqlite = require("lsqlite")

local db = lsqlite.open(":memory:")
```


TROUBLE SHOOTING
----------------

Currently there is an issue linking to the shared sqlite3.so on Debian
with gcc-9/-10. You may use the Makefile-Amalgamed to overcome this problem.

For this to work you must place sqlite3.c and sqlite3.h from an
archive downloaded from https://www.sqlite.org/download.html in this
directory.

Compilation:

```sh
make -f Makefile-Amalgamed
```


COPYRIGHT
---------

Copyright © 2010-2020 by Nico Hoffmann. License is MIT. For the complete
text see the supplied documentation.

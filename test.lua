local lsqlite = require("lsqlite")

-- open an in memory database
-- local db = lsqlite.open(":memory:")

-- open a file
local db = lsqlite.open("test.db")

-- open an inaccessible file
-- local db = lsqlite.open("/root/test.db")

-- check if we got an handle
if not db then print("could not open the database") return end

db:exec("drop table 'my_table';")
db:exec("create table 'my_table' ( 'a', 'b' );")
db:exec("insert into 'my_table' values ( 1, 2 );")
db:exec("insert into 'my_table' values ( 3, 4 );")
db:exec("insert into 'my_table' values ( 5, 6 );")
db:exec("insert into 'my_table' values ( -111, -222 );")
results, err=db:exec("select * from 'my_table';")

print(results, "error:", err)

local sum=0
for i = 1, #results do
    for k, v in pairs( results[i] ) do
    print( "row:", i, "key:", k, "value:", v )
    sum=sum+v
    end
end

print("sum: " .. sum)

db:close()


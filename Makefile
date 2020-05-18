CC=gcc
LD=$(CC)
LF=-shared -lsqlite3
RM=rm -rf

# --- lua5.1 ---
# CF=-Wall -Wno-unused -I/usr/include/lua5.1

# --- lua5.2 ---
# CF=-Wall -Wno-unused -I/usr/include/lua5.2

# --- lua5.3 ---
CF=-Wall -Wno-unused -I/usr/include/lua5.3

all: lsqlite.so

lsqlite.so: lsqlite.o
	$(LD) $(LF) -o $@ $<

lsqlite.o: lsqlite.c
	$(CC) $(CF) -c -o $@ $<

test: all
	lua test.lua

re: clean all

clean:
	$(RM) lsqlite.so lsqlite.o test.db




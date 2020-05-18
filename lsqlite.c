#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>


/**
 * this is the data structure every database instance will hold.
 * it contains a pointer to the sqlite3 data of the initialised database and
 * a pointer to the name of the database, as given by the user.
 */
typedef struct lsqlite_data lsqlite_data;
struct lsqlite_data
{
    sqlite3 *p;
    char *n;
};


/**
 * register our modules data structure to lua.
 * 
 * @param  L     the lua state
 * @param  index i dont remember, sorry
 * @return       a new instance of our data structure
 */
static lsqlite_data* check_lsqlite(lua_State *L, int index)
{
    return (lsqlite_data*) luaL_checkudata(L, index, "lsqlite.DB");
}


/**
 * db = lsqlite.open(filename) opens a sqlite3 database file.
 * filename might be ':memory:' to get an in memory database.
 * 
 * @param  L the lua state
 * @return   int(1); a handle to this modules data structure or nil
 */
static int lsqlite_open(lua_State *L)
{
    sqlite3 *db;
    const char *fn;
    
    if( lua_gettop(L) != 1 )
    {
        /* wrong count of arguments */
        luaL_error(L, "usage: lsqlite.open( filename )");
    }

    fn = luaL_checkstring(L,1);
    if( sqlite3_open( fn, &db ) == SQLITE_OK )
    {
        /* setup the local data */
        lsqlite_data *self = (lsqlite_data*) lua_newuserdata( L, sizeof(lsqlite_data) );
        self->p = db;
        self->n = strdup(fn);
        luaL_getmetatable(L, "lsqlite.DB");
        lua_setmetatable(L, -2);
    }
    else
    {
        /* err, what was this good for, when i did it? */
        if( db )
        {
            sqlite3_close(db);
        }
        lua_pushnil(L);
    }

    /* return the metatable or nil */
    return 1;
}


/**
 * db:close() closes a database handle and invalidates it.
 * 
 * @param  L the lua state
 * @return   int(0); nothing
 */
static int lsqlite_close(lua_State *L)
{
    lsqlite_data *self = check_lsqlite(L, 1);
    sqlite3_close(self->p);
    self->p = NULL;
    free(self->n);
    self->n = NULL;

    /* no results */
    return 0;
}


/**
 * results, error = db:exec(statements) executes statements and returns the
 * results, if any and a possible error.
 * 
 * @param  L the lua state
 * @return   int(2); rows, error
 */
static int lsqlite_exec(lua_State *L)
{
    lsqlite_data *self = check_lsqlite(L, 1);
    sqlite3 *db = self->p;
    char *err = NULL;
    int index = 0;
    
    /* this is fairly old c code style but it spares us a lot of additional
       functionalities */
    int cb( void *p, int argc, char **argv, char **args )
    {
        int i;

        /* this creates an indexed and a row of data, the metatable was
           already created before the call to cb(...) */
        lua_pushnumber(L, ++index);
        lua_newtable( L );

        /* now loop the columns in the result and add it to the metatable */
        for(i=0; i<argc; i++)
        {
            double value;
            char *endptr = NULL;

            /* push the column name */
            lua_pushstring(L, args[i]);

            /* try to get a double or a string */
            value = strtod(argv[i], &endptr);
            if( *endptr==(0) ) lua_pushnumber(L, value);
            else lua_pushstring(L, argv[i]);

            /* push it to the metatable */
            lua_settable(L, -3);
        }

        /* finish the metatable we were given on calling of cb(...) */
        lua_settable(L, -3);
        return 0;
    }
    
    /* check the arguments */
    if( lua_gettop(L) != 2 ){ luaL_error(L, "usage: lsqlite.exec( statements )"); }
    const char *cmd = luaL_checkstring(L,2);
    lua_pushnumber(L, 1);
    lua_newtable( L );
    sqlite3_exec( db, cmd, cb, (0), &err );
    lua_pushstring(L, err);

    /* return the results and the error, if any */
    return 2;
}


/**
 * when you leave lua without closing the database, the garbage collector
 * will clean up for us.
 * 
 * @param  L the lua state, again
 * @return   int(0); nothing
 */
static int lsqlite__gc(lua_State *L)
{
    lsqlite_data *self = check_lsqlite(L, 1);
    lsqlite_close(L);
    return 0;
}


/**
 * a string representation of a db instance.
 * 
 * @param  L the lua state, over and over again
 * @return   int(1); some string like "SQLite Database (<name of it>)" or "SQLite Database (closed)"
 */
static int lsqlite__tostring(lua_State *L)
{
    lsqlite_data *self = check_lsqlite(L, 1);
    if(self->p) lua_pushfstring(L, "SQLite Database (%s)", self->n);
    else lua_pushstring(L, "SQLite Database (closed)");
    return 1;
}


/**
 * the map of methods a database object exposes to lua.
 */
static const luaL_Reg lsqlite_method_map[] =
{
    {"close",lsqlite_close},
    {"exec",lsqlite_exec},
    {"__gc",lsqlite__gc},
    {"__tostring",lsqlite__tostring},
    {NULL,NULL}
};


/**
 * the methods this module exposes to lua.
 */
static const luaL_Reg lsqlite_module[] =
{
    {"open",lsqlite_open},
    {NULL,NULL}
};


/**
 * the loader called when our shared library is loaded.
 * 
 * @param  L the lua state
 * @return   int(1); our module in luas format
 */
int luaopen_lsqlite(lua_State *L)
{

    /* The module system has changed. This switches the behaviour. */
#if LUA_VERSION_NUM == 501
    luaL_register(L, "lsqlite", lsqlite_module);
#else
    luaL_newlib(L, lsqlite_module);
#endif

    luaL_newmetatable(L, "lsqlite.DB");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    /* The module system has changed. This switches the behaviour. */
#if LUA_VERSION_NUM == 501
    luaL_register(L, NULL, lsqlite_method_map);
#else
    luaL_setfuncs(L, lsqlite_method_map, 0);
#endif

    lua_pop(L,1);


    return 1;
}

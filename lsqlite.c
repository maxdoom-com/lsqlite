#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>


typedef struct lsqlite_data lsqlite_data;
struct lsqlite_data
{
	sqlite3 *p;
	char *n;
};

static lsqlite_data* check_lsqlite(lua_State *L, int index)
{
	return (lsqlite_data*) luaL_checkudata(L, index, "lsqlite.DB");
}

static int lsqlite_open(lua_State *L)
{
	sqlite3 *db;
	const char *fn;
	if( lua_gettop(L) != 1 )
	{
		luaL_error(L, "usage: lsqlite.open( filename )");
	}
	fn = luaL_checkstring(L,1);
	if( sqlite3_open( fn, &db ) == SQLITE_OK )
	{
		lsqlite_data *self = (lsqlite_data*) lua_newuserdata( L, sizeof(lsqlite_data) );
		self->p = db;
		self->n = strdup(fn);
		luaL_getmetatable(L, "lsqlite.DB");
		lua_setmetatable(L, -2);
	}
	else
	{
		if( db )
		{
				sqlite3_close(db);
			}
			lua_pushnil(L);
	}
	return 1;
}

static int lsqlite_close(lua_State *L)
{
	lsqlite_data *self = check_lsqlite(L, 1);
	sqlite3_close(self->p);
	self->p = NULL;
	free(self->n);
	self->n = NULL;
	return 0;
}

static int lsqlite_exec(lua_State *L)
{
	lsqlite_data *self = check_lsqlite(L, 1);
	sqlite3 *db = self->p;
	char *err = NULL;
	int index = 0;
	
	int cb( void *p, int argc, char **argv, char **args )
	{
		int i;
		lua_pushnumber(L, ++index);
		lua_newtable( L );
		for(i=0; i<argc; i++)
		{
			double value;
			char *endptr = NULL;
				lua_pushstring(L, args[i]);
				value = strtod(argv[i], &endptr);
				if( *endptr==(0) ) lua_pushnumber(L, value);
				else				lua_pushstring(L, argv[i]);
				/*{ fprintf(stderr,"lsqlite.exec.cb... %s = %s (%i)\n",args[i],argv[i], *endptr); }*/
				lua_settable(L, -3);
		}
		lua_settable(L, -3);
		return 0;
	}
	
	if( lua_gettop(L) != 2 ){ luaL_error(L, "usage: lsqlite.exec( statements )"); }
	const char *cmd = luaL_checkstring(L,2);
	lua_pushnumber(L, 1);
	lua_newtable( L );
	sqlite3_exec( db, cmd, cb, (0), &err );
	lua_pushstring(L, err);
	return 2;
}

static int lsqlite__gc(lua_State *L)
{
	lsqlite_data *self = check_lsqlite(L, 1);
	lsqlite_close(L);
	return 0;
}

static int lsqlite__tostring(lua_State *L)
{
	lsqlite_data *self = check_lsqlite(L, 1);
	if(self->p) lua_pushfstring(L, "SQLite Database (%s)", self->n);
	else		lua_pushstring(L, "SQLite Database (closed)");
	return 1;
}

static const luaL_Reg lsqlite_method_map[] =
{
	{"close",lsqlite_close},
	{"exec",lsqlite_exec},
	{"__gc",lsqlite__gc},
	{"__tostring",lsqlite__tostring},
	{NULL,NULL}
};

static const luaL_Reg lsqlite_module[] =
{
	{"open",lsqlite_open},
	{NULL,NULL}
};

int luaopen_lsqlite(lua_State *L)
{
	luaL_register(L, "lsqlite", lsqlite_module);
	luaL_newmetatable(L, "lsqlite.DB");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, lsqlite_method_map);
	lua_pop(L,1);
	return 1;
}

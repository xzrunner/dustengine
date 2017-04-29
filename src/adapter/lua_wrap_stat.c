#include "dust_statistics.h"

#include <lua.h>
#include <lauxlib.h>

static int
lenable_stat(lua_State* L) {
	bool enable = lua_toboolean(L, 1);
	stat_enable(enable);
	return 1;
}

static int
lset_stat_mem(lua_State* L) {
	float tot = lua_tonumber(L, 1);
	float lua = lua_tonumber(L, 2);
	stat_set_mem(tot, lua);
	return 0;
}

int
luaopen_stat_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "enable_stat", lenable_stat },
		{ "set_stat_mem", lset_stat_mem },
		{ NULL, NULL },		
	};
	luaL_newlib(L, l);
	return 1;
}
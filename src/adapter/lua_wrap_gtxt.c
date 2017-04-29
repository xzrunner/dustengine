#include "c_wrap_gum.h"

#include <lua.h>
#include <lauxlib.h>

static int
linit(lua_State* L) {
	int cap_bitmap = (int)(luaL_optinteger(L, 1, 50));
	int cap_layout = (int)(luaL_optinteger(L, 2, 500));
	gum_gtxt_init(cap_bitmap, cap_layout);
	return 0;
}

static int
lclear(lua_State* L) {
	gum_gtxt_clear();
	return 0;
}

static int
ladd_font(lua_State* L) {
	const char* name = luaL_checkstring(L, 1);
	const char* path = luaL_checkstring(L, 2);
	gum_gtxt_add_font(name, path);
	return 0;
}

static int
ladd_color(lua_State* L) {
	const char* key = luaL_checkstring(L, 1);
	unsigned int val = (unsigned int)(lua_tointeger(L, 2));
	gum_gtxt_add_color(key, val);
	return 0;
}

static int
ladd_user_font(lua_State* L) {
	const char* str = lua_tostring(L, 1);
	const char* pkg = lua_tostring(L, 2);
	const char* node = lua_tostring(L, 3);
	gum_gtxt_add_user_font_char(str, pkg, node);
	return 0;
}

int
luaopen_gtxt_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "init", linit },
		{ "clear", lclear },
		{ "add_font", ladd_font },
		{ "add_color", ladd_color },
		{ "add_user_font", ladd_user_font },
		{ NULL, NULL },		
	};
	luaL_newlib(L, l);
	return 1;
}

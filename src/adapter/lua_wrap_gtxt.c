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

static int
lprint(lua_State* L) {
	const char* str = luaL_checkstring(L, 1);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	int size = luaL_checkinteger(L, 4);

	uint8_t r, g, b, a;
	if (lua_istable(L, 5)) {
		for (int i = 1; i <= 4; i++) {
			lua_rawgeti(L, 5, i);
		}
		r = (uint8_t)luaL_checkinteger(L, -4);
		g = (uint8_t)luaL_checkinteger(L, -3);
		b = (uint8_t)luaL_checkinteger(L, -2);
		a = (uint8_t)luaL_optinteger(L, -1, 255); 
		lua_pop(L, 4);
	} else {
		r = (uint8_t)luaL_checkinteger(L, 1);
		g = (uint8_t)luaL_checkinteger(L, 2);
		b = (uint8_t)luaL_checkinteger(L, 3);
		a = (uint8_t)luaL_optinteger(L, 4, 255);
	}
	uint32_t color = r << 24 | g << 16 | b << 8 | a;

	gum_gtxt_print(str, x, y, size, color);

	return 0;
}

static int
lsize(lua_State* L) {
	const char* str = luaL_checkstring(L, 1);
	int size = luaL_checkinteger(L, 2);
	float w, h;
	gum_gtxt_size(str, size, &w, &h);
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

int
luaopen_gtxt_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "init", linit },
		{ "clear", lclear },
		{ "add_font", ladd_font },
		{ "add_color", ladd_color },
		{ "add_user_font", ladd_user_font },
		{ "print", lprint },
		{ "size", lsize },		
		{ NULL, NULL },		
	};
	luaL_newlib(L, l);
	return 1;
}

#include "dust_camera25.h"

#include <sm_c_vector.h>

#include <lua.h>
#include <lauxlib.h>

static int
lenable(lua_State* L) {
	bool enable = lua_toboolean(L, 1);
	dust_cam25_enable(enable);
	return 0;
}

static int
lis_closed(lua_State* L) {
	bool is_closed = dust_cam25_is_closed();
	lua_pushboolean(L, is_closed);
	return 1;
}

static int
ltranslate(lua_State* L) {
	struct sm_vec3 offset;
	offset.x = lua_tonumber(L, 1);
	offset.y = lua_tonumber(L, 2);
	offset.z = lua_tonumber(L, 3);
	dust_cam25_translate(&offset);
	return 0;
}

static int
lscreen_to_world(lua_State* L) {
	struct sm_ivec2 screen;
	screen.x = (int)(lua_tonumber(L, 1));
	screen.y = (int)(lua_tonumber(L, 2));
	struct sm_vec2 world;
	dust_cam25_screen_to_world(&world, &screen);
	lua_pushnumber(L, world.x);
	lua_pushnumber(L, world.y);
	return 2;
}

static int
lset_pos(lua_State* L) {
	struct sm_vec3 pos;
	pos.x = lua_tonumber(L, 1);
	pos.y = lua_tonumber(L, 2);
	pos.z = lua_tonumber(L, 3);
	dust_cam25_set_pos(&pos);
	return 0;
}

static int
lset_angle(lua_State* L) {
	float angle = lua_tonumber(L, 1);
	dust_cam25_set_angle(angle);
	return 0;
}

int
luaopen_c25_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "enable", lenable },
		{ "is_closed", lis_closed },

		{ "translate", ltranslate },
		{ "screen_to_world", lscreen_to_world },

		{ "set_pos", lset_pos },
		{ "set_angle", lset_angle },

		{ NULL, NULL },		
	};
	luaL_newlib(L, l);
	return 1;
}
#include "c_wrap_gum.h"
#include "c_wrap_sl.h"

#include <lua.h>
#include <lauxlib.h>

static int
lenable_graph(lua_State* L) {
	gum_stat_enable_graph(!gum_stat_is_graph_enable());
	return 0;
}

static int
lenable_console(lua_State* L) {
	gum_stat_enable_console(!gum_stat_is_console_enable());
	return 0;
}

static int
lenable_file(lua_State* L) {
	gum_stat_enable_file(!gum_stat_is_file_enable());
	return 0;
}

static int
lis_graph_enable(lua_State* L) {
	lua_pushboolean(L, gum_stat_is_graph_enable());
	return 1;
}

static int
lis_console_enable(lua_State* L) {
	lua_pushboolean(L, gum_stat_is_console_enable());
	return 1;
}

static int
lis_file_enable(lua_State* L) {
	lua_pushboolean(L, gum_stat_is_file_enable());
	return 1;
}

static int
lenable_tag(lua_State* L) {
	gum_stat_enable_tag(!gum_stat_is_tag_enable());
	return 0;
}

static int
lis_tag_enable(lua_State* L) {
	lua_pushboolean(L, gum_stat_is_tag_enable());
	return 1;
}

static int
lprint_tag(lua_State* L) {
	const char* type = lua_tostring(L, 1);
	const char* msg = lua_tostring(L, 2);
	gum_stat_print_tag(type, msg);
	return 0;
}

static int
lenable_opt(lua_State* L) {
	bool enable = lua_toboolean(L, 1);
	gum_stat_opt_enable(enable);
	return 0;
}

//static int
//lstat_set_render(lua_State* L) {
//	bool b = lua_toboolean(L, 1);
//	stat_set_render(b);
//	return 0;
//}
//
//
//static int
//lstat_get_render(lua_State* L) {
//	bool b = stat_get_render();
//	lua_pushboolean(L, b);
//	return 1;
//}

static int
lset_stat_mem(lua_State* L) {
	float tot = lua_tonumber(L, 1);
	float lua = lua_tonumber(L, 2);
	gum_stat_set_mem(tot, lua);
	return 0;
}

static int
lcheckpoint_begin(lua_State* L) {
	gum_stat_fps_begin();
	return 0;
}

static int
lcheckpoint_end(lua_State* L) {
	gum_stat_fps_end();
	return 0;
}

static int
lget_cp_fps(lua_State* L) {
	int fps = gum_stat_get_fps();
	lua_pushinteger(L, fps);
	return 1;
}

static int
lflush(lua_State* L) {
	gum_stat_flush();	
	return 0;
}

static int
lrecord_screen_set_enable(lua_State* L) {
	gum_record_screen_set_enable(!gum_record_screen_is_enable());
	return 0;
}

static int
lrecord_screen_is_enable(lua_State* L) {
	lua_pushboolean(L, gum_record_screen_is_enable());
	return 1;
}

static int
lrecord_screen_print(lua_State* L) {
	void* rt = lua_touserdata(L, 1);
	gum_record_screen_print(rt);
	return 0;
}

static int
lrecord_screen_flush(lua_State* L) {
	gum_record_screen_flush();
	return 0;
}

static int
lrecord_screen_clear(lua_State* L) {
	gum_record_screen_clear();
	return 0;
}

static int
lstat_set_render(lua_State* L) {
	bool b = lua_toboolean(L, 1);
	sl_stat_set_render(b);
	return 0;
}


int
luaopen_stat_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "enable_graph", lenable_graph },
		{ "enable_console", lenable_console },
		{ "enable_file", lenable_file },
		
		{ "stat_set_render" , lstat_set_render },

		{ "is_graph_enable", lis_graph_enable },
		{ "is_console_enable", lis_console_enable },
		{ "is_file_enable", lis_file_enable },

		{ "enable_tag", lenable_tag },
		{ "is_tag_enable", lis_tag_enable },
		{ "print_tag", lprint_tag },

		{ "enable_opt", lenable_opt },
		{ "set_stat_mem", lset_stat_mem },
		//{ "set_stat_render", lstat_set_render },
		//{ "get_stat_render", lstat_get_render },

		{ "checkpoint_begin", lcheckpoint_begin },
		{ "checkpoint_end", lcheckpoint_end },
		{ "get_cp_fps", lget_cp_fps },

		{ "flush", lflush },

		{ "record_screen_set_enable", lrecord_screen_set_enable },
		{ "record_screen_is_enable", lrecord_screen_is_enable },
		{ "record_screen_print", lrecord_screen_print },
		{ "record_screen_flush", lrecord_screen_flush },
		{ "record_screen_clear", lrecord_screen_clear },

		{ NULL, NULL },		
	};
	luaL_newlib(L, l);
	return 1;
}

#include "dust_shaderlab.h"

#include <c_wrap_sl.h>

#include <lua.h>
#include <lauxlib.h>

#include <string.h>

static int
lblend(lua_State* L) {
	if (lua_isnoneornil(L,1)) {
		sl_set_default_blend();
	} else {
		int m1 = (int)luaL_checkinteger(L,1);
		int m2 = (int)luaL_checkinteger(L,2);
		sl_set_blend(m1, m2);
	}
	return 0;
}

static int
lclear(lua_State* L) {
	uint32_t c = (uint32_t)(luaL_optinteger(L, 1, 0xff000000));
	sl_render_clear(c);
	return 0;
}

static int
lversion(lua_State* L) {
	lua_pushinteger(L, sl_render_version());
	return 1;
}

static int
lset_heat_haze_tex(lua_State* L) {
	int id = lua_tointeger(L, -1);
	sl_filter_set_heat_haze_texture(id);
	return 0;
}

static int
lset_burning_map_height_tex(lua_State* L) {
	int id = lua_tointeger(L, -1);
	sl_filter_set_burning_map_height_texture(id);
	return 0;
}

static int
lset_burning_map_border_tex(lua_State* L) {
	int id = lua_tointeger(L, -1);
	sl_filter_set_burning_map_border_texture(id);
	return 0;
}

static int
lset_color_grading_tex(lua_State* L) {
	int id = lua_tointeger(L, -1);
	sl_filter_set_col_grading_texture(id);
	return 0;
}

static int
lset_shader(lua_State* L) {
	const char* shader = lua_tostring(L, -1);
	if (strcmp(shader, "SPRITE2") == 0) {
		sl_set_shader(ST_SPRITE2);
	} else if (strcmp(shader, "FILTER") == 0) {
		sl_set_shader(ST_FILTER);
	}
	return 0;
}

static int
lset_filter_mode(lua_State* L) { 
 	const char* filter = lua_tostring(L, 1);
 	if (strcmp(filter, "GRAY") == 0) {
 		sl_filter_set_mode(SLFM_GRAY);
 	} else if (strcmp(filter, "BURNING_MAP") == 0) {
 		sl_filter_set_mode(SLFM_BURNING_MAP);
		int texid = lua_tointeger(L, 2);
		sl_filter_set_burning_map_upper_texture(texid);
 	} else if (strcmp(filter, "GAUSSIAN_BLUR_HORI") == 0) {
 		sl_filter_set_mode(SLFM_GAUSSIAN_BLUR_HORI);
 	} else if (strcmp(filter, "GAUSSIAN_BLUR_VERT") == 0) {
 		sl_filter_set_mode(SLFM_GAUSSIAN_BLUR_VERT);
 	} else if (strcmp(filter, "COLOR_GRADING") == 0) {
 		sl_filter_set_mode(SLFM_COL_GRADING);
 	}
	return 0;
}

static int
lclear_filter_time(lua_State* L) {
	sl_filter_clear_time();
	return 0;
}

int
luaopen_sl_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "blend", lblend },
		{ "clear", lclear },
		{ "version", lversion },

		{ "set_heat_haze_tex", lset_heat_haze_tex },
		{ "set_burning_map_height_tex", lset_burning_map_height_tex },
		{ "set_burning_map_border_tex", lset_burning_map_border_tex },
		{ "set_color_grading_tex", lset_color_grading_tex },

		{ "set_shader", lset_shader },
		{ "set_filter_mode", lset_filter_mode },

		{ "clear_filter_time", lclear_filter_time },

		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}
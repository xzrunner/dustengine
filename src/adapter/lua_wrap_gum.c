#include <c_wrap_gum.h>
#include <c_wrap_sl.h>

#include <lua.h>
#include <lauxlib.h>

#include <assert.h>

static int
lgc(lua_State* L) {
	gum_gc();
	return 0;
}

static int
lget_sym_count(lua_State* L) {
	int count = gum_get_sym_count();
	lua_pushinteger(L, count);
	return 1;
}

static int
lget_img_count(lua_State* L) {
	int count = gum_get_img_count();
	lua_pushinteger(L, count);
	return 1;
}

static int
lis_pkg_exists(lua_State* L) {
	const char* name = lua_tostring(L, 1);
	bool exists = gum_pkg_exists(name);
	lua_pushboolean(L, exists);
	return 1;
}

static int
lnew_pkg(lua_State* L) {
	const char* name = lua_tostring(L, 1);
	int id = lua_tointeger(L, 2);
	const char* spr_path = lua_tostring(L, 3);
	const char* tex_path = lua_tostring(L, 4);
	bool succ = gum_create_pkg(name, id, spr_path, tex_path);
	if (!succ) {
		luaL_error(L, "create pkg %s fail.", name);
	}
	return 0;
}

static int
lpkg_get_page_count(lua_State* L) {
	const char* name = lua_tostring(L, 1);
	int count = gum_pkg_get_page_count(name);
	lua_pushinteger(L, count);
	return 1;
}

static int
lpkg_set_page_filepath(lua_State* L) {
	const char* name = lua_tostring(L, 1);
	int page = lua_tointeger(L, 2);
	const char* filepath = lua_tostring(L, 3);
	gum_pkg_set_page_filepath(name, page, filepath);
	return 0;
}

static int
lpkg_get_tex_count(lua_State* L) {
	int id = lua_tointeger(L, 1);
	int tex, lod;
	gum_pkg_get_texture_count(id, &tex, &lod);
	if (tex <= 0) {
		luaL_error(L, "pkg get texture count fail.");
	}
	if (lod < 0) {
		luaL_error(L, "pkg get texture lod fail.");
	}

	lua_pushinteger(L, tex);
	lua_pushinteger(L, lod);

	return 2;
}

static int
lpkg_set_tex_filepath(lua_State* L) {
	int id = lua_tointeger(L, 1);
	int tex = lua_tointeger(L, 2);
	int lod = lua_tointeger(L, 3);
	const char* filepath = lua_tostring(L, 4);
	gum_pkg_set_texture_filepath(id, tex, lod, filepath);
	return 0;
}

static int
lstore_snapshot(lua_State* L) {
	const char* filepath = lua_tostring(L, 1);
	sl_flush();
	gum_store_snapshot(filepath);
	return 0;
}

static int
lcompare_snapshot(lua_State* L) {
	const char* filepath = lua_tostring(L, 1);
	sl_flush();
	int cmp = gum_compare_snapshot(filepath);
	lua_pushinteger(L, cmp);
	return 1;
}

static int
lget_screen_size(lua_State* L) {
	int w, h;
	gum_get_screen_size(&w, &h);
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 2;
}

int
luaopen_gum_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "gc", lgc },
		{ "get_sym_count", lget_sym_count },
		{ "get_img_count", lget_img_count },
		{ "is_pkg_exists", lis_pkg_exists },
		{ "new_pkg", lnew_pkg },
		{ "pkg_get_page_count", lpkg_get_page_count },
		{ "pkg_set_page_filepath", lpkg_set_page_filepath },
		{ "pkg_get_tex_count", lpkg_get_tex_count },
		{ "pkg_set_tex_filepath", lpkg_set_tex_filepath },
		{ "store_snapshot", lstore_snapshot },
		{ "compare_snapshot", lcompare_snapshot },
		{ "get_screen_size", lget_screen_size },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}
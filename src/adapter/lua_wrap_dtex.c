#include <c_wrap_dtex.h>

#include <lua.h>
#include <lauxlib.h>

static int
lquery_cache(lua_State* L) {
	const char* key = lua_tostring(L, 1);
	void* cache = dtex_query_cache(key);
	lua_pushlightuserdata(L, cache);
	return 1;
}

static int
lloading_task_empty(lua_State* L) {
	bool empty = dtex_loading_task_empty();
	lua_pushboolean(L, empty);
	return 1;
}

static int
lcache_pkg_static_query(lua_State* L) {
	const char* key = lua_tostring(L, 1);
	void* ret = dtex_cache_pkg_static_query(key);
	if (ret) {
		lua_pushlightuserdata(L, ret);
		return 1;
	} else {
		return 0;
	}
}

static int
lcache_pkg_static_create(lua_State* L) {
	const char* key = lua_tostring(L, 1);
	int tex_size = lua_tointeger(L, 2);
	void* ret = dtex_cache_pkg_static_create(key, tex_size);
	lua_pushlightuserdata(L, ret);
	return 1;
}

static int
lcache_pkg_static_delete(lua_State* L) {
	const char* key = lua_tostring(L, 1);
	dtex_cache_pkg_static_delete(key);
	return 0;
}

static int
lcache_pkg_static_load(lua_State* L) {
	void* cache = lua_touserdata(L, 1);
	int pkg_id = lua_tointeger(L, 2);
	dtex_cache_pkg_static_load(cache, pkg_id);
	return 0;
}

static int
lcache_pkg_static_load_finish(lua_State* L) {
	void* cache = lua_touserdata(L, 1);
	bool async = lua_toboolean(L, 2);
	dtex_cache_pkg_static_load_finish(cache, async);
	return 0;
}

static int
lcache_pkg_static_is_empty(lua_State* L) {
	void* cache = lua_touserdata(L, 1);
	bool empty = dtex_cache_pkg_static_is_empty(cache);
	lua_pushboolean(L, empty);
	return 1;
}

int
luaopen_dtex2_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "query_cache", lquery_cache },
		{ "loading_task_empty", lloading_task_empty },

		{ "cache_pkg_static_query", lcache_pkg_static_query },
		{ "cache_pkg_static_create", lcache_pkg_static_create },
		{ "cache_pkg_static_delete", lcache_pkg_static_delete },

		{ "cache_pkg_static_load", lcache_pkg_static_load },
		{ "cache_pkg_static_load_finish", lcache_pkg_static_load_finish },

		{ "cache_pkg_static_is_empty", lcache_pkg_static_is_empty },

		{ NULL, NULL },		
	};
	luaL_newlib(L, l);
	return 1;
}
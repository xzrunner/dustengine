#include <c_wrap_m3.h>

#include <lua.h>
#include <lauxlib.h>

#include <string.h>

static int
lcreate_surface(lua_State* L) {
	const char* type = luaL_checkstring(L, 1);
	void* surface = NULL;
	if (strcmp(type, "cone") == 0) {
		float height = luaL_optnumber(L, 2, 1);
		float radius = luaL_optnumber(L, 3, 1);
		surface = m3_create_cone(height, radius);
	} else if (strcmp(type, "sphere") == 0) {
		float radius = luaL_optnumber(L, 2, 1);
		surface = m3_create_sphere(radius);
	} else if (strcmp(type, "torus") == 0) {
		float major_radius = luaL_optnumber(L, 2, 1);
		float minor_radius = luaL_optnumber(L, 3, 1);
		surface = m3_create_torus(major_radius, minor_radius);
	} else if (strcmp(type, "trefoi_knot") == 0) {
		float scale = luaL_optnumber(L, 2, 1);
		surface = m3_create_trefoi_knot(scale);
	} else if (strcmp(type, "mobius_strip") == 0) {
		float scale = luaL_optnumber(L, 2, 1);
		surface = m3_create_mobius_strip(scale);
	} else if (strcmp(type, "klein_bottle") == 0) {
		float scale = luaL_optnumber(L, 2, 1);
		surface = m3_create_klein_bottle(scale);
	}

	if (surface) {
		lua_pushlightuserdata(L, surface);
		return 1;
	} else {
		return 0;
	}
}

int
luaopen_m3_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "create_surface", lcreate_surface },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}
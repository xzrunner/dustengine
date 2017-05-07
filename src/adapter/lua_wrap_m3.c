#include <c_wrap_m3.h>

#include <sm_c_vector.h>

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

static int
lcam_create(lua_State* L) {
	void* cam = m3_cam_create();
	lua_pushlightuserdata(L, cam);
	return 1;
}

static int
lcam_release(lua_State* L) {
	void* cam = lua_touserdata(L, 1);
	if (!cam) {
		luaL_error(L, "Error passed cam");
	}
	m3_cam_release(cam);
	return 0;
}

static int
lcam_rotate(lua_State* L) {
	void* cam = lua_touserdata(L, 1);
	if (!cam) {
		luaL_error(L, "Error passed cam");
	}
	
	if (!lua_istable(L, 2) || lua_rawlen(L, 2) != 3 ||
		!lua_istable(L, 3) || lua_rawlen(L, 3) != 3) {
		return 0;
	}

	struct sm_vec3 begin;
	lua_rawgeti(L, 2, 1);
	begin.x = luaL_checknumber(L, -1);
	lua_rawgeti(L, 2, 2);
	begin.y = luaL_checknumber(L, -1);
	lua_rawgeti(L, 2, 3);
	begin.z = luaL_checknumber(L, -1);

	struct sm_vec3 end;
	lua_rawgeti(L, 3, 1);
	end.x = luaL_checknumber(L, -1);
	lua_rawgeti(L, 3, 2);
	end.y = luaL_checknumber(L, -1);
	lua_rawgeti(L, 3, 3);
	end.z = luaL_checknumber(L, -1);

	lua_pop(L, 6);

	m3_cam_rotate(cam, &begin, &end);

	return 0;
}

static int
lcam_translate(lua_State* L) {
	void* cam = lua_touserdata(L, 1);
	if (!cam) {
		luaL_error(L, "Error passed cam");
	}

	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float z = luaL_checknumber(L, 4);
	m3_cam_translate(cam, x, y, z);

	return 0;
}

static int
lcam_get_mat(lua_State* L) {
	void* cam = lua_touserdata(L, 1);
	if (!cam) {
		luaL_error(L, "Error passed cam");
	}
	if (!lua_istable(L, 2)) {
		return 0;
	}

	float mat[16];
	m3_cam_get_mat(cam, mat);

	for (int i = 0; i < 16; ++i) {
		lua_pushnumber(L, mat[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 0;
}

int
luaopen_m3_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "create_surface", lcreate_surface },

		{ "cam_create", lcam_create },
		{ "cam_release", lcam_release },
		{ "cam_rotate", lcam_rotate },	
		{ "cam_translate", lcam_translate },	
		{ "cam_get_mat", lcam_get_mat },	

		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}
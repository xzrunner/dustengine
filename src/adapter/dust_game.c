#include "dust_game.h"
#include "dust_shaderlab.h"

#include <c_wrap_sl.h>
#include <fault.h>
#include <glp_loop.h>
#include <c_wrap_gum.h>
#include <c_wrap_s2.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <assert.h>
#include <stdlib.h>

//#define LOGIC_FRAME 30

#define DUST_LOAD "DUST_LOAD"
#define DUST_UPDATE "DUST_UPDATE"
#define DUST_DRAW "DUST_DRAW"
#define DUST_TOUCH "DUST_TOUCH"
#define DUST_KEY "DUST_KEY"
#define DUST_MESSAGE "DUST_MESSAGE"
#define DUST_GESTURE "DUST_GESTURE"
#define DUST_HANDLE_ERROR "DUST_HANDLE_ERROR"
#define DUST_RESUME "DUST_RESUME"
#define DUST_PAUSE "DUST_PAUSE"

#define TRACEBACK_FUNCTION 1
#define UPDATE_FUNCTION 2
#define DRAWFRAME_FUNCTION 3
#define TOP_FUNCTION 3

static int LOGIC_FRAME = 30;

static int
_panic(lua_State* L) {
	const char * err = lua_tostring(L,-1);
	fault("%s", err);
	return 0;
}

static int
linject(lua_State* L) {
	static const char* dust_callback[] = {
		DUST_LOAD,
		DUST_UPDATE,
		DUST_DRAW,
		DUST_TOUCH,
		DUST_KEY,
		DUST_MESSAGE,
	};
	int i;
	for (i=0;i<sizeof(dust_callback)/sizeof(dust_callback[0]);i++) {
		lua_getfield(L, lua_upvalueindex(1), dust_callback[i]);
		if (!lua_isfunction(L,-1)) {
			return luaL_error(L, "%s is not found", dust_callback[i]);
		}
		lua_setfield(L, LUA_REGISTRYINDEX, dust_callback[i]);
	}
	return 0;
}

static int
lset_screen(lua_State* L) {
	int w = (int)luaL_checkinteger(L, 1);
	int h = (int)luaL_checkinteger(L, 2);
	float scale = lua_tonumber(L, 3);
	dust_on_size(w * scale, h * scale);
	return 0;
}

static int
dust_framework(lua_State* L) {
	luaL_Reg l[] = {
		{ "inject", linject },
		{ "set_screen", lset_screen },
		{ NULL, NULL },
	};
	luaL_newlibtable(L, l);
	lua_pushvalue(L,-1);
	luaL_setfuncs(L,l,1);
	return 1;
}

static void
checkluaversion(lua_State* L) {
	const lua_Number *v = lua_version(L);
	if (v != lua_version(NULL))
		fault("multiple Lua VMs detected");
	else if (*v != LUA_VERSION_NUM) {
		fault("Lua version mismatch: app. needs %f, Lua core provides %f",
			LUA_VERSION_NUM, *v);
	}
}



#if __ANDROID__
#define OS_STRING "ANDROID"
#elif _MSC_VER
#define OS_STRING "MSC"
#else
#define STR_VALUE(arg)	#arg
#define _OS_STRING(name) STR_VALUE(name)
#define OS_STRING _OS_STRING(DUST_OS)
#endif

static void
lua_init_aux(struct game *G) {
	G->L = luaL_newstate();
}

static char *
memory_status_(struct game *G) {
	return NULL;
}

#define release_memory(G)	((void)0)
#define memory_report_(G)	((void)0)

lua_State *
dust_lua_init(struct game *G) {
	lua_init_aux(G);
	lua_State* L = G->L;
	if (L == NULL) {
		return NULL;
	}
	checkluaversion(L);
	lua_pushliteral(L, OS_STRING);
	lua_setglobal(L , "OS");
	lua_atpanic(L, _panic);
	luaL_openlibs(L);
	return L;
}


struct game *
dust_game() {
	struct game *G = (struct game *)malloc(sizeof(*G));
	lua_State* L = dust_lua_init(G);
	if (L == NULL) {
		return NULL;
	}

	G->L = L;
	G->real_time = 0.0f;
	G->logic_time = 0.0f;
	G->hold_update_time = 0.0f;
	luaL_requiref(L, "dust.framework", dust_framework, 0);

	lua_settop(L,0);

	s2_init();
	dust_sl_init();

	return G;
}

void
dust_close_lua(struct game *G) {
	if (G) {
		if (G->L) {
			lua_close(G->L);
			G->L = NULL;
		}
		release_memory(G);
	}
}

void
dust_game_exit(struct game *G) {
	gum_clear();

	dust_close_lua(G);

	if (G) {
		free(G);
		G = NULL;
	}
}

lua_State *
dust_game_lua(struct game *G) {
	return G->L;
}

static int
traceback(lua_State* L) {
	const char* msg = lua_tostring(L, 1);
	if (msg) {
		luaL_traceback(L, L, msg, 1);
		printf("traceback %s\n", msg);
	} else if (!lua_isnoneornil(L, 1)) {
		if (!luaL_callmeta(L, 1, "__tostring")) {
			lua_pushliteral(L, "(no error message)");
		}
	}
	return 1;
}

void
dust_game_logicframe(int frame)
{
	LOGIC_FRAME = frame;
}

void
dust_game_start(struct game *G) {
	lua_State* L = G->L;
	lua_getfield(L, LUA_REGISTRYINDEX, DUST_LOAD);
	lua_call(L, 0, 0);
	assert(lua_gettop(L) == 0);
	lua_pushcfunction(L, traceback);
	lua_getfield(L, LUA_REGISTRYINDEX, DUST_UPDATE);
	lua_getfield(L, LUA_REGISTRYINDEX, DUST_DRAW);
}

void 
dust_handle_error(lua_State* L, const char *err_type, const char *msg) {
	lua_getfield(L, LUA_REGISTRYINDEX, DUST_HANDLE_ERROR);
	lua_pushstring(L, err_type);
	lua_pushstring(L, msg);
	int err = lua_pcall(L, 2, 0, 0);
	switch(err) {
	case LUA_OK:
		break;
	case LUA_ERRRUN:
		fault("!LUA_ERRRUN : %s\n", lua_tostring(L,-1));
		break;
	case LUA_ERRMEM:
		fault("!LUA_ERRMEM : %s\n", lua_tostring(L,-1));
		break;
	case LUA_ERRERR:
		fault("!LUA_ERRERR : %s\n", lua_tostring(L,-1));
		break;
	case LUA_ERRGCMM:
		fault("!LUA_ERRGCMM : %s\n", lua_tostring(L,-1));
		break;
	default:
		fault("!Unknown Lua error: %d\n", err);
		break;
	}
}

static int
call(lua_State* L, int n, int r) {
	int err = lua_pcall(L, n, r, TRACEBACK_FUNCTION);
	switch(err) {
	case LUA_OK:
		break;
	case LUA_ERRRUN:
		dust_handle_error(L, "LUA_ERRRUN", lua_tostring(L,-1));
		fault("!LUA_ERRRUN : %s\n", lua_tostring(L,-1));
		break;
	case LUA_ERRMEM:
		dust_handle_error(L, "LUA_ERRMEM", lua_tostring(L,-1));
		fault("!LUA_ERRMEM : %s\n", lua_tostring(L,-1));
		break;
	case LUA_ERRERR:
		dust_handle_error(L, "LUA_ERRERR", lua_tostring(L,-1));
		fault("!LUA_ERRERR : %s\n", lua_tostring(L,-1));
		break;
	case LUA_ERRGCMM:
		dust_handle_error(L, "LUA_ERRGCMM", lua_tostring(L,-1));
		fault("!LUA_ERRGCMM : %s\n", lua_tostring(L,-1));
		break;
	default:
		dust_handle_error(L, "UnknownError", "Unknown");
		fault("!Unknown Lua error: %d\n", err);
		break;
	}
	return err;
}

void
dust_call_lua(lua_State* L, int n, int r) {
  call(L, n, r);
	lua_settop(L, TOP_FUNCTION);
}

static void
logic_frame(lua_State* L, float time) {
	lua_pushvalue(L, UPDATE_FUNCTION);
	lua_pushnumber(L, time);
	call(L, 1, 0);
	lua_settop(L, TOP_FUNCTION);
}

void
dust_game_update(struct game *G, float time) {
	if (G->logic_time == 0) {
		G->real_time = 1.0f/LOGIC_FRAME;
	} else {
		G->real_time += time;
	}
	G->hold_update_time += time;
	while (G->logic_time < G->real_time) {
		logic_frame(G->L, G->hold_update_time);
		float dt = 1.0f/LOGIC_FRAME;
		G->logic_time += dt;
		gum_update(dt);
		G->hold_update_time = 0.0f;
	}
	gum_flush();
}

void
dust_game_drawframe(struct game *G, float delta_time) {
	sl_filter_update(delta_time);

	lua_pushvalue(G->L, DRAWFRAME_FUNCTION);
	lua_pushnumber(G->L, delta_time);
	call(G->L, 1, 0);
	lua_settop(G->L, TOP_FUNCTION);
	sl_flush();

	gum_stat_no_stat_begin();
	gum_debug_draw();
	gum_stat_update();
	gum_stat_print();
	gum_stat_reset();
	gum_stat_no_stat_end();
}

int
dust_game_touch(struct game *G, int id, float x, float y, int status) {
	int disable_gesture = 0;
	lua_getfield(G->L, LUA_REGISTRYINDEX, DUST_TOUCH);
	lua_pushnumber(G->L, x);
	lua_pushnumber(G->L, y);
	lua_pushinteger(G->L, status+1);
	lua_pushinteger(G->L, id);
	int err = call(G->L, 4, 1);
	if (err == LUA_OK) {
		disable_gesture = lua_toboolean(G->L, -1);
	}
	lua_settop(G->L, TOP_FUNCTION);
	return disable_gesture;
}

void
dust_game_gesture(struct game *G, int type,
					double x1, double y1,double x2,double y2, int s) {
						lua_getfield(G->L, LUA_REGISTRYINDEX, DUST_GESTURE);
						lua_pushnumber(G->L, type);
						lua_pushnumber(G->L, x1);
						lua_pushnumber(G->L, y1);
						lua_pushnumber(G->L, x2);
						lua_pushnumber(G->L, y2);
						lua_pushinteger(G->L, s);
						call(G->L, 6, 0);
						lua_settop(G->L, TOP_FUNCTION);
}

void
dust_game_message(struct game* G,int id, const char* state, const char* data, lua_Number n) {
	lua_State* L = G->L;
	lua_getfield(L, LUA_REGISTRYINDEX, DUST_MESSAGE);
	lua_pushnumber(L, id);
	lua_pushstring(L, state);
	lua_pushstring(L, data);
	lua_pushnumber(L, n);
	call(L, 4, 0);
	lua_settop(L, TOP_FUNCTION);
}

void
dust_game_resume(struct game* G){
	lua_State* L = G->L;
	lua_getfield(L, LUA_REGISTRYINDEX, DUST_RESUME);
	call(L, 0, 0);
	lua_settop(L, TOP_FUNCTION);
}

void
dust_game_pause(struct game* G) {
	lua_State* L = G->L;
	lua_getfield(L, LUA_REGISTRYINDEX, DUST_PAUSE);
	call(L, 0, 0);
	lua_settop(L, TOP_FUNCTION);
}

void 
dust_on_size(int w, int h) {
	dust_sl_on_size(w, h);
	s2_on_size(w, h);
	gum_on_size(w, h);
}

void
dust_game_memory_report(struct game * G) {
	memory_report_(G);
}

char *
dust_game_memory_status(struct game *G) {
	return memory_status_(G);
}

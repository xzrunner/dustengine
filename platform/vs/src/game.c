#include "game.h"

#include <dust_game.h>
#include <fs_file.h>
#include <fault.h>

#include <windows.h>

#include <stdlib.h>
#include <stdint.h>

#include <rc4.h>
#include <LzmaDec.h>
#include <Alloc.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "sysutil.h"

int luaopen_dtex2_c(lua_State* L);
int luaopen_gtxt_c(lua_State* L);
int luaopen_sl_c(lua_State* L);
int luaopen_c25_c(lua_State* L);
int luaopen_stat_c(lua_State *L);
int luaopen_s2_c(lua_State *L);
int luaopen_gum_c(lua_State *L);

#define WIDTH 800
#define HEIGHT 600

typedef struct {
	HWND wnd;
	struct game *game;
	DWORD last_update;
	DWORD last_draw;
	int reload_flag;
} game_t;

static game_t *G = NULL;
static int last_width = 0;
static int last_height = 0;
static float last_scale = 0.0f;

static const char * startscript =
"local path, script = ...\n"
"require(\"dust.framework\").WorkDir = ''\n"
"assert(script, 'I need a script name')\n"
"path = string.match(path,[[(.*)\\[^\\]*$]])\n"
"package.path = path .. [[\\?.lua;]] .. path .. [[\\?\\init.lua;.\\src\\scripts\\?.lua;.\\src\\scripts\\init.lua]]\n"
"print(path)"
"print(package.path)"
"local f = assert(loadfile(script))\n"
"f(script)\n"
;

struct game*
game_instance() {
	return G ? G->game : NULL;
}

static int
traceback(lua_State *L) {
	const char *msg = lua_tostring(L, 1);
	if (msg) {
		luaL_traceback(L, L, msg, 1);
	} else if (!lua_isnoneornil(L, 1)) {
		if (!luaL_callmeta(L, 1, "__tostring")) {
			lua_pushliteral(L, "(no error message)");
		}
	}
	return 1;
}

static void
reg_lua_lib(lua_State *L, lua_CFunction func, const char * libname) {
	luaL_requiref(L, libname, func, 0);
	lua_pop(L, 1);
}

void
game_init(int argc, char* argv[]) {
	G = (game_t*)malloc(sizeof(*G));
	G->game = dust_game();
	G->last_update = 0;
	G->last_draw = 0;
	G->reload_flag = 0;

	lua_State *L = G->game->L;

	// common libs
	reg_lua_lib(L, luaopen_dtex2_c, "dtex2.c");
	reg_lua_lib(L, luaopen_gtxt_c, "gtxt.c");
	reg_lua_lib(L, luaopen_sl_c, "shaderlab.c");
	reg_lua_lib(L, luaopen_c25_c, "camera25.c");
	reg_lua_lib(L, luaopen_stat_c, "stat.c");
	reg_lua_lib(L, luaopen_s2_c, "sprite2.c");
	reg_lua_lib(L, luaopen_gum_c, "gum.c");

	lua_pushcfunction(L, traceback);
	int tb = lua_gettop(L);

	int err;

	// {
	err = luaL_loadstring(L, startscript);
	if (err) {
		const char *msg = lua_tostring(L,-1);
		fault("%s", msg);
	}

	char modname[1024];
	int sz = GetModuleFileNameA(0,  modname, 1024);
	lua_pushlstring(L, modname, sz);

	int i;
	for (i=1;i<argc;i++) {
		lua_pushstring(L, argv[i]);
	}

	err = lua_pcall(L, argc, 0, tb);
	if (err) {
		const char *msg = lua_tostring(L,-1);
		fault("%s", msg);
	}

	lua_pop(L,1); // pop traceback function

	dust_game_logicframe(30);
	dust_game_start(G->game);

	game_resize(WIDTH, HEIGHT, 1);
}

static inline float
update_time(DWORD *slot) {
	DWORD now = GetTickCount();
	DWORD last = *slot;
	*slot = now;
	if (last != 0) {
		return (float)(now - last) * 0.001f;
	} else {
		return 0.01f;
	}
}

// todo
static void
_reload() {
	dust_game_exit(G->game);
	free(G);
//	game_init();
}

void 
game_update() {
	float timediff = update_time(&G->last_update);
	dust_game_update(G->game, timediff);
	if(G->reload_flag) {
		_reload();
	}
}

void 
game_draw() {
	float timediff = update_time(&G->last_draw);
	dust_game_drawframe(G->game, timediff);
}

void
game_resize(int width, int height, float scale) {
	char buf[128];
	dust_on_size(width * scale, height * scale);
	sprintf(buf, "%d;%d;%f", width, height, scale);
	dust_game_message(G->game, 0, "RESIZE", buf, 0);
}

void
game_scroll(int x, int y, int value) {
	char buf[128];
	sprintf(buf, "%d;%d;%d", x, y, value);
	dust_game_message(G->game, 0, "SCROLL", buf, 0);
}

void
game_key_messge(const char *state, char code) {
	char buf[2];
	buf[0] = code;
	buf[1] = 0;
	dust_game_message(G->game, 0, state, buf, code);
}

void
game_reload() {
	G->reload_flag = 1;
}

#ifndef dustgame_h
#define dustgame_h

#include <lua.h>

struct game {
	lua_State *L;
	float real_time;
	float logic_time;
	float hold_update_time;
};


struct game * dust_game();
lua_State * dust_lua_init(struct game *);
void dust_game_exit(struct game *);
void dust_close_lua(struct game *);
lua_State *  dust_game_lua(struct game *);
void dust_handle_error(lua_State *L, const char *err_type, const char *msg);
void dust_game_logicframe(int);
void dust_game_start(struct game *);
void dust_game_update(struct game *, float dt);
void dust_game_drawframe(struct game *, float dt);
int dust_game_touch(struct game *, int id, float x, float y, int status);
void dust_game_gesture(struct game *, int type,
                         double x1, double y1, double x2, double y2, int s);
void dust_game_message(struct game* G,int id_, const char* state, const char* data, lua_Number n);
void dust_game_pause(struct game* G);
void dust_game_resume(struct game* G);

int dust_game_handle_url(struct game *G, const char* url);

void dust_call_lua(lua_State *L, int n, int r);

void dust_on_size(int w, int h);

void dust_game_memory_report(struct game * G);
char *dust_game_memory_status(struct game * G);

#endif // dustgame_h

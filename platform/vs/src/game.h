#ifndef dust_game_h
#define dust_game_h

struct game;

struct game* game_instance();
void game_init(int argc, char* argv[]);

void game_update();
void game_draw();

void game_resize(int width, int height, float scale);

void game_scroll(int x, int y, int value);
void game_key_messge(const char* state, char code);
void game_reload();

#endif // dust_game_h
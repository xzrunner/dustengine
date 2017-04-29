#include "dust_statistics.h"

#include <c_wrap_sl.h>
#include <glp_loop.h>
#include <rvg.h>
#include <c_wrap_gum.h>
#include <logger.h>

#include <stdio.h>
#include <string.h>

#define SCREEN_SCALE 16

#define DC_SMOOTHING 0.99f
#define UPDATE_TIME (30 * 1)

#define MEM_INV (1.0f / 1024.0f / 1024.0f)

struct state {
	int fps;
	int drawcall;
	int vertices;
	float overdraw;

	float mem_tot;
	float mem_tex;
	float mem_lua;
	float mem_loader;
	float mem_other;

	int tick;

	bool enable;
};

static struct state S;

static void
_draw_text(const char* str, int x, int y, int w) {
	sl_set_shader(ST_SPRITE2);
	gum_draw_text(str, x, y, w);
}

static void
_draw_rect(int x, int y, int max, int mid, int curr, bool small_batter) {
	sl_set_shader(ST_SHAPE2);

	int red = 0x88444488, green = 0x8844aa44;
	int col = 0;
	if (small_batter) {
		col = curr <= mid ? green : red;
	} else {
		col = curr >= mid ? green : red;
	}
	sl_shape2_color(col);

	const int WIDTH = 400, HEIGHT = 15;
	float w = (float)curr / max * WIDTH;
	float xmin = x, xmax = x + w;
	float ymin = y, ymax = y + HEIGHT;
// 	screen_trans(&xmin, &ymin);
// 	screen_trans(&xmax, &ymax);
	xmin *= SCREEN_SCALE;
	xmax *= SCREEN_SCALE;
	ymin *= SCREEN_SCALE;
	ymax *= SCREEN_SCALE;

	rvg_rect(xmin, ymin, xmax, ymax, true);
}

void
stat_init() {
	memset(&S, 0, sizeof(S));
	S.enable = false;
}

void 
stat_enable(bool enable) {
	S.enable = enable;
}

void 
stat_update() {
	if (!S.enable) {
		return;
	}

	S.fps = glp_loop_get_fps();
	S.drawcall = sl_get_drawcall();
	S.vertices = sl_get_vertices_count();
	sl_reset_statistics();

	++S.tick;
	if (S.tick > UPDATE_TIME) {
		S.tick = 0;
//		S.mem_tex = dtex_stat_tex_mem() * MEM_INV;
// 		S.mem_loader = dtexf_load_buf_sz() * MEM_INV;
// 		if (S.mem_tot > 0) {
// 			S.mem_other = S.mem_tot - S.mem_tex - S.mem_lua - S.mem_loader;
// 		}
	}
}

static void
_draw_col_rect(int col, float xmin, float ymin, float xmax, float ymax) {
	sl_shape2_color(col);
// 	screen_trans(&xmin, &ymin);
// 	screen_trans(&xmax, &ymax);
	xmin *= SCREEN_SCALE;
	xmax *= SCREEN_SCALE;
	ymin *= SCREEN_SCALE;
	ymax *= SCREEN_SCALE;
	rvg_rect(xmin, ymin, xmax, ymax, true);
	sl_flush();
}

static void
_draw_mem(int x, int y) {
	if (S.mem_tot <= 0) {
		return;
	}
	
	sl_set_shader(ST_SHAPE2);

	int ctex = 0x8844aa44, clua = 0x88aa4444, cloader = 0x88aaaa44, cother = 0x88444488;
	
	const int WIDTH = 600, HEIGHT = 15;
	float wtex = S.mem_tex / S.mem_tot * WIDTH;
	float wlua = S.mem_lua / S.mem_tot * WIDTH;
	float wloader = S.mem_loader / S.mem_tot * WIDTH;

	float xmin = x, xmax = x + wtex;
	float ymin = y, ymax = y + HEIGHT;
	_draw_col_rect(ctex, xmin, ymin, xmax, ymax);

	xmin = xmax; xmax += wlua;
	_draw_col_rect(clua, xmin, ymin, xmax, ymax);

	xmin = xmax; xmax += wloader;
	_draw_col_rect(cloader, xmin, ymin, xmax, ymax);

	xmin = xmax; xmax = x + WIDTH;
	_draw_col_rect(cother, xmin, ymin, xmax, ymax);
}

static void
_print_with_text() {
	static int count = 0;
	++count;
	if (count == 60) {
		LOGI("FPS: %d", S.fps);
		count = 0;
	}
}

static void
_print_with_glyph() {
	char buf[512];

	sprintf(buf, "FPS: %d", S.fps);
	_draw_text(buf, 0, -100, 400);

	sprintf(buf, "DC: %d", S.drawcall);
	_draw_text(buf, 0, -50, 400);
	_draw_rect(200, 150, 100, 50, S.drawcall, true);

	_draw_text("MEM: ", 100, 0, 100);
	_draw_mem(200, 200);
	sprintf(buf, "TOT: %0.1f,  TEX: %0.1f,  LUA: %0.1f,  LOADER: %0.1f,  OTHER: %0.1f", 
		S.mem_tot, S.mem_tex, S.mem_lua, S.mem_loader, S.mem_other);
	_draw_text(buf, 0, 20, 600);

	sprintf(buf, "VERTICES: %d", S.vertices);
	_draw_text(buf, 0, 70, 400);

	sprintf(buf, "OVERDRAW: %0.1f", S.overdraw / 4.0f);
	_draw_text(buf, 0, 100, 400);
	S.overdraw = 0;

	sl_flush();

	sl_set_shader(ST_SPRITE2);
}

void 
stat_print() {
	if (!S.enable) {
		return;
	}

	_print_with_text();
//	_print_with_glyph();
}

void
stat_set_mem(float tot, float lua) {
	S.mem_tot = tot;
	S.mem_lua = lua;
}

void 
stat_add_overdraw_area(float area) {
	S.overdraw += area;
}
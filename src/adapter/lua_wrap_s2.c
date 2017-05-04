#include <sm_const.h>
#include <c_wrap_sl.h>
#include <c_wrap_s2.h>
#include <c_wrap_gum.h>

#include <lua.h>
#include <lauxlib.h>

#include <string.h>
#include <assert.h>

#define SRT_X 1
#define SRT_Y 2
#define SRT_SX 3
#define SRT_SY 4
#define SRT_ROT 5
#define SRT_SCALE 6

struct actor_proxy
{
	void* actor;
};

struct srt {
	float trans_x, trans_y;
	float rot;
	float scale_x, scale_y;
};

static int
lget_actor_count(lua_State* L) {
	int count = s2_actor_get_count();
	lua_pushinteger(L, count);
	return 1;
}

static double
_read_key(lua_State* L, int idx, int key, double def) {
	lua_pushvalue(L, lua_upvalueindex(key));
	lua_rawget(L, idx);
	double ret = luaL_optnumber(L, -1, def);
	lua_pop(L,1);
	return ret;
}

static double
_read_key2(lua_State* L, int idx, const char* key, double def) {
	lua_pushstring(L, key);
	lua_rawget(L, idx);
	double ret = luaL_optnumber(L, -1, def);
	lua_pop(L,1);
	return ret;
}

static void
_fill_srt(lua_State* L, struct srt* srt, int idx) {
	if (lua_isnoneornil(L, idx)) {
		srt->trans_x = 0;
		srt->trans_y = 0;
		srt->rot     = 0;
		srt->scale_x = 1;
		srt->scale_y = 1;
		return;
	}
	luaL_checktype(L,idx,LUA_TTABLE);
	double x     = _read_key(L, idx, SRT_X, 0);
	double y     = _read_key(L, idx, SRT_Y, 0);
	double scale = _read_key(L, idx, SRT_SCALE, 0);
	double sx;
	double sy;
	double rot = _read_key(L, idx, SRT_ROT, 0);
	if (scale > 0) {
		sx = sy = scale;
	} else {
		sx = _read_key(L, idx, SRT_SX, 1);
		sy = _read_key(L, idx, SRT_SY, 1);
	}
	srt->trans_x = x;
	srt->trans_y = y;
	srt->scale_x = sx;
	srt->scale_y = sy;
	srt->rot     = rot;
}

static void
_filling_region_from_cam(const void* cam, struct s2_region* r) {
	float x, y, scale;
	s2_cam_get(cam, &x, &y, &scale);

	int w, h;
	s2_get_screen_size(&w, &h);

	float hw = w * 0.5f * scale;
	float hh = h * 0.5f * scale;

	r->xmin = -hw + x;
	r->xmax =  hw + x;
	r->ymin = -hh + y;
	r->ymax =  hh + y;		
}

static void
_filling_region_from_lua(lua_State* L, struct s2_region* r, int idx) {
	if (lua_isnoneornil(L, idx)) {
		r->xmin = 0;
		r->ymin = 0;
		r->xmax = 1;
		r->ymax = 1;
		return;
	}
	luaL_checktype(L, idx, LUA_TTABLE);

	r->xmin = _read_key2(L, idx, "xmin", 0);
	r->ymin = _read_key2(L, idx, "ymin", 0);
	r->xmax = _read_key2(L, idx, "xmax", 0);
	r->ymax = _read_key2(L, idx, "ymax", 0);
}

static void*
_actor(lua_State* L) {
	struct actor_proxy* proxy = (struct actor_proxy*)lua_touserdata(L, 1);
	if (!proxy) {
		luaL_error(L, "Error passed actor");
	}
	if (!proxy->actor) {
		luaL_error(L, "Error wrapper actor");
	}
	return proxy->actor;
}

static void*
_spr(lua_State* L) {
	void* actor = _actor(L);
	void* spr = s2_actor_get_spr(actor);
	if (!spr) {
		luaL_error(L, "Error wrapper spr");
	}
	return spr;
}

static void
_return_proxy(lua_State* L, void* actor) {
	lua_getfield(L, LUA_REGISTRYINDEX, "s2_objects");
	assert(lua_istable(L, -1));

	lua_pushlightuserdata(L, actor);
	lua_gettable(L, -2);
	if (lua_type(L, -1) != LUA_TUSERDATA) {
		lua_pop(L, 1);

		void* spr = s2_actor_get_spr(actor);
		s2_spr_retain(spr);
		struct actor_proxy* proxy = (struct actor_proxy*)lua_newuserdata(L, sizeof(*proxy));
		proxy->actor = actor;

		lua_pushlightuserdata(L, proxy->actor);
		lua_pushvalue(L, -2);
		lua_settable(L, -4);
	}
	lua_remove(L, -2);
}

static int
lfetch(lua_State* L) {
	void* actor = _actor(L);
	const char* name = luaL_checkstring(L, 2);
	void* child = s2_actor_fetch_child(actor, name);
	if (child) {
		void* ret = s2_actor_create(actor, child);
		ret = s2_actor_get_anchor_real(ret);
		_return_proxy(L, ret);
		return 1;
	} else {
		return 0;
	}
}

static int
lfetch_by_index(lua_State* L) {
	void* actor = _actor(L);
	int index = (int)luaL_checkinteger(L, 2);
	void* child = s2_actor_fetch_child_by_index(actor, index);
	if (child) {
		void* ret = s2_actor_create(actor, child);
		ret = s2_actor_get_anchor_real(ret);
		_return_proxy(L, ret);
		return 1;
	} else {
		return 0;
	}
}

static int
lmount(lua_State* L) {
	void* actor = _actor(L);
	const char* name = luaL_checkstring(L, 2);
	if (!lua_toboolean(L, 3)) {
		s2_actor_mount(actor, name, NULL);
		lua_pushinteger(L, 0);
	} else {
		void* ud = lua_touserdata(L, 3);
		if (!ud) {
			luaL_error(L, "Error passed actor");
		}
		void* anchor_actor = ((struct actor_proxy*)ud)->actor;
		int ret = s2_actor_mount(actor, name, anchor_actor);
		lua_pushinteger(L, ret);
	}
	return 1;
}

static int
lget_parent(lua_State* L) {
	void* actor = _actor(L);
	void* parent = s2_actor_get_parent(actor);
	if (!parent) {
		return 0;
	} else {
		_return_proxy(L, parent);
		return 1;
	}
}

static int
ldtex_c2_enable(lua_State* L) {
	void* spr = _spr(L);
	bool enable = lua_toboolean(L, 2);
	s2_spr_set_dtex_enable(spr, enable);
	return 0;
}

static int
ldtex_c2_force_cached(lua_State* L) {
	void* spr = _spr(L);
	bool cache = lua_toboolean(L, 2);
	s2_spr_set_dtex_force_cached(spr, cache);
	return 0;
}

static int
ldtex_c2_force_cached_set_dirty(lua_State* L) {
	void* spr = _spr(L);
	bool dirty = lua_toboolean(L, 2);
	s2_spr_set_dtex_force_cached_dirty(spr, dirty);
	return 0;
}

static int
lbuild_flatten(lua_State* L) {
	void* actor = _actor(L);
	s2_actor_build_flatten(actor);
	return 0;
}

static int
lps(lua_State* L) {
	void* actor = _actor(L);
	int n = lua_gettop(L);
	switch (n) {
	case 4:
		{
			// x, y, scale
			float x = luaL_checknumber(L, 2),
				  y = luaL_checknumber(L, 3);
			y = -y;
			float s = luaL_checknumber(L, 4);
			s2_actor_set_pos(actor, x, y);
			s2_actor_set_scale(actor, s, s);
		}
		break;
	case 3:
		{
			// x, y
			float x = luaL_checknumber(L, 2),
				  y = luaL_checknumber(L, 3);
			y = -y;
			s2_actor_set_pos(actor, x, y);
		}
		break;
	case 2:
		{
			// scale
			float s = luaL_checknumber(L, 2);
			s2_actor_set_scale(actor, s, s);
		}
		break;
	default:
		return luaL_error(L, "Invalid params: ps");
	}
	return 0;
}

static int
lsr(lua_State* L) {
	void* actor = _actor(L);
	int n = lua_gettop(L);
	switch (n) {
	case 4:
		{
			// sx, sy, rot
			float sx = luaL_checknumber(L, 2),
				  sy = luaL_checknumber(L, 3);
			float rot = luaL_checknumber(L, 4);
			s2_actor_set_scale(actor, sx, sy);
			s2_actor_set_angle(actor, -rot * SM_DEG_TO_RAD);
		}
		break;
	case 3:
		{
			// sx, sy
			float sx = luaL_checknumber(L, 2),
				  sy = luaL_checknumber(L, 3);
			s2_actor_set_scale(actor, sx, sy);
		}
		break;
	case 2:
		{
			// rot
			float rot = luaL_checknumber(L, 2);
			s2_actor_set_angle(actor, -rot * SM_DEG_TO_RAD);
		}
		break;
	default:
		return luaL_error(L, "Invalid params: sr");
	}
	return 0;
}

/*
	userdata sprite
	table { .x .y .sx .sy .rot }
 */
static int
ldraw(lua_State* L) {
	void* actor = _actor(L);

	struct srt srt;
	_fill_srt(L, &srt, 2);

	struct s2_region r;
	const void* cam = lua_touserdata(L, 3);
	_filling_region_from_cam(cam, &r);

	s2_spr_draw(actor, srt.trans_x, srt.trans_y, srt.rot, srt.scale_x, srt.scale_y,
		r.xmin, r.ymin, r.xmax, r.ymax);

	return 0;
}

/*
	userdata sprite
	table { .x .y .sx .sy .rot }
 */
static int
ldraw_symbol(lua_State* L) {
	void* actor = _actor(L);

	struct srt srt;
	_fill_srt(L, &srt, 2);

	struct s2_region r;
	const void* cam = lua_touserdata(L, 3);
	_filling_region_from_cam(cam, &r);

	s2_symbol_draw(actor, srt.trans_x, srt.trans_y, srt.rot, srt.scale_x, srt.scale_y,
		r.xmin, r.ymin, r.xmax, r.ymax);

	return 0;
}

static int
ldraw_symbol2(lua_State* L) {
	const void* sym = lua_touserdata(L, 1);
	float x = lua_tonumber(L, 2);
	float y = lua_tonumber(L, 3);
	s2_symbol_draw2(sym, x, y);
	return 0;
}

/*
	userdata sprite
	table { .x .y .sx .sy .rot }
 */
static int
ldraw_actor(lua_State* L) {
	void* actor = _actor(L);

	struct srt srt;
	_fill_srt(L, &srt, 2);

	struct s2_region r;
	const void* cam = lua_touserdata(L, 3);
	_filling_region_from_cam(cam, &r);

	s2_actor_draw(actor, srt.trans_x, srt.trans_y, srt.rot, srt.scale_x, srt.scale_y,
		r.xmin, r.ymin, r.xmax, r.ymax);

	return 0;
}

static int
ldraw_aabb(lua_State* L) {
	void* spr = _spr(L);

	float mat[6];
	luaL_checktype(L, 2, LUA_TTABLE);
	if (lua_rawlen(L, 2) != 6) {
		return luaL_error(L, "error mat");
	}
	for (int i = 0; i < 6; ++i) {
		lua_rawgeti(L, 2, i + 1);
		mat[i] = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}

	struct srt srt;
	_fill_srt(L, &srt, 3);

	s2_spr_draw_aabb(spr, srt.trans_x, srt.trans_y, srt.rot, srt.scale_x, srt.scale_y, mat);

	return 0;
}

static int
lupdate(lua_State* L) {
	void* actor = _actor(L);
	s2_actor_update(actor, false);
	return 0;
}

static int
lstart(lua_State* L) {
	void* actor = _actor(L);
	s2_actor_msg_start(actor);
	return 0;
}

static int
lstop(lua_State* L) {
	void* actor = _actor(L);
	s2_actor_msg_stop(actor);
	return 0;
}

static int
lclear(lua_State* L) {
	void* actor = _actor(L);
	s2_actor_msg_clear(actor);
	return 0;
}

static int
lpoint_query(lua_State* L) {
	void* actor = _actor(L);
	float x = luaL_checknumber(L, 2),
		  y = luaL_checknumber(L, 3);
	float mat[6];
	void* ret = s2_point_query_actor(actor, x, -y, mat);
	if (!ret) {
		return 0;
	}

	_return_proxy(L, ret);

	lua_newtable(L);
	for (int i = 0; i < 6; ++i) {
		lua_pushnumber(L, mat[i]);
		lua_rawseti(L, -2, i + 1);
	}

	return 2;
}

static int
lpoint_test(lua_State* L) {
	void* spr = _spr(L);
	float x = luaL_checknumber(L, 2),
		  y = luaL_checknumber(L, 3);
	bool contain = s2_spr_point_test(spr, x, -y);
	lua_pushboolean(L, contain);
	return 1;
}

static int
lhas_action(lua_State* L) {
	void* spr = _spr(L);
	const char* name = lua_tostring(L, 2);
	bool has = s2_spr_has_action(spr, name);
	lua_pushboolean(L, has);
	return 1;
}

static int
laabb(lua_State* L) {
	void* actor = _actor(L);
	float aabb[4];
	s2_actor_get_aabb(actor, aabb);
	for (int i = 0; i < 4; ++i) {
		lua_pushnumber(L, aabb[i]);
	}
	return 4;
}

static int
ltext_size(lua_State* L) {
	void* actor = _actor(L);
	float w, h;
	if (s2_actor_get_text_size(actor, &w, &h)) {
		lua_pushnumber(L, w);
		lua_pushnumber(L, h);
		return 2;
	} else {
		return 0;
	}
}

static int
lgetwpos(lua_State* L) {
	void* actor = _actor(L);
	float x, y;
	s2_actor_get_world_pos(actor, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, -y);
	return 2;
}

static int
lgetwangle(lua_State* L) {
	void* actor = _actor(L);
	float angle = s2_actor_get_world_angle(actor);
	lua_pushnumber(L, angle);
	return 1;
}

static int
lgetwscale(lua_State* L) {
	void* actor = _actor(L);
	float sx, sy;
	s2_actor_get_world_scale(actor, &sx, &sy);
	lua_pushnumber(L, sx);
	lua_pushnumber(L, sy);
	return 2;
}

static int
lgetpannel(lua_State* L) {
	void* spr = _spr(L);
	float xmin, ymin, xmax, ymax;
	s2_spr_get_scissor(spr, &xmin, &ymin, &xmax, &ymax);
	lua_pushnumber(L, xmax - xmin);
	lua_pushnumber(L, ymax - ymin);
	return 2;
}

static int
lsetpannel(lua_State* L) {
	void* spr = _spr(L);
	
	float w = luaL_optnumber(L, 2, 0);
	float h = luaL_optnumber(L, 3, 0);
	float xmin, ymin, xmax, ymax;
	bool ok = s2_spr_get_scissor(spr, &xmin, &ymin, &xmax, &ymax);
	if(ok) {
		float offset_x = w-(xmax-xmin);
		float offset_y = h-(ymax-ymin);
		s2_spr_set_scissor(spr, xmin-offset_x, ymin-offset_y, xmax, ymax);
	}
	return 0;
}

static int
lget_pos(lua_State* L) {
	void* actor = _actor(L);
	float x, y;
	s2_actor_get_pos(actor, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, -y);
	return 2;
}

static int
lset_pos(lua_State* L) {
	void* actor = _actor(L);
	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);
	s2_actor_set_pos(actor, x, -y);
	return 0;
}

static int
lget_angle(lua_State* L) {
	void* actor = _actor(L);
	float angle = s2_actor_get_angle(actor);
	lua_pushnumber(L, angle);
	return 1;
}

static int
lset_angle(lua_State* L) {
	void* actor = _actor(L);
	float angle = luaL_optnumber(L, 2, 0);
	s2_actor_set_angle(actor, angle);
	return 0;
}

static int
lget_scale(lua_State* L) {
	void* actor = _actor(L);
	float sx, sy;
	s2_actor_get_scale(actor, &sx, &sy);
	lua_pushnumber(L, sx);
	lua_pushnumber(L, sy);
	return 2;
}

static int
lset_scale(lua_State* L) {
	void* actor = _actor(L);
	float sx = luaL_optnumber(L, 2, 1);
	float sy = luaL_optnumber(L, 3, 1);
	s2_actor_set_scale(actor, sx, sy);
	return 0;
}

static int
lget_spr_pos(lua_State* L) {
	void* spr = _spr(L);
	float x, y;
	s2_spr_get_pos(spr, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, -y);
	return 2;
}

static int
lset_spr_pos(lua_State* L) {
	void* spr = _spr(L);
	float x = luaL_optnumber(L, 2, 0);
	float y = luaL_optnumber(L, 3, 0);
	s2_spr_set_pos(spr, x, -y);
	return 0;
}

static int
lget_spr_angle(lua_State* L) {
	void* spr = _spr(L);
	float angle = s2_spr_get_angle(spr);
	lua_pushnumber(L, angle);
	return 1;
}

static int
lset_spr_angle(lua_State* L) {
	void* spr = _spr(L);
	float angle = luaL_optnumber(L, 2, 0);
	s2_spr_set_angle(spr, angle);
	return 0;
}

static int
lget_spr_scale(lua_State* L) {
	void* spr = _spr(L);
	float sx, sy;
	s2_spr_get_scale(spr, &sx, &sy);
	lua_pushnumber(L, sx);
	lua_pushnumber(L, sy);
	return 2;
}

static int
lset_spr_scale(lua_State* L) {
	void* spr = _spr(L);
	float sx = luaL_optnumber(L, 2, 1);
	float sy = luaL_optnumber(L, 3, 1);
	s2_spr_set_scale(spr, sx, sy);
	return 0;
}

static int
lscale9_resize(lua_State* L) {
	void* actor = _actor(L);
	float w = luaL_optnumber(L, 2, 0),
		  h = luaL_optnumber(L, 3, 0);
	s2_actor_scale9_resize(actor, w, h);
	return 0;
}

static int
ltextbox_reset_time(lua_State* L) {
	void* spr = _spr(L);
	s2_spr_textbox_reset_time(spr);
	return 0;
}

static int
lanim2_set_static_time(lua_State* L) {
	void* spr = _spr(L);
	int time = lua_tointeger(L, 2);
	s2_spr_anim2_set_static_time(spr, time);
	return 0;
}

static int
lp3d_set_local(lua_State* L) {
	void* spr = _spr(L);
	bool local = lua_toboolean(L, 2);
	s2_spr_p3d_set_local(spr, local);
	return 0;
}

static int
lp3d_set_loop(lua_State* L) {
	void* spr = _spr(L);
	bool loop = lua_toboolean(L, 2);
	s2_spr_p3d_set_loop(spr, loop);
	return 0;
}

static int
lp3d_is_finished(lua_State* L) {
	void* spr = _spr(L);
	bool finish = s2_spr_p3d_is_finished(spr);
	lua_pushboolean(L, finish);
	return 1;
}

static int
lp3d_update(lua_State* L) {
	void* spr = _spr(L);
	float dt = luaL_optnumber(L, 2, 0);
	s2_spr_p3d_update(spr, dt);
	return 0;
}

static int
lset_dtex_strategy(lua_State* L) {
	return 0;
}

static const char* spr_key[] = {
	"x",
	"y",
	"sx",
	"sy",
	"rot",
	"scale",

	"xmin",
	"ymin",
	"xmax",
	"ymax",
};

static void
lmethod(lua_State* L) {
	luaL_Reg l[] = {
		{ "fetch", lfetch },
		{ "fetch_by_index", lfetch_by_index },
		{ "mount", lmount },
		{ "get_parent", lget_parent },

 		{ "dtex_c2_enable", ldtex_c2_enable },
 		{ "dtex_c2_force_cached", ldtex_c2_force_cached },
 		{ "dtex_c2_force_cached_set_dirty", ldtex_c2_force_cached_set_dirty },

		{ "build_flatten", lbuild_flatten },

		{ NULL, NULL },
	};
	luaL_newlib(L,l);

	int nk = sizeof(spr_key) / sizeof(spr_key[0]);
	for (int i = 0; i < nk; ++i) {
		lua_pushstring(L, spr_key[i]);
	}

	luaL_Reg l2[] = {
		{ "ps", lps },
		{ "sr", lsr },
 		{ "draw", ldraw },
		{ "draw_symbol", ldraw_symbol },
		{ "draw_symbol2", ldraw_symbol2 },
		{ "draw_actor", ldraw_actor },
		{ "draw_aabb", ldraw_aabb },
		{ "update", lupdate },
		{ "start", lstart },
		{ "stop", lstop },
		{ "clear", lclear },
		{ "point_query", lpoint_query },
		{ "point_test", lpoint_test },
		{ "has_action", lhas_action },
		{ "aabb", laabb },
		{ "text_size", ltext_size },
		{ "world_pos", lgetwpos },
		{ "world_angle", lgetwangle },
		{ "world_scale", lgetwscale },
		{ "get_pannel", lgetpannel },
		{ "set_pannel", lsetpannel },

		// actor
		{ "get_pos", lget_pos },
		{ "set_pos", lset_pos },
		{ "get_angle", lget_angle },
		{ "set_angle", lset_angle },
		{ "get_scale", lget_scale },
		{ "set_scale", lset_scale },

		// sprite
		{ "get_spr_pos", lget_spr_pos },
		{ "set_spr_pos", lset_spr_pos },
		{ "get_spr_angle", lget_spr_angle },
		{ "set_spr_angle", lset_spr_angle },
		{ "get_spr_scale", lget_spr_scale },
		{ "set_spr_scale", lset_spr_scale },

		{ "scale9_resize", lscale9_resize },
		{ "textbox_reset_time", ltextbox_reset_time },
		{ "anim2_set_static_time", lanim2_set_static_time },
		{ "p3d_set_local", lp3d_set_local },
		{ "p3d_set_loop", lp3d_set_loop },
		{ "p3d_is_finished", lp3d_is_finished },
		{ "p3d_update", lp3d_update },
// 		{ "reinit", lreinit },
// 		{ "release", lrelease },
		{ "set_dtex_strategy", lset_dtex_strategy },
		{ NULL, NULL, },
	};
	luaL_setfuncs(L, l2, nk);
}

static int
lgetframe(lua_State* L) {
	void* actor = _actor(L);
	lua_pushinteger(L, s2_actor_get_frame(actor));
	return 1;
}

static int
lgettotalframe(lua_State* L) {
	void* spr = _spr(L);
	lua_pushinteger(L, s2_spr_get_frame_count(spr));
	return 1;
}

static int
lgettotalcomponent(lua_State* L) {
	void* actor = _actor(L);
	lua_pushinteger(L, s2_actor_get_component_count(actor));
	return 1;
}

static int
lgetvisible(lua_State* L) {
	void* actor = _actor(L);
	bool visible = s2_actor_get_visible(actor);
	lua_pushboolean(L, visible);
	return 1;
}

static int
lgetmessage(lua_State* L) {
	void* actor = _actor(L);
	bool editable = s2_actor_get_editable(actor);
	lua_pushboolean(L, editable);
	return 1;
}

static int
lgetid(lua_State* L) {
	void* spr = _spr(L);
	lua_pushinteger(L, s2_spr_get_sym_id(spr));
	return 1;
}

static int
lgetid2(lua_State* L) {
	void* spr = _spr(L);
	lua_pushinteger(L, s2_spr_get_id(spr));
	return 1;
}

static int
lgetname(lua_State* L) {
	void* spr = _spr(L);
	const char* name = s2_spr_get_name(spr);
	if (!name) {
		return 0;
	}
	lua_pushstring(L, name);
	return 1;
}

static int
lgettype(lua_State* L) {
	void* spr = _spr(L);
	lua_pushinteger(L, s2_spr_get_sym_type(spr));
	return 1;
}

static int
lgettext(lua_State* L) {
	void* actor = _actor(L);
	const char* text = s2_actor_get_text(actor);
	lua_pushstring(L, text);
	return 1;
}

static int
lgetcolor(lua_State* L) {
	void* actor = _actor(L);
	uint32_t mul = s2_actor_get_col_mul(actor);
	lua_pushinteger(L, s2_trans_color(mul, ABGR, ARGB));
	return 1;
}

static int
lgetalpha(lua_State* L) {
	void* actor = _actor(L);
	uint32_t mul = s2_actor_get_col_mul(actor);
	lua_pushinteger(L, (mul >> 24) & 0xff);
	return 1;
}

static int
lgetadditive(lua_State* L) {
	void* actor = _actor(L);
	uint32_t add = s2_actor_get_col_add(actor);
	lua_pushinteger(L, s2_trans_color(add, ABGR, ARGB));
	return 1;
}

static int
lgetcolormap(lua_State* L) {
	void* actor = _actor(L);
	uint32_t rmap, gmap, bmap;
	s2_actor_get_col_map(actor, &rmap, &gmap, &bmap);
	lua_pushinteger(L, s2_trans_color(rmap, ABGR, ARGB));
	lua_pushinteger(L, s2_trans_color(gmap, ABGR, ARGB));
	lua_pushinteger(L, s2_trans_color(bmap, ABGR, ARGB));
	return 3;
}

static int
lget_force_inherit_frame(lua_State* L) {
	void* actor = _actor(L);
	bool force = s2_actor_get_force_update(actor);
	lua_pushboolean(L, force);
	return 1;
}

static int
lget_random_child_base_frame(lua_State* L) {
	lua_pushboolean(L, false);
	return 1;
}

static int
lgetmat(lua_State* L) {
	return 0;
}

static int
lgetwmat(lua_State* L) {
	return 0;
}

static int
lgetparentname(lua_State* L) {
	return 0;
}

static void
lgetter(lua_State* L) {
 	luaL_Reg l[] = {
		{"frame", lgetframe},
		{"frame_count", lgettotalframe },
		{"component_count", lgettotalcomponent },
		{"visible", lgetvisible },
		{"message", lgetmessage },
		{"id", lgetid },
		{"id2", lgetid2 },
		{"name", lgetname },
		{"type", lgettype },
		{"text", lgettext},
		{"color", lgetcolor },
		{"alpha", lgetalpha },
		{"additive", lgetadditive },
		{"colormap", lgetcolormap },
		{"force_inherit_frame", lget_force_inherit_frame },
		{"random_child_base_frame", lget_random_child_base_frame },
		{"matrix", lgetmat },
		{"world_matrix", lgetwmat },
		{"parent_name", lgetparentname },
		{NULL, NULL},
 	};
 	luaL_newlib(L, l);
}

static int
lsetframe(lua_State* L) {
	void* actor = _actor(L);
	int frame = (int)luaL_checkinteger(L, 2);
	s2_actor_set_frame(actor, frame);
	return 0;
}

static int
lsetaction(lua_State* L) {
	void* actor = _actor(L);
	const char* action = lua_tostring(L, 2);
	s2_spr_set_action(actor, action);
	return 0;
}

static int
lsetvisible(lua_State* L) {
	void* actor = _actor(L);
	bool visible = lua_toboolean(L, 2);
	s2_actor_set_visible(actor, visible);
	return 0;
}

static int
lsetmessage(lua_State* L) {
	void* actor = _actor(L);
	bool editable = lua_toboolean(L, 2);
	s2_actor_set_editable(actor, editable);
	return 0;
}

static int
lsetmat(lua_State* L) {
	return 0;
}

static int
lsettext(lua_State* L) {
	void* actor = _actor(L);
	const char* text = NULL;
	if (lua_isstring(L, 2)) {
		text = lua_tostring(L, 2);
		s2_actor_set_text(actor, text);
	} else {
		s2_actor_set_text(actor, "");
	}
	return 0;
}

static int
lsetcolor(lua_State* L) {
	void* actor = _actor(L);
	uint32_t color = luaL_checkinteger(L, 2);
	s2_actor_set_col_mul(actor, s2_trans_color(color, ARGB, ABGR));
	return 0;
}

static int
lsetalpha(lua_State* L) {
	void* actor = _actor(L);
	uint8_t alpha = luaL_checkinteger(L, 2);

	uint32_t abgr = s2_actor_get_col_mul(actor);
	if (((abgr >> 24) & 0xff) == alpha) {
		return 0;
	}

	abgr = (alpha << 24) | (abgr & 0x00ffffff);
	s2_actor_set_col_mul(actor, abgr);

	return 0;
}

static int
lsetadditive(lua_State* L) {
	void* actor = _actor(L);
	uint32_t additive = luaL_checkinteger(L,2);
	additive &= 0x00ffffff;
	s2_actor_set_col_add(actor, s2_trans_color(additive, ARGB, ABGR));
	return 0;
}

static int
lsetcolormap(lua_State* L) {
	void* actor = _actor(L);
	uint32_t rmap = luaL_optinteger(L, 2, 0xffff0000);
	uint32_t gmap = luaL_optinteger(L, 3, 0xff00ff00);
	uint32_t bmap = luaL_optinteger(L, 4, 0xff0000ff);
	s2_actor_set_col_map(actor, 
		s2_trans_color(rmap, ARGB, ABGR), 
		s2_trans_color(gmap, ARGB, ABGR), 
		s2_trans_color(bmap, ARGB, ABGR));
	return 0;
}

static int
lfilter(lua_State* L) {
	void* actor = _actor(L);
	const char* filter = lua_tostring(L, 2);
	if (!filter) {
		s2_actor_set_filter(actor, SLFM_NULL);
	} else if (strcmp(filter, "NORMAL") == 0) {
		s2_actor_set_filter(actor, SLFM_NULL);
	} else if (strcmp(filter, "GRAY") == 0) {
		s2_actor_set_filter(actor, SLFM_GRAY);
	} else if (strcmp(filter, "COLOR_GRADING") == 0) {
		s2_actor_set_filter(actor, SLFM_COL_GRADING);
	}
	return 0;
}

static int
lset_force_inherit_frame(lua_State* L) {
	void* actor = _actor(L);
	bool force = lua_toboolean(L, 2);
	s2_actor_set_force_update(actor, force);
	return 0;
}

static int
lset_random_child_base_frame(lua_State* L) {
	return 0;
}

static int
lsetscissor(lua_State* L) {
	return 0;
}

static void
lsetter(lua_State* L) {
	luaL_Reg l[] = {
		{"frame", lsetframe},
		{"action", lsetaction},
		{"visible", lsetvisible},
		{"message", lsetmessage },
		{"matrix" , lsetmat},
		{"text", lsettext},
		{"color", lsetcolor},
		{"alpha", lsetalpha},
		{"additive", lsetadditive },
		{"colormap", lsetcolormap},
		{"force_inherit_frame", lset_force_inherit_frame },
		{"random_child_base_frame", lset_random_child_base_frame },
		{"filter", lfilter},
		{"scissor", lsetscissor },
		{NULL, NULL},
	};
	luaL_newlib(L, l);
}

static struct actor_proxy*
_create_proxy(lua_State* L, void* spr) {
	lua_getfield(L, LUA_REGISTRYINDEX, "s2_objects");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		lua_newtable(L);

		lua_newtable(L);
		lua_pushliteral(L, "v");
		lua_setfield(L, -2, "__mode");
		lua_setmetatable(L, -2);

		lua_setfield(L, LUA_REGISTRYINDEX, "s2_objects");
	}
	lua_getfield(L, LUA_REGISTRYINDEX, "s2_objects");
	assert(lua_istable(L, -1));

	struct actor_proxy* proxy = (struct actor_proxy*)lua_newuserdata(L, sizeof(*proxy));
	proxy->actor = s2_actor_create(NULL, spr);

	lua_pushlightuserdata(L, proxy->actor);
	lua_pushvalue(L, -2);
	lua_settable(L, -4);

	lua_remove(L, -2);

	return proxy;
}

static int
lnew_sym_model(lua_State* L) {
	const char* surface = lua_touserdata(L, 1);
	void* sym = gum_create_sym_model(surface);
	lua_pushlightuserdata(L, sym);
	return 1;
}

/*
	userdata sprite_pack
	integer id

	ret: userdata sprite
 */
static int
lnew_spr(lua_State* L) {
	const char* pkg_str = lua_tostring(L, 1);
	const char* spr_str = lua_tostring(L, 2);
	void* spr = gum_create_spr(pkg_str, spr_str);
	if (!spr) {
//		luaL_error(L, "fail to create spr: pkg %s, spr %s\n", pkg_str, spr_str);
		return 0;
	}
	_create_proxy(L, spr);
	return 1;
}

static int
lfetch_spr_cached(lua_State* L) {
	const char* pkg_str = lua_tostring(L, 1);
	const char* spr_str = lua_tostring(L, 2);
	bool is_new = false;
	void* actor = gum_fetch_actor_cached(pkg_str, spr_str, &is_new);
	if (is_new) 
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "s2_objects");
		if (!lua_istable(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);

			lua_newtable(L);
			lua_pushliteral(L, "v");
			lua_setfield(L, -2, "__mode");
			lua_setmetatable(L, -2);

			lua_setfield(L, LUA_REGISTRYINDEX, "s2_objects");
		}
		lua_getfield(L, LUA_REGISTRYINDEX, "s2_objects");
		assert(lua_istable(L, -1));

		struct actor_proxy* proxy = (struct actor_proxy*)lua_newuserdata(L, sizeof(*proxy));
		proxy->actor = actor;

		lua_pushlightuserdata(L, proxy->actor);
		lua_pushvalue(L, -2);
		lua_settable(L, -4);

		lua_remove(L, -2);
	} 
	else 
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "s2_objects");
		assert(lua_istable(L, -1));

		lua_pushlightuserdata(L, actor);
		lua_gettable(L, -2);
		if (lua_type(L, -1) != LUA_TUSERDATA) {
			lua_pop(L, 1);

			struct actor_proxy* proxy = (struct actor_proxy*)lua_newuserdata(L, sizeof(*proxy));
			proxy->actor = actor;

			lua_pushlightuserdata(L, proxy->actor);
			lua_pushvalue(L, -2);
			lua_settable(L, -4);
		}
		lua_remove(L, -2);
	}
	return 1;
}

static int
lreturn_spr_cached(lua_State* L) {
	void* actor = _actor(L);
	gum_return_actor_cached(actor);
	return 0;
}

static int
lnew_spr_by_id(lua_State* L) {
	int id = lua_tointeger(L, 1);
	void* spr = gum_create_spr_by_id(id);
	if (!spr) {
		luaL_error(L, "fail to create spr: id %d\n", id);
	}
	_create_proxy(L, spr);
	return 1;
}

static int
lnew_spr_from_file(lua_State* L) {
	const char* filepath = lua_tostring(L, 1);
	void* spr = gum_create_spr_from_file(filepath);
	if (!spr) {
		luaL_error(L, "fail to create spr: file %s\n", filepath);
	}
	_create_proxy(L, spr);
	return 1;
}

static int
lrelease_spr(lua_State* L) {
	void* actor = _actor(L);
	void* spr = s2_actor_get_spr(actor);
	if (!spr) {
		luaL_error(L, "Error wrapper spr");
	}
	s2_spr_release(spr);

	return 0;
}

static int
lrt_fetch(lua_State* L) {
	void* rt = s2_rt_fetch();
	if (!rt) {
		luaL_error(L, "s2 rt empty!");
	}
	lua_pushlightuserdata(L, rt);
	return 1;
}

static int
lrt_return(lua_State* L) {
	void* rt = lua_touserdata(L, 1);
	if (!rt) {
		luaL_error(L, "Error passed rt");
	}
	s2_rt_return(rt);
	return 0;
}

static int
lrt_draw_from(lua_State* L) {
	void* rt = lua_touserdata(L, 1);
	if (!rt) {
		luaL_error(L, "Error passed rt");
	}

	struct s2_region src, dst;
	_filling_region_from_lua(L, &src, 2);
	_filling_region_from_lua(L, &dst, 3);

	int src_tex_id = lua_tointeger(L, 4);

	s2_rt_draw_from(rt, &dst, &src, src_tex_id);

	return 0;
}

static int
lrt_draw_to(lua_State* L) {
	void* rt = lua_touserdata(L, 1);
	if (!rt) {
		luaL_error(L, "Error passed rt");
	}

	struct s2_region src, dst;
	_filling_region_from_lua(L, &src, 2);
	_filling_region_from_lua(L, &dst, 3);

	s2_rt_draw_to(rt, &dst, &src);

	return 0;
}

static int
lrt_get_texid(lua_State* L) {
	void* rt = lua_touserdata(L, 1);
	if (!rt) {
		luaL_error(L, "Error passed rt");
	}
	lua_pushinteger(L, s2_rt_get_texid(rt));
	return 1;
}

static int
lcam_create(lua_State* L) {
	void* cam = s2_cam_create();
	lua_pushlightuserdata(L, cam);
	return 1;
}

static int
lcam_release(lua_State* L) {
	void* cam = lua_touserdata(L, 1);
	if (!cam) {
		luaL_error(L, "Error passed cam");
	}
	s2_cam_release(cam);
	return 0;
}

static int
lcam_bind(lua_State* L) {
	void* cam = lua_touserdata(L, 1);
	if (!cam) {
		luaL_error(L, "Error passed cam");
	}
	s2_cam_bind(cam);
	return 0;
}

static int
lcam_set(lua_State* L) {
	void* cam = lua_touserdata(L, 1);
	if (!cam) {
		luaL_error(L, "Error passed cam");
	}
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float scale = luaL_checknumber(L, 4);
	s2_cam_set(cam, x, y, scale);
	return 0;
}

static int
lcam_screen2project(lua_State* L) {
	void* cam = lua_touserdata(L, 1);
	if (!cam) {
		luaL_error(L, "Error passed cam");
	}
	int src_x = luaL_checkinteger(L, 2);
	int src_y = luaL_checkinteger(L, 3);
	float dst_x, dst_y;
	s2_cam_screen2project(cam, src_x, src_y, &dst_x, &dst_y);
	return 2;
}

static int
lp3d_buffer_draw(lua_State* L) {
	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 1, 0);
	float scale = luaL_optnumber(L, 1, 1);
	s2_spr_p3d_buffer_draw(x, y, scale);
	return 0;
}

static int
lrvg_set_color(lua_State* L) {
	uint8_t r, g, b, a;
	if (lua_istable(L, 1)) {
		for (int i = 1; i <= 4; i++) {
			lua_rawgeti(L, 1, i);
		}
		r = (uint8_t)luaL_checkinteger(L, -4);
		g = (uint8_t)luaL_checkinteger(L, -3);
		b = (uint8_t)luaL_checkinteger(L, -2);
		a = (uint8_t)luaL_optinteger(L, -1, 255); 
		lua_pop(L, 4);
	} else {
		r = (uint8_t)luaL_checkinteger(L, 1);
		g = (uint8_t)luaL_checkinteger(L, 2);
		b = (uint8_t)luaL_checkinteger(L, 3);
		a = (uint8_t)luaL_optinteger(L, 4, 255);
	}
	s2_rvg_set_color(r, g, b, a);
	return 0;
}

static int
lrvg_set_line_width(lua_State* L) {
	float width = luaL_checknumber(L, -1);
	s2_rvg_set_line_width(width);
	return 0;
}

static int
lrvg_draw_line(lua_State *L) {
	int num = lua_gettop(L);
	bool is_table = false;
	if (num == 1 &&	lua_istable(L, 1)) {
		num = lua_rawlen(L, 1);
		is_table = true;
	} 

	float coords[num];
	if (is_table) {
		for (int i = 0; i < num; ++i) {
			lua_rawgeti(L, 1, i + 1);
			coords[i] = luaL_checknumber(L, -1);
			lua_pop(L, 1);
		}
	} else {
		for (int i = 0; i < num; ++i) {
			coords[i] = luaL_checknumber(L, i+1);
		}
	}

	s2_rvg_draw_line(coords, num / 2);

	return 0;
}

static int
lrvg_draw_rect(lua_State* L) {
	bool filling = lua_toboolean(L, 1);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float w = luaL_checknumber(L, 4);
	float h = luaL_checknumber(L, 5);
	s2_rvg_draw_rect(filling, x, y, w, h);
	return 0;
}

static int
lrvg_draw_circle(lua_State* L) {
	bool filling = lua_toboolean(L, 1);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float radius = luaL_checknumber(L, 4);
	int segments;
	if (lua_isnoneornil(L, 5)) {
		segments = radius > 10 ? (int) (radius) : 10;
	} else {
		segments = luaL_checkinteger(L, 5);
	}
	s2_rvg_draw_circle(filling, x, y, radius, segments);

	return 0;
}

int
luaopen_s2_c(lua_State* L) {
	luaL_Reg l[] = {
		{ "get_actor_count", lget_actor_count },

		{ "new_sym_model", lnew_sym_model },

		{ "new_spr", lnew_spr },
		{ "new_spr_by_id", lnew_spr_by_id },
		{ "new_spr_from_file", lnew_spr_from_file },
		{ "release_spr", lrelease_spr },	

		{ "fetch_spr_cached", lfetch_spr_cached },
		{ "return_spr_cached", lreturn_spr_cached },

		{ "rt_fetch", lrt_fetch },
		{ "rt_return", lrt_return }, 
		{ "rt_draw_from", lrt_draw_from },
		{ "rt_draw_to", lrt_draw_to },
		{ "rt_get_texid", lrt_get_texid },

		{ "cam_create", lcam_create },
		{ "cam_release", lcam_release },
		{ "cam_bind", lcam_bind },
		{ "cam_set", lcam_set },
		{ "cam_screen2project", lcam_screen2project },

		{ "p3d_buffer_draw", lp3d_buffer_draw },

		{ "rvg_set_color", lrvg_set_color },
		{ "rvg_set_line_width", lrvg_set_line_width },
		{ "rvg_draw_line", lrvg_draw_line },
		{ "rvg_draw_rect", lrvg_draw_rect },
		{ "rvg_draw_circle", lrvg_draw_circle },

		{ NULL, NULL },
	};
	luaL_newlib(L, l);

	lmethod(L);
	lua_setfield(L, -2, "method");
 	lgetter(L);
 	lua_setfield(L, -2, "get");
	lsetter(L);
	lua_setfield(L, -2, "set");

	return 1;
}
#include "dust_shaderlab.h"
#include "dust_camera25.h"

#include <c_wrap_sl.h>
#include <c_wrap_gum.h>

#include <string.h>

void 
dust_sl_init() {
	static bool inited = false;
	if (inited) {
		return;
	}

	sl_create(gum_get_render_context());

	sl_create_shader(ST_SHAPE2);
	sl_create_shader(ST_SPRITE2);
	sl_create_shader(ST_SPRITE3);
	sl_create_shader(ST_BLEND);
	sl_create_shader(ST_FILTER);
	sl_create_shader(ST_MASK);
	sl_create_shader(ST_MODEL3);

	sl_filter_set_heat_haze_factor(0.025f, 0.25f);

	inited = true;
}

void 
dust_sl_on_size(int w, int h) {
	// 	sl_on_projection2(2, 2);
	// 	sl_on_modelview2(-2*0.5f, 2*0.5f, 1, 1);

	sl_on_projection2(w, h);
	sl_on_modelview2(0, 0, 1, 1);

	dust_cam25_create((float)w / h);
	sl_on_projection3(dust_cam25_get_proj_mat());
	sl_on_modelview3(dust_cam25_get_mv_mat());

	sl_set_viewport(0, 0, w, h);	
}
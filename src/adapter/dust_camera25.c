#include "dust_camera25.h"

#include <c25_camera.h>
#include <c_wrap_sl.h>
#include <sm_c_vector.h>

#include <stddef.h>

static struct c25_camera* CAM = NULL;
static bool IS_ENABLE = true;

void 
dust_cam25_enable(bool enable) {
	IS_ENABLE = enable;
}

bool 
dust_cam25_is_closed() {
	return !IS_ENABLE;
}

void 
dust_cam25_create(float aspect) {
	if (CAM) {
		c25_cam_release(CAM);
	}

	struct sm_vec3 pos = { 0, 0, 0 };
	CAM = c25_cam_create(&pos, 0, aspect);
}

void 
dust_cam25_release() {
	if (CAM) {
		c25_cam_release(CAM);
        CAM = NULL;
	}
}

const union sm_mat4* 
dust_cam25_get_mv_mat() {
	return CAM ? c25_cam_get_modelview_mat(CAM) : NULL;
}

const union sm_mat4* 
dust_cam25_get_proj_mat() {
	return CAM ? c25_cam_get_project_mat(CAM) : NULL;
}

void 
dust_cam25_translate(struct sm_vec3* offset) {
	if (CAM) {
		c25_cam_translate(CAM, offset);
		sl_on_modelview3(dust_cam25_get_mv_mat());
	}
}

void 
dust_cam25_set_pos(const struct sm_vec3* pos) {
	if (CAM) {
		c25_cam_set_pos(CAM, pos);
		sl_on_modelview3(dust_cam25_get_mv_mat());
	}
}

float 
dust_cam25_get_angle() {
	if (CAM) {
		return c25_cam_get_angle(CAM);
	} else {
		return 0;
	}
}

void 
dust_cam25_set_angle(float angle) {
	if (CAM) {
		c25_cam_set_angle(CAM, angle);
	}
}

void 
dust_cam25_screen_to_world(struct sm_vec2* world, const struct sm_ivec2* screen) {
// 	if (CAM) {
// 		float w, h, scale;
// 		dtex_get_screen(&w, &h, &scale);
// 		c25_screen_to_world(CAM, world, screen, w, h);
// 	}
}
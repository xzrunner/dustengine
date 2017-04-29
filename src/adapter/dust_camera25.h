#ifndef dust_camera25_h
#define dust_camera25_h

#include <stdbool.h>

union sm_mat4;
struct sm_vec3;
struct sm_vec2;
struct sm_ivec2;

void dust_cam25_enable(bool enable);
bool dust_cam25_is_closed();

void dust_cam25_create(float aspect);
void dust_cam25_release();

const union sm_mat4* dust_cam25_get_mv_mat();
const union sm_mat4* dust_cam25_get_proj_mat();

void dust_cam25_translate(struct sm_vec3* offset);
void dust_cam25_set_pos(const struct sm_vec3* pos);

float dust_cam25_get_angle();
void dust_cam25_set_angle(float angle);

void dust_cam25_screen_to_world(struct sm_vec2* world, const struct sm_ivec2* screen);

#endif // dust_camera25_h
#include "Cam3dOP.h"

#include <painting3/Camera.h>
#include <painting3/Viewport.h>

namespace rt
{

Cam3dOP::Cam3dOP(pt3::Camera& cam, pt3::Viewport& vp)
	: m_cam(cam)
	, m_vp(vp)
{
}

void Cam3dOP::OnMouseLeftDown(float x, float y)
{
	m_is_last_left = true;
	m_last_pos.Set(x, y);
}

void Cam3dOP::OnMouseRightDown(float x, float y)
{
	m_is_last_left = false;
	m_last_pos.Set(x, y);
}

void Cam3dOP::OnMouseDrag(float x, float y)
{
	// rotate
	if (m_is_last_left)
	{
		float dx = x - m_last_pos.x;
		float dy = y - m_last_pos.y;

		m_cam.Yaw(-dx * 0.2f);
		m_cam.Pitch(-dy * 0.2f);
		m_cam.AimAtTarget();
		m_cam.SetUpDir(sm::vec3(0, 1, 0));

		m_last_pos.Set(x, y);
	}
	// translate
	else
	{
		float dx = x - m_last_pos.x;
		float dy = y - m_last_pos.y;

		float dist = m_cam.GetDistance();
		static const float SCALE = 0.002f;
		m_cam.Translate(dx * dist * SCALE, dy * dist * SCALE);

		m_last_pos.Set(x, y);
	}
}

void Cam3dOP::OnMouseWheelRotation(float x, float y, float offx, float offy, float speed)
{
	sm::vec2 pos(static_cast<float>(x), static_cast<float>(y));
	sm::vec3 dir = m_vp.TransPos3ScreenToDir(pos, m_cam);
	m_cam.Move(dir, m_cam.GetDistance() * offy * speed);
}

}
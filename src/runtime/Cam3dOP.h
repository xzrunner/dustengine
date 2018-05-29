#pragma once

#include "EditOP.h"

#include <SM_Vector.h>

namespace pt3 { class Camera; class Viewport; }

namespace rt
{

class Cam3dOP : public EditOP
{
public:
	Cam3dOP(pt3::Camera& cam, pt3::Viewport& vp);

	virtual void OnMouseLeftDown(float x, float y) override;
	virtual void OnMouseRightDown(float x, float y) override;
	virtual void OnMouseDrag(float x, float y) override;
	virtual void OnMouseWheelRotation(float x, float y, float offx, float offy) override;

private:
	pt3::Camera&   m_cam;
	pt3::Viewport& m_vp;

	bool m_is_last_left;
	sm::vec2 m_last_pos;

}; // Cam3dOP

}
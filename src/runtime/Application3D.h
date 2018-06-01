#pragma once

#include "Application.h"

#include <painting3/Camera.h>
#include <painting3/Viewport.h>
#include <painting3/ViewFrustum.h>

namespace rt
{

class Application3D : public Application
{
public:
	Application3D(const std::string& title);

protected:
	void UpdateProjMat();
	void UpdateModelViewMat();

protected:
	pt3::Camera      m_camera;
	pt3::Viewport    m_viewport;
	pt3::ViewFrustum m_frustum;

}; // Application3D

}
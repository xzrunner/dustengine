#pragma once

#include "Application.h"

#include <painting3/Camera.h>
#include <painting3/Viewport.h>

namespace rt
{

class Application3D : public Application
{
public:
	Application3D(const std::string& title);

private:
	pt3::Camera   m_camera;
	pt3::Viewport m_viewport;

}; // Application3D

}
 #include "Application3D.h"

#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>

namespace rt
{

Application3D::Application3D(const std::string& title)
	: Application(title)
	, m_camera(sm::vec3(0, 2, -2), sm::vec3(0, 0, 0), sm::vec3(0, 1, 0))
{
	auto wc = std::make_shared<pt3::WindowContext>();
	wc->Bind();
	pt3::Blackboard::Instance()->SetWindowContext(wc);

	UpdateProjMat();
	UpdateModelViewMat();
}

void Application3D::UpdateProjMat()
{
	auto wc = std::make_shared<pt3::WindowContext>();

	wc->SetScreen(WIDTH, HEIGHT);
	m_viewport.SetSize(WIDTH, HEIGHT);

	m_camera.SetAspect((float)WIDTH / HEIGHT);
	wc->SetProjection(m_camera.GetProjectionMat());
}

void Application3D::UpdateModelViewMat()
{
	auto wc = std::make_shared<pt3::WindowContext>();
	wc->SetModelView(m_camera.GetModelViewMat());
}

}
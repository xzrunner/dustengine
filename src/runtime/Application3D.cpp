#include "Application3D.h"
#include "Cam3dOP.h"

#include <unirender/RenderContext.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <facade/RenderContext.h>

namespace rt
{

Application3D::Application3D(const std::string& title)
	: Application(title)
	, m_camera(sm::vec3(0, 2, -2), sm::vec3(0, 0, 0), sm::vec3(0, 1, 0))
{
	m_rc->GetUrRc().EnableDepth(true);

	m_editop = std::make_unique<Cam3dOP>(m_camera, m_viewport);

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
	m_viewport.SetSize((float)WIDTH, (float)HEIGHT);

	m_camera.SetAspect((float)WIDTH / HEIGHT);
	wc->SetProjection(m_camera.GetProjectionMat());
}

void Application3D::UpdateModelViewMat()
{
	auto wc = std::make_shared<pt3::WindowContext>();
	wc->SetModelView(m_camera.GetModelViewMat());
}

}
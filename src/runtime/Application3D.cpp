#include "Application3D.h"
#include "Cam3dOP.h"

#include <unirender/RenderContext.h>
#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <facade/RenderContext.h>

namespace rt
{

Application3D::Application3D(const std::string& title, bool has2d)
	: Application(title)
	, m_camera(sm::vec3(0, 2, -2), sm::vec3(0, 0, 0), sm::vec3(0, 1, 0))
{
	auto& rc = m_rc->GetUrRc();
	rc.EnableDepth(true);
	rc.SetCull(ur::CULL_BACK);

	m_editop = std::make_unique<Cam3dOP>(m_camera, m_viewport);

	auto wc = std::make_shared<pt3::WindowContext>();
	wc->Bind();
	pt3::Blackboard::Instance()->SetWindowContext(wc);

	UpdateProjMat();
	UpdateModelViewMat();

	if (has2d)
	{
		auto wc = std::make_shared<pt2::WindowContext>();
		wc->Bind();
		pt2::Blackboard::Instance()->SetWindowContext(wc);

		wc->SetViewport(0, 0, WIDTH, HEIGHT);
		wc->SetScreen(WIDTH, HEIGHT);
		wc->SetProjection(WIDTH, HEIGHT);

		wc->SetModelView(sm::vec2(0, 0), 1);
	}
}

void Application3D::UpdateProjMat()
{
	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();

	wc->SetScreen(WIDTH, HEIGHT);
	m_viewport.SetSize((float)WIDTH, (float)HEIGHT);

	m_camera.SetAspect((float)WIDTH / HEIGHT);
	wc->SetProjection(m_camera.GetProjectionMat());
}

void Application3D::UpdateModelViewMat()
{
	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
	wc->SetModelView(m_camera.GetModelViewMat());
}

}
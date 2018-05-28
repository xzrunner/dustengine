#include "Application2D.h"

#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>

namespace rt
{

Application2D::Application2D(const std::string& title)
	: Application(title)
{
	auto wc = std::make_shared<pt2::WindowContext>();
	wc->Bind();
	pt2::Blackboard::Instance()->SetWindowContext(wc);

	InitProjection();
	InitModelView();
}

void Application2D::InitProjection()
{
	auto& wc = pt2::Blackboard::Instance()->GetWindowContext();
	wc->SetViewport(0, 0, WIDTH, HEIGHT);
	wc->SetScreen(WIDTH, HEIGHT);
	wc->SetProjection(WIDTH, HEIGHT);
}

void Application2D::InitModelView()
{
	auto& wc = pt2::Blackboard::Instance()->GetWindowContext();
	wc->SetModelView(sm::vec2(0, 0), 1);
}

}